#include <algorithm>
#include <cctype>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

struct Course {
    string id;
    string name;
    int creditHours = 0;
    int maxCapacity = 0;
    string prerequisiteId = "-";
};

struct CompletedCourse {
    string courseId;
    char grade = 'F';
};

struct Student {
    string id;
    string name;
    string major;
    int creditLimit = 18;
    vector<string> enrolledCourseIds;
    vector<CompletedCourse> completedCourses;
};

const string COURSES_FILE = "courses.txt";
const string STUDENTS_FILE = "students.txt";

string cleanField(string s) {
    replace(s.begin(), s.end(), '|', '/');
    replace(s.begin(), s.end(), ',', ';');
    return s;
}

string readLine(const string& prompt) {
    cout << prompt;
    string value;
    getline(cin, value);
    return value;
}

int readInt(const string& prompt, int minimum, int maximum) {
    while (true) {
        cout << prompt;
        string line;
        getline(cin, line);
        stringstream ss(line);
        int value;
        char extra;
        if (ss >> value && !(ss >> extra) && value >= minimum && value <= maximum) {
            return value;
        }
        cout << "Invalid input. Enter a number from " << minimum << " to " << maximum << ".\n";
    }
}

string readRequired(const string& prompt) {
    while (true) {
        string value = cleanField(readLine(prompt));
        if (!value.empty()) return value;
        cout << "This field cannot be empty.\n";
    }
}

vector<string> split(const string& text, char delimiter) {
    vector<string> values;
    string item;
    stringstream ss(text);
    while (getline(ss, item, delimiter)) {
        if (!item.empty()) values.push_back(item);
    }
    return values;
}

int findCourseIndex(const vector<Course>& courses, const string& id) {
    for (int i = 0; i < (int)courses.size(); ++i)
        if (courses[i].id == id) return i;
    return -1;
}

int findStudentIndex(const vector<Student>& students, const string& id) {
    for (int i = 0; i < (int)students.size(); ++i)
        if (students[i].id == id) return i;
    return -1;
}

double gradePoint(char grade) {
    switch (toupper(static_cast<unsigned char>(grade))) {
    case 'A': return 4.0;
    case 'B': return 3.0;
    case 'C': return 2.0;
    case 'D': return 1.0;
    default: return 0.0;
    }
}

bool hasCompleted(const Student& student, const string& courseId) {
    for (const auto& item : student.completedCourses)
        if (item.courseId == courseId && gradePoint(item.grade) > 0.0) return true;
    return false;
}

bool isEnrolled(const Student& student, const string& courseId) {
    return find(student.enrolledCourseIds.begin(), student.enrolledCourseIds.end(), courseId)
        != student.enrolledCourseIds.end();
}

int enrolledCount(const vector<Student>& students, const string& courseId) {
    int count = 0;
    for (const auto& student : students)
        if (isEnrolled(student, courseId)) ++count;
    return count;
}

int enrolledCredits(const Student& student, const vector<Course>& courses) {
    int total = 0;
    for (const auto& id : student.enrolledCourseIds) {
        int index = findCourseIndex(courses, id);
        if (index != -1) total += courses[index].creditHours;
    }
    return total;
}

void saveCourses(const vector<Course>& courses) {
    ofstream file(COURSES_FILE);
    for (const auto& c : courses)
        file << c.id << '|' << c.name << '|' << c.creditHours << '|'
        << c.maxCapacity << '|' << c.prerequisiteId << '\n';
}

void saveStudents(const vector<Student>& students) {
    ofstream file(STUDENTS_FILE);
    for (const auto& s : students) {
        file << s.id << '|' << s.name << '|' << s.major << '|' << s.creditLimit << '|';
        for (size_t i = 0; i < s.enrolledCourseIds.size(); ++i) {
            if (i) file << ',';
            file << s.enrolledCourseIds[i];
        }
        file << '|';
        for (size_t i = 0; i < s.completedCourses.size(); ++i) {
            if (i) file << ',';
            file << s.completedCourses[i].courseId << ':' << s.completedCourses[i].grade;
        }
        file << '\n';
    }
}

void loadCourses(vector<Course>& courses) {
    ifstream file(COURSES_FILE);
    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        Course c;
        string credits, capacity;
        if (getline(ss, c.id, '|') && getline(ss, c.name, '|') &&
            getline(ss, credits, '|') && getline(ss, capacity, '|') &&
            getline(ss, c.prerequisiteId)) {
            try {
                c.creditHours = stoi(credits);
                c.maxCapacity = stoi(capacity);
                if (findCourseIndex(courses, c.id) == -1) courses.push_back(c);
            }
            catch (...) {
                // Ignore malformed rows rather than crashing.
            }
        }
    }
}

void loadStudents(vector<Student>& students) {
    ifstream file(STUDENTS_FILE);
    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        Student s;
        string limit, enrolled, completed;
        if (getline(ss, s.id, '|') && getline(ss, s.name, '|') &&
            getline(ss, s.major, '|') && getline(ss, limit, '|') &&
            getline(ss, enrolled, '|') && getline(ss, completed)) {
            try {
                s.creditLimit = stoi(limit);
                s.enrolledCourseIds = split(enrolled, ',');
                for (const auto& record : split(completed, ',')) {
                    size_t colon = record.find(':');
                    if (colon != string::npos && colon + 1 < record.size())
                        s.completedCourses.push_back({ record.substr(0, colon), record[colon + 1] });
                }
                if (findStudentIndex(students, s.id) == -1) students.push_back(s);
            }
            catch (...) {
                // Ignore malformed rows.
            }
        }
    }
}

void listCourses(const vector<Course>& courses, const vector<Student>& students) {
    if (courses.empty()) {
        cout << "No courses in the catalog yet.\n";
        return;
    }
    cout << left << setw(12) << "Course ID" << setw(28) << "Course Name"
        << setw(9) << "Credits" << setw(12) << "Seats" << "Prerequisite\n";
    cout << string(78, '-') << '\n';
    for (const auto& c : courses) {
        cout << left << setw(12) << c.id << setw(28) << c.name
            << setw(9) << c.creditHours
            << (to_string(enrolledCount(students, c.id)) + "/" + to_string(c.maxCapacity))
            << string(max(1, 12 - (int)(to_string(enrolledCount(students, c.id)).size() +
                to_string(c.maxCapacity).size() + 1)), ' ')
            << c.prerequisiteId << '\n';
    }
}

void addCourse(vector<Course>& courses) {
    Course c;
    c.id = readRequired("Course ID (no | or ,): ");
    if (findCourseIndex(courses, c.id) != -1) {
        cout << "That course ID already exists.\n";
        return;
    }
    c.name = readRequired("Course name: ");
    c.creditHours = readInt("Credit hours (1-6): ", 1, 6);
    c.maxCapacity = readInt("Maximum capacity (1-500): ", 1, 500);
    c.prerequisiteId = readLine("Prerequisite course ID, or - if none: ");
    if (c.prerequisiteId.empty()) c.prerequisiteId = "-";
    if (c.prerequisiteId != "-" && findCourseIndex(courses, c.prerequisiteId) == -1) {
        cout << "That prerequisite is not in the catalog. Course not added.\n";
        return;
    }
    if (c.prerequisiteId == c.id) {
        cout << "A course cannot be its own prerequisite.\n";
        return;
    }
    courses.push_back(c);
    cout << "Course added.\n";
}

void registerStudent(vector<Student>& students) {
    Student s;
    s.id = readRequired("Student ID: ");
    if (findStudentIndex(students, s.id) != -1) {
        cout << "That student ID already exists.\n";
        return;
    }
    s.name = readRequired("Full name: ");
    s.major = readRequired("Major code: ");
    s.creditLimit = readInt("Starting credit limit (1-30): ", 1, 30);
    students.push_back(s);
    cout << "Student registered.\n";
}

void printSchedule(const Student& student, const vector<Course>& courses) {
    cout << "\nSchedule for " << student.name << " (" << student.id << ")\n";
    cout << left << setw(12) << "Course ID" << setw(30) << "Course Name" << "Credits\n";
    cout << string(52, '-') << '\n';
    if (student.enrolledCourseIds.empty()) cout << "No courses registered.\n";
    for (const auto& id : student.enrolledCourseIds) {
        int i = findCourseIndex(courses, id);
        if (i != -1)
            cout << left << setw(12) << courses[i].id << setw(30) << courses[i].name
            << courses[i].creditHours << '\n';
    }
    cout << "Enrolled credits: " << enrolledCredits(student, courses)
        << " / " << student.creditLimit << '\n';
}

void enrollStudent(vector<Student>& students, const vector<Course>& courses) {
    string studentId = readRequired("Student ID: ");
    int si = findStudentIndex(students, studentId);
    if (si == -1) { cout << "Student not found.\n"; return; }

    string courseId = readRequired("Course ID to add: ");
    int ci = findCourseIndex(courses, courseId);
    if (ci == -1) { cout << "Course not found.\n"; return; }

    Student& s = students[si];
    const Course& c = courses[ci];
    if (isEnrolled(s, c.id)) { cout << "Student is already enrolled in this course.\n"; return; }
    if (hasCompleted(s, c.id)) { cout << "Student has already completed this course.\n"; return; }
    if (enrolledCount(students, c.id) >= c.maxCapacity) { cout << "Course is full.\n"; return; }
    if (enrolledCredits(s, courses) + c.creditHours > s.creditLimit) {
        cout << "This would exceed the student's credit limit.\n"; return;
    }
    if (c.prerequisiteId != "-" && !hasCompleted(s, c.prerequisiteId)) {
        cout << "Prerequisite not satisfied. The student must pass "
            << c.prerequisiteId << " first.\n";
        return;
    }
    s.enrolledCourseIds.push_back(c.id);
    cout << "Enrollment successful.\n";
}

void dropCourse(vector<Student>& students) {
    string studentId = readRequired("Student ID: ");
    int si = findStudentIndex(students, studentId);
    if (si == -1) { cout << "Student not found.\n"; return; }
    string courseId = readRequired("Course ID to drop: ");
    auto& ids = students[si].enrolledCourseIds;
    auto it = find(ids.begin(), ids.end(), courseId);
    if (it == ids.end()) { cout << "That course is not on the student's schedule.\n"; return; }
    ids.erase(it);
    cout << "Course dropped.\n";
}

void recordGrade(vector<Student>& students, const vector<Course>& courses) {
    string studentId = readRequired("Student ID: ");
    int si = findStudentIndex(students, studentId);
    if (si == -1) { cout << "Student not found.\n"; return; }
    string courseId = readRequired("Completed course ID: ");
    int ci = findCourseIndex(courses, courseId);
    if (ci == -1) { cout << "Course not found in catalog.\n"; return; }

    const vector<string> allowed = { "A", "B", "C", "D", "F" };
    string gradeText;
    while (true) {
        gradeText = readRequired("Grade (A/B/C/D/F): ");
        transform(gradeText.begin(), gradeText.end(), gradeText.begin(),
            [](unsigned char ch) { return static_cast<char>(toupper(ch)); });
        if (gradeText.size() == 1 &&
            find(allowed.begin(), allowed.end(), gradeText) != allowed.end()) break;
        cout << "Please enter A, B, C, D, or F.\n";
    }

    Student& s = students[si];
    s.enrolledCourseIds.erase(remove(s.enrolledCourseIds.begin(), s.enrolledCourseIds.end(), courseId),
        s.enrolledCourseIds.end());
    auto it = find_if(s.completedCourses.begin(), s.completedCourses.end(),
        [&](const CompletedCourse& cc) { return cc.courseId == courseId; });
    if (it == s.completedCourses.end()) s.completedCourses.push_back({ courseId, gradeText[0] });
    else it->grade = gradeText[0];
    cout << "Grade saved. (F is recorded as not passed.)\n";
}

double calculateGPA(const Student& student, const vector<Course>& courses, int& totalCredits) {
    double weightedPoints = 0.0;
    totalCredits = 0;
    for (const auto& completed : student.completedCourses) {
        int ci = findCourseIndex(courses, completed.courseId);
        if (ci == -1) continue;
        int credits = courses[ci].creditHours;
        totalCredits += credits;
        weightedPoints += gradePoint(completed.grade) * credits;
    }
    return totalCredits == 0 ? 0.0 : weightedPoints / totalCredits;
}

void showGPA(const Student& student, const vector<Course>& courses) {
    int credits = 0;
    double gpa = calculateGPA(student, courses, credits);
    cout << fixed << setprecision(2)
        << "Cumulative GPA: " << gpa << " / 4.00\n"
        << "Completed credit hours: " << credits << '\n';
}

void generateTranscript(const Student& student, const vector<Course>& courses) {
    string filename = "transcript_" + student.id + ".txt";
    ofstream file(filename);
    if (!file) { cout << "Could not create transcript file.\n"; return; }
    int totalCredits = 0;
    double gpa = calculateGPA(student, courses, totalCredits);
    file << "UNOFFICIAL ACADEMIC TRANSCRIPT\n"
        << "Student ID: " << student.id << "\nName: " << student.name
        << "\nMajor: " << student.major << "\n\nCompleted Courses\n";
    file << left << setw(14) << "Course ID" << setw(30) << "Course Name"
        << setw(10) << "Credits" << "Grade\n";
    file << string(64, '-') << '\n';
    for (const auto& cc : student.completedCourses) {
        int ci = findCourseIndex(courses, cc.courseId);
        if (ci == -1) continue;
        file << left << setw(14) << cc.courseId << setw(30) << courses[ci].name
            << setw(10) << courses[ci].creditHours << cc.grade << '\n';
    }
    file << fixed << setprecision(2) << "\nCumulative GPA: " << gpa
        << " / 4.00\nCompleted credit hours: " << totalCredits << '\n';
    cout << "Transcript saved to " << filename << '\n';
}

void listStudents(const vector<Student>& students) {
    if (students.empty()) { cout << "No students registered.\n"; return; }
    cout << left << setw(14) << "Student ID" << setw(28) << "Name"
        << setw(16) << "Major" << "Credit limit\n";
    cout << string(70, '-') << '\n';
    for (const auto& s : students)
        cout << left << setw(14) << s.id << setw(28) << s.name
        << setw(16) << s.major << s.creditLimit << '\n';
}

int main() {
    vector<Course> courses;
    vector<Student> students;
    loadCourses(courses);
    loadStudents(students);

    cout << "University Course Registration & Academic Advising System\n";
    cout << "Loaded " << students.size() << " student(s) and "
        << courses.size() << " course(s).\n";

    bool running = true;
    while (running) {
        cout << "\n========== MAIN MENU ==========\n"
            << "1. Add course (administrator)\n"
            << "2. View course catalog\n"
            << "3. Register student\n"
            << "4. List students\n"
            << "5. Enroll student in a course\n"
            << "6. Drop a course\n"
            << "7. Print student schedule\n"
            << "8. Record/update a completed course grade\n"
            << "9. Calculate student's GPA\n"
            << "10. Generate unofficial transcript\n"
            << "0. Save and exit\n";
        int choice = readInt("Choose: ", 0, 10);
        if (choice == 1) addCourse(courses);
        else if (choice == 2) listCourses(courses, students);
        else if (choice == 3) registerStudent(students);
        else if (choice == 4) listStudents(students);
        else if (choice == 5) enrollStudent(students, courses);
        else if (choice == 6) dropCourse(students);
        else if (choice == 7) {
            string id = readRequired("Student ID: ");
            int si = findStudentIndex(students, id);
            if (si == -1) cout << "Student not found.\n";
            else printSchedule(students[si], courses);
        }
        else if (choice == 8) recordGrade(students, courses);
        else if (choice == 9) {
            string id = readRequired("Student ID: ");
            int si = findStudentIndex(students, id);
            if (si == -1) cout << "Student not found.\n";
            else showGPA(students[si], courses);
        }
        else if (choice == 10) {
            string id = readRequired("Student ID: ");
            int si = findStudentIndex(students, id);
            if (si == -1) cout << "Student not found.\n";
            else generateTranscript(students[si], courses);
        }
        else running = false;

        saveCourses(courses);
        saveStudents(students);
    }
    cout << "Data saved. Goodbye!\n";
    return 0;
}
