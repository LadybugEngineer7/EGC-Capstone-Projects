#include <algorithm>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#define _CRT_SECURE_NO_WARNINGS

using namespace std;

struct Ward {
    string name;
    int capacity;
};

struct Patient {
    string id;
    string name;
    int age = 0;
    string symptomCode;
    int priority = 3;
    string status = "Waiting";
    string ward = "-";
};

const string PATIENTS_FILE = "patients.txt";
const string AUDIT_FILE = "hospital_audit.txt";

string cleanField(string s) {
    replace(s.begin(), s.end(), '|', '/');
    return s;
}

string readLine(const string& prompt) {
    cout << prompt;
    string value;
    getline(cin, value);
    return value;
}

string readRequired(const string& prompt) {
    while (true) {
        string value = cleanField(readLine(prompt));
        if (!value.empty()) return value;
        cout << "This field cannot be empty.\n";
    }
}

int readInt(const string& prompt, int minimum, int maximum) {
    while (true) {
        cout << prompt;
        string line;
        getline(cin, line);
        stringstream ss(line);
        int value;
        char extra;
        if (ss >> value && !(ss >> extra) && value >= minimum && value <= maximum)
            return value;
        cout << "Invalid input. Enter a number from " << minimum << " to " << maximum << ".\n";
    }
}
string timestampNow() {
    time_t now = time(nullptr);

    tm local{};
    if (localtime_s(&local, &now) != 0) {
        return "Unknown time";
    }

    ostringstream out;
    out << put_time(&local, "%Y-%m-%d %H:%M:%S");

    return out.str();
}

void audit(const string& action, const Patient& p, const string& details) {
    ofstream file(AUDIT_FILE, ios::app);
    if (file)
        file << '[' << timestampNow() << "] " << action
        << " | Patient ID: " << p.id << " | Name: " << p.name
        << " | " << details << '\n';
    else
        cerr << "Warning: could not write to audit log.\n";
}

int findPatientIndex(const vector<Patient>& patients, const string& id) {
    for (int i = 0; i < (int)patients.size(); ++i)
        if (patients[i].id == id) return i;
    return -1;
}

int findWardIndex(const vector<Ward>& wards, const string& name) {
    for (int i = 0; i < (int)wards.size(); ++i)
        if (wards[i].name == name) return i;
    return -1;
}

int wardOccupancy(const vector<Patient>& patients, const string& wardName) {
    int count = 0;
    for (const auto& p : patients)
        if (p.status == "Admitted" && p.ward == wardName) ++count;
    return count;
}

void sortQueue(vector<Patient>& patients) {
    stable_sort(patients.begin(), patients.end(), [](const Patient& a, const Patient& b) {
        if (a.status != b.status) return a.status == "Waiting";
        if (a.priority != b.priority) return a.priority < b.priority;
        return a.id < b.id;
        });
}

void savePatients(const vector<Patient>& patients) {
    ofstream file(PATIENTS_FILE);
    for (const auto& p : patients)
        file << p.id << '|' << p.name << '|' << p.age << '|'
        << p.symptomCode << '|' << p.priority << '|'
        << p.status << '|' << p.ward << '\n';
}

void loadPatients(vector<Patient>& patients) {
    ifstream file(PATIENTS_FILE);
    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        Patient p;
        string age, priority;
        if (getline(ss, p.id, '|') && getline(ss, p.name, '|') &&
            getline(ss, age, '|') && getline(ss, p.symptomCode, '|') &&
            getline(ss, priority, '|') && getline(ss, p.status, '|') &&
            getline(ss, p.ward)) {
            try {
                p.age = stoi(age);
                p.priority = stoi(priority);
                if (p.status == "Waiting" || p.status == "Admitted")
                    if (findPatientIndex(patients, p.id) == -1) patients.push_back(p);
            }
            catch (...) {

            }
        }
    }
    sortQueue(patients);
}

void printPatientTable(const vector<Patient>& patients, bool waitingOnly = false) {
    cout << left << setw(12) << "ID" << setw(22) << "Name"
        << setw(6) << "Age" << setw(14) << "Priority"
        << setw(12) << "Status" << "Ward\n";
    cout << string(78, '-') << '\n';
    bool any = false;
    for (const auto& p : patients) {
        if (waitingOnly && p.status != "Waiting") continue;
        any = true;
        string priorityText = p.priority == 1 ? "1-Critical" :
            p.priority == 2 ? "2-Urgent" : "3-Stable";
        cout << left << setw(12) << p.id << setw(22) << p.name
            << setw(6) << p.age << setw(14) << priorityText
            << setw(12) << p.status << p.ward << '\n';
    }
    if (!any) cout << "No matching patients.\n";
}

void showOccupancy(const vector<Patient>& patients, const vector<Ward>& wards) {
    cout << "\nWARD OCCUPANCY REPORT\n";
    cout << left << setw(18) << "Ward" << setw(14) << "Occupied"
        << setw(14) << "Capacity" << "Available\n";
    cout << string(58, '-') << '\n';
    for (const auto& ward : wards) {
        int occupied = wardOccupancy(patients, ward.name);
        cout << left << setw(18) << ward.name << setw(14) << occupied
            << setw(14) << ward.capacity << ward.capacity - occupied << '\n';
    }
}

void admitPatient(vector<Patient>& patients) {
    Patient p;
    p.id = readRequired("Assign patient ID: ");
    if (findPatientIndex(patients, p.id) != -1) {
        cout << "That ID is already in use by an active patient.\n";
        return;
    }
    p.name = readRequired("Patient name: ");
    p.age = readInt("Age (0-120): ", 0, 120);
    p.symptomCode = readRequired("Symptom code/brief symptom: ");
    p.priority = readInt("Triage priority (1=Critical, 2=Urgent, 3=Stable): ", 1, 3);
    p.status = "Waiting";
    p.ward = "-";
    patients.push_back(p);
    sortQueue(patients);
    audit("ADMISSION", p, "Registered in triage queue; priority=" + to_string(p.priority));
    cout << "Patient registered and added to the triage queue.\n";
}

void assignBed(vector<Patient>& patients, vector<Ward>& wards) {
    string id = readRequired("Patient ID to assign: ");
    int pi = findPatientIndex(patients, id);
    if (pi == -1) { cout << "Active patient not found.\n"; return; }
    Patient& p = patients[pi];
    if (p.status == "Admitted") { cout << "Patient already has a bed in " << p.ward << ".\n"; return; }

    showOccupancy(patients, wards);
    cout << "Available wards: ICU, General, Emergency\n";
    string wardName = readRequired("Ward name (use exact spelling): ");
    int wi = findWardIndex(wards, wardName);
    if (wi == -1) { cout << "Unknown ward name.\n"; return; }
    if (wardOccupancy(patients, wards[wi].name) >= wards[wi].capacity) {
        cout << "No beds available in that ward.\n"; return;
    }
    p.status = "Admitted";
    p.ward = wards[wi].name;
    audit("BED ASSIGNMENT", p, "Assigned to ward " + p.ward);
    sortQueue(patients);
    cout << "Bed assigned successfully.\n";
}

void changePriority(vector<Patient>& patients) {
    string id = readRequired("Patient ID: ");
    int pi = findPatientIndex(patients, id);
    if (pi == -1) { cout << "Active patient not found.\n"; return; }
    Patient& p = patients[pi];
    int oldPriority = p.priority;
    p.priority = readInt("New priority (1=Critical, 2=Urgent, 3=Stable): ", 1, 3);
    audit("STATUS CHANGE", p, "Priority changed from " + to_string(oldPriority) +
        " to " + to_string(p.priority));
    sortQueue(patients);
    cout << "Priority updated and queue re-sorted.\n";
}

void updateSymptomStatus(vector<Patient>& patients) {
    string id = readRequired("Patient ID: ");
    int pi = findPatientIndex(patients, id);
    if (pi == -1) { cout << "Active patient not found.\n"; return; }
    Patient& p = patients[pi];
    string oldSymptom = p.symptomCode;
    p.symptomCode = readRequired("Updated symptom/status note: ");
    audit("STATUS CHANGE", p, "Clinical note changed from [" + oldSymptom +
        "] to [" + p.symptomCode + "]");
    cout << "Patient note updated.\n";
}

void dischargePatient(vector<Patient>& patients) {
    string id = readRequired("Patient ID to discharge: ");
    int pi = findPatientIndex(patients, id);
    if (pi == -1) { cout << "Active patient not found.\n"; return; }
    Patient p = patients[pi];
    audit("DISCHARGE", p, "Discharged; previous status=" + p.status +
        "; previous ward=" + p.ward);
    patients.erase(patients.begin() + pi);
    cout << "Patient discharged. Any assigned bed is now available.\n";
}

void receptionistMenu(vector<Patient>& patients, vector<Ward>& wards) {
    bool back = false;
    while (!back) {
        cout << "\n--- RECEPTIONIST MENU ---\n"
            << "1. Register/admit incoming patient to triage queue\n"
            << "2. View triage queue\n"
            << "3. Assign patient to a ward/bed\n"
            << "4. Discharge patient\n"
            << "5. View ward occupancy report\n"
            << "0. Back to role selection\n";
        int choice = readInt("Choose: ", 0, 5);
        if (choice == 1) admitPatient(patients);
        else if (choice == 2) printPatientTable(patients, true);
        else if (choice == 3) assignBed(patients, wards);
        else if (choice == 4) dischargePatient(patients);
        else if (choice == 5) showOccupancy(patients, wards);
        else back = true;
        sortQueue(patients);
        savePatients(patients);
    }
}

void doctorMenu(vector<Patient>& patients, vector<Ward>& wards) {
    bool back = false;
    while (!back) {
        cout << "\n--- DOCTOR MENU ---\n"
            << "1. View all active patients\n"
            << "2. Change triage priority\n"
            << "3. Update patient symptom/clinical note\n"
            << "4. View ward occupancy report\n"
            << "5. Discharge patient\n"
            << "0. Back to role selection\n";
        int choice = readInt("Choose: ", 0, 5);
        if (choice == 1) printPatientTable(patients);
        else if (choice == 2) changePriority(patients);
        else if (choice == 3) updateSymptomStatus(patients);
        else if (choice == 4) showOccupancy(patients, wards);
        else if (choice == 5) dischargePatient(patients);
        else back = true;
        sortQueue(patients);
        savePatients(patients);
    }
}

int main() {
    vector<Patient> patients;
    vector<Ward> wards = { {"ICU", 5}, {"General", 20}, {"Emergency", 10} };
    loadPatients(patients);

    cout << "Hospital Patient Triage & Ward Management System\n";
    cout << "Loaded " << patients.size() << " active patient(s).\n";
    bool running = true;
    while (running) {
        cout << "\n========== ROLE SELECTION ==========\n"
            << "1. Receptionist view\n"
            << "2. Doctor view\n"
            << "3. View ward occupancy report\n"
            << "0. Save and exit\n";
        int choice = readInt("Choose: ", 0, 3);
        if (choice == 1) receptionistMenu(patients, wards);
        else if (choice == 2) doctorMenu(patients, wards);
        else if (choice == 3) showOccupancy(patients, wards);
        else running = false;
        sortQueue(patients);
        savePatients(patients);
    }
    cout << "Active patient data saved. Audit events are in " << AUDIT_FILE << ".\n";
    return 0;
}
