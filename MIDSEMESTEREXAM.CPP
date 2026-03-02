#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

using namespace std;

class Student
{
public:
    string indexNumber;
    string name;
    Student(string idx, string n) : indexNumber(idx), name(n) {}
    void display()
    {
        cout << "Index: " << indexNumber << ", Name: " << name << endl;
    }
};

struct Attendance
{
    string index;
    char status; // P, A, L
};

vector<Student> students;
vector<Attendance> currentSession;
string currentSessionName = "";

void saveStudents()
{
    ofstream file("students.txt");
    for (auto &s : students)
    {
        file << s.indexNumber << "," << s.name << "\n";
    }
    file.close();
}

void loadStudents()
{
    ifstream file("students.txt");
    string line;
    while (getline(file, line))
    {
        stringstream ss(line);
        string idx, name;
        getline(ss, idx, ',');
        getline(ss, name);
        if (!idx.empty())
        {
            students.push_back(Student(idx, name));
        }
    }
    file.close();
}

bool studentExists(string idx)
{
    for (auto &s : students)
    {
        if (s.indexNumber == idx)
            return true;
    }
    return false;
}

void registerStudent()
{
    string idx, name;
    cout << "Enter index number: ";
    cin >> idx;
    cin.ignore();
    cout << "Enter name: ";
    getline(cin, name);

    if (studentExists(idx))
    {
        cout << "Already registered.\n";
        return;
    }

    students.push_back(Student(idx, name));
    saveStudents();
    cout << "Student added.\n";
}

void viewStudents()
{
    if (students.empty())
    {
        cout << "No students yet.\n";
        return;
    }
    for (auto &s : students)
    {
        s.display();
    }
}

void searchStudent()
{
    string idx;
    cout << "Enter index number: ";
    cin >> idx;
    cin.ignore();

    for (auto &s : students)
    {
        if (s.indexNumber == idx)
        {
            s.display();
            return;
        }
    }
    cout << "Not found.\n";
}

void createSession()
{
    string course, date, time;
    cout << "Course code: ";
    cin >> course;
    cout << "Date (YYYY-MM-DD): ";
    cin >> date;
    cout << "Start time (HH:MM): ";
    cin >> time;

    currentSessionName = course + "_" + date + "_" + time + ".txt";
    // replace : with -
    for (char &c : currentSessionName)
    {
        if (c == ':')
            c = '-';
    }

    currentSession.clear();
    cout << "Session created: " << currentSessionName << "\n";
}

void markAttendance()
{
    if (currentSessionName.empty())
    {
        cout << "No session. Create one first.\n";
        return;
    }

    string idx;
    char status;

    cout << "Mark attendance (type 'done' to finish)\n";

    while (true)
    {
        cout << "Index: ";
        cin >> idx;
        if (idx == "done")
            break;

        if (!studentExists(idx))
        {
            cout << "Student not found.\n";
            continue;
        }

        cout << "Status (P/A/L): ";
        cin >> status;
        status = toupper(status);

        if (status != 'P' && status != 'A' && status != 'L')
        {
            cout << "Invalid.\n";
            continue;
        }

        // simple update or add
        bool found = false;
        for (auto &r : currentSession)
        {
            if (r.index == idx)
            {
                r.status = status;
                found = true;
                break;
            }
        }
        if (!found)
        {
            currentSession.push_back({idx, status});
        }

        cout << "Recorded.\n";
    }

    // save
    ofstream file("sessions/" + currentSessionName);
    for (auto &r : currentSession)
    {
        file << r.index << "," << r.status << "\n";
    }
    file.close();
    cout << "Saved.\n";
}

void viewAttendance()
{
    if (currentSessionName.empty())
    {
        cout << "No session active.\n";
        return;
    }

    cout << "\nAttendance for " << currentSessionName << ":\n";
    int p = 0, a = 0, l = 0;

    for (auto &r : currentSession)
    {
        cout << r.index << " -> " << r.status << endl;
        if (r.status == 'P')
            p++;
        else if (r.status == 'A')
            a++;
        else if (r.status == 'L')
            l++;
    }

    cout << "\nPresent: " << p << "   Absent: " << a << "   Late: " << l << endl;
}

void exportCurrentSessionCSV()
{
    if (currentSessionName.empty())
    {
        cout << "No session active.\n";
        return;
    }

    string outname;
    cout << "Enter CSV filename (or press enter to use default): ";
    getline(cin, outname);
    if (outname.empty())
    {
        outname = currentSessionName;
        size_t pos = outname.rfind(".txt");
        if (pos != string::npos)
            outname.replace(pos, 4, ".csv");
    }

    ofstream csv(outname);
    if (!csv.is_open())
    {
        cout << "Cannot create CSV file.\n";
        return;
    }

    csv << "Index,Name,Status\n";
    for (auto &r : currentSession)
    {
        string name = "";
        for (auto &s : students)
        {
            if (s.indexNumber == r.index)
            {
                name = s.name;
                break;
            }
        }
        if (name.find(',') != string::npos)
            name = '"' + name + '"';
        csv << r.index << "," << name << "," << r.status << "\n";
    }
    csv.close();
    cout << "Exported to " << outname << "\n";
}

int main()
{
    system("if not exist sessions mkdir sessions");
    loadStudents();

    int choice;
    do
    {
        cout << "\n1. Register student\n";
        cout << "2. View students\n";
        cout << "3. Search student\n";
        cout << "4. Create session\n";
        cout << "5. Mark attendance\n";
        cout << "6. View current attendance\n";
        cout << "7. Export current session to CSV\n";
        cout << "0. Exit\n";
        cout << "Choice: ";
        cin >> choice;
        cin.ignore();

        if (choice == 1)
            registerStudent();
        else if (choice == 2)
            viewStudents();
        else if (choice == 3)
            searchStudent();
        else if (choice == 4)
            createSession();
        else if (choice == 5)
            markAttendance();
        else if (choice == 6)
            viewAttendance();
        else if (choice == 7)
            exportCurrentSessionCSV();
        else if (choice == 0)
            cout << "Goodbye!\n";
        else
            cout << "Wrong choice.\n";

    } while (choice != 0);

    return 0;
}
