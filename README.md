<h1 align="center">EGC C++ Capstone Projects</h1>

<p align="center">
  Two console-based C++ applications built with Problem Solving, C++ Fundamentals, and Object-Oriented Programming (OOP).
</p>

---

## 📖 About

This repository contains two C++ capstone projects developed as part of the EGC academic program. They apply C++ programming concepts to practical problems involving data management, algorithms, and file handling.

> These projects were built after studying **Problem Solving**, **C++**, and **Object-Oriented Programming (OOP)**.

## 🧠 Background & Skills

Before developing these projects, I studied and practiced:

- **Problem Solving:** analyzing problems, breaking them into smaller steps, and designing algorithms (searching, sorting, and validation).
- **C++ Fundamentals:** variables, conditions, loops, functions, arrays, vectors, structs, and file handling.
- **Object-Oriented Programming (OOP):** classes and objects, encapsulation, constructors, and organizing code into reusable components.

These skills were applied directly in both projects to build clean, structured, and maintainable console applications.

---

## 📚 Project 1: University Course Registration & Academic Advising System

### Overview
A console-based application designed to manage student profiles, university course catalogs, course registration, credit limits, prerequisites, and academic performance.

### Objectives
- Manage student information and university course offerings.
- Organize course registration and student schedules.
- Validate course prerequisites and available capacity.
- Calculate cumulative GPA and generate unofficial transcripts.
- Store and retrieve information using text files.

### Functional Requirements

**1. Course Catalog Management**
- Add new courses with course IDs, names, credit hours, maximum capacity, and prerequisite information.
- View available courses.
- Load and save course information using `courses.txt`.

**2. Student Profiles & Enrollment**
- Register students with unique IDs, major codes, and credit limits.
- Enroll students in courses after checking prerequisite requirements and course capacity.
- Allow students to drop courses.
- Display student schedules in a structured table.
- Store and retrieve student information using `students.txt`.

**3. Academic Reporting**
- Calculate cumulative GPA based on completed course grades.
- Generate an unofficial transcript text file for a selected student.

### Grading Scale

| Grade | Points |
|-------|--------|
| A     | 4.0    |
| B     | 3.0    |
| C     | 2.0    |
| D     | 1.0    |
| F     | 0.0    |

### Core C++ Concepts
- Structures (`struct`) and related data models.
- Arrays and `std::vector` collections.
- Functions and pass-by-reference.
- Conditional statements and loops.
- Input validation.
- Searching and prerequisite verification.
- File handling for reading and writing text files.
- Object-Oriented Programming (classes such as `Student` and `Course`).

---

## 🏥 Project 2: Hospital Patient Triage & Ward Management System

### Overview
A console-based hospital management application designed to register patients, prioritize emergency cases, manage ward occupancy, assign beds, and maintain records of hospital activities.

### Objectives
- Organize patient admission and triage.
- Prioritize patients according to their medical urgency.
- Track bed availability across different hospital wards.
- Manage patient assignments and discharges.
- Maintain a record of important hospital transactions.

### Functional Requirements

**1. Patient Admission & Triage**
- Register patients with names, ages, unique IDs, symptom codes, and triage priority levels.
- Priority levels:
  - 🔴 Priority 1: Critical (Red)
  - 🟡 Priority 2: Urgent (Yellow)
  - 🟢 Priority 3: Stable (Green)
- Sort the active patient queue so that critical cases appear first.

**2. Ward & Bed Assignment**
- Track bed availability across ICU, General, and Emergency wards.
- Assign patients to available beds.
- Discharge patients and update bed availability.
- Generate reports showing ward occupancy and available beds.

**3. Medical & Audit Logging**
- Record patient admissions, status changes, and discharges in `hospital_audit.txt`.
- Append new events to the existing log without overwriting previous records (`ios::app`).
- Include timestamps generated with `<ctime>`.

**4. Role-Based Menus**
- **Receptionist menu:** patient registration and administrative tasks.
- **Doctor menu:** viewing patient information and triage priorities.

### Core C++ Concepts
- Structures (`struct`) and arrays or vectors.
- Functions and data organization.
- Conditional statements and loops.
- Searching and sorting algorithms.
- String handling and parsing.
- File handling and append operations using `ios::app`.
- Input validation.
- Object-Oriented Programming (classes such as `Patient` and `Ward`).

---

## 🛠️ Technologies & Tools

- **Language:** C++ (C++17)
- **Application Type:** Console-Based Applications
- **Data Storage:** Text Files
- **Paradigms:** Procedural Programming and Object-Oriented Programming (OOP)
- **Data Organization:** Structures, Arrays, and Vectors
- **Algorithms:** Searching, Sorting, and Data Validation
- **File Handling:** Reading, Writing, and Appending Text Files

## 📂 Project Structure

```
EGC-CPP-Capstone/
├── logo.svg
├── Project1_Course_Registration/
│   ├── main.cpp
│   ├── courses.txt
│   └── students.txt
├── Project2_Hospital_Triage/
│   ├── main.cpp
│   └── hospital_audit.txt
└── README.md
```

## ▶️ How to Build & Run

```bash
# Project 1
cd Project1_Course_Registration
g++ -std=c++17 main.cpp -o course_system
./course_system

# Project 2
cd Project2_Hospital_Triage
g++ -std=c++17 main.cpp -o hospital_system
./hospital_system
```

> On Windows, run `course_system.exe` or `hospital_system.exe` instead.

## 🎯 Learning Objectives

- Applying C++ concepts to practical problems.
- Organizing and managing related data.
- Developing structured and readable code.
- Using functions, loops, and conditional statements.
- Implementing searching and sorting operations.
- Validating user input.
- Reading from and writing to text files.
- Understanding how programming supports academic and hospital management systems.
- Applying Object-Oriented Programming concepts.

## 📌 Project Status

- 🚧 Project 1: In progress
- 🚧 Project 2: In progress

> Update these to ✅ Completed when finished.

---

**Author:** Rahma Gaber
**Academic Program:** EGC
**Programming Language:** C++
