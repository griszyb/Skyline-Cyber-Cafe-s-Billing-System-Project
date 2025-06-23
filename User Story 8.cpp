// User Story 8.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>   // standard input (cin) and output (cout) streams
#include <fstream>    // file input/output operations (reading/writing user data)
#include <string>     // to use the string class for handling text data
#include <ctime>      // for handling time functions (getting current time for user ID and join date)
#include <iomanip>    // provides input/output manipulators
#include <sstream>    // for string stream classes
#include <cstdlib>    // for random number generation
#include <chrono>     // used for session timing

using namespace std; // so that i dont have type std again
using namespace chrono; //so that i dont have to type chrono again

const double COST_PER_PAGE = 2.00;  // cost per printed page in dollars
const double COST_PER_SCAN = 1.00;  // cost per scanned item in dollars

// validates password with basic rules (at least 8 characters)
bool validatePassword(const string& password) {
    return password.length() >= 8;
}

// generates a unique user ID based on the current timestamp
string generateUniqueId() {
    time_t now = time(0);           // get current time
    return to_string(now);          // convert to string to use as unique ID
}

// checks if email already exists in the users.txt file to prevent duplicate registration
bool emailExists(const string& email) {
    ifstream file("users.txt");    // open the users file for reading
    string line;
    while (getline(file, line)) {  // read the file line by line
        if (line.find(email) != string::npos) { // if email found anywhere in line
            return true;            // email already registered
        }
    }
    return false;                   // email not found, OK to register
}

// gets the current date and time formatted as a string for join date
string getJoinDate() {
    time_t now = time(0);           // current time
    char dateStr[100];
    errno_t err = ctime_s(dateStr, sizeof(dateStr), &now);  // converts to readable string safely
    if (err != 0) {
        cout << "Error getting current time.\n";
        return "";                 // return empty string if error occurs
    }
    return string(dateStr);        // return formatted date/time string (with newline)
}

// logs the session duration, print and scan costs to session_logs.txt
void logSession(const string& userId, int minutes, int seconds, double printCost, double scanCost) {
    ofstream logFile("session_logs.txt", ios::app);  // Append mode
    if (logFile.is_open()) {
        time_t now = time(0);
        tm localTime;
        localtime_s(&localTime, &now);
        char buffer[80];
        strftime(buffer, sizeof(buffer), "%d/%m/%Y %H:%M:%S", &localTime); // Format: day/month/year

        logFile << "UserID: " << userId
            << " | Duration: " << minutes << " min " << seconds << " sec"
            << " | Print: $" << fixed << setprecision(2) << printCost
            << " | Scan: $" << fixed << setprecision(2) << scanCost
            << " | Date: " << buffer << "\n";

        logFile.close();
    }
    else {
        cout << "Failed to write session log.\n";
    }
}

// logs print job details to print_logs.txt
void logPrintJob(const string& userId, int pages, double cost) {
    ofstream printLog("print_logs.txt", ios::app);
    if (printLog.is_open()) {
        time_t now = time(0);
        tm localTime;
        localtime_s(&localTime, &now);
        char buffer[80];
        strftime(buffer, sizeof(buffer), "%d/%m/%Y %H:%M:%S", &localTime); // Format: day/month/year

        printLog << "UserID: " << userId
            << " | Pages: " << pages
            << " | Cost: $" << fixed << setprecision(2) << cost
            << " | Date: " << buffer << "\n";

        printLog.close();
    }
    else {
        cout << "Failed to write print job.\n";
    }
}

// logs scan job details to scan_logs.txt
void logScanJob(const string& userId, int items, double cost) {
    ofstream scanLog("scan_logs.txt", ios::app);
    if (scanLog.is_open()) {
        time_t now = time(0);
        tm localTime;
        localtime_s(&localTime, &now);
        char buffer[80];
        strftime(buffer, sizeof(buffer), "%d/%m/%Y %H:%M:%S", &localTime); // Format: day/month/year

        scanLog << "UserID: " << userId
            << " | Items: " << items
            << " | Cost: $" << fixed << setprecision(2) << cost
            << " | Date: " << buffer << "\n";

        scanLog.close();
    }
    else {
        cout << "Failed to write scan job.\n";
    }
}

// handles user registration: input, validation, and saving user data
void registerUser() {
    string name, email, password;
    cout << "\n--- User Registration ---\n";

    cout << "Enter your full name: ";
    getline(cin, name);            // read full line including spaces

    cout << "Enter your email: ";
    getline(cin, email);

    // check if email already registered
    if (emailExists(email)) {
        cout << "Email already registered. Try logging in.\n";
        return;
    }

    cout << "Enter your password (min 8 characters): ";
    getline(cin, password);

    // validate password length
    if (!validatePassword(password)) {
        cout << "Password too short.\n";
        return;
    }

    // generate unique ID and join date
    string userId = generateUniqueId();
    string joinDate = getJoinDate();
    if (joinDate.empty()) return;

    // saves user data to file
    ofstream file("users.txt", ios::app);
    if (file.is_open()) {
        file << userId << " " << name << " " << email << " " << password << " " << joinDate;
        file.close();
        cout << "Registration successful! Your user ID is: " << userId << endl;
    }
    else {
        cout << "Error saving user data.\n";
    }
}

// handles user login by validating credentials and recording session start time
bool loginUser(string& userId, steady_clock::time_point& sessionStartTime) {
    string email, password;
    cout << "\n--- User Login ---\n";

    cout << "Enter your email: ";
    getline(cin, email);
    cout << "Enter your password: ";
    getline(cin, password);

    ifstream file("users.txt");
    if (!file.is_open()) {
        cout << "Error accessing user database.\n";
        return false;
    }

    string line;
    while (getline(file, line)) {
        istringstream iss(line);
        string storedId, name, storedEmail, storedPassword;

        // extract user ID, name, email, and password from the line, using space as the separator
        getline(iss, storedId, ' ');
        getline(iss, name, ' ');
        getline(iss, storedEmail, ' ');
        getline(iss, storedPassword, ' ');

        // validate credentials
        if (email == storedEmail && password == storedPassword) {
            userId = storedId;                  // assign user ID for session tracking
            sessionStartTime = steady_clock::now();  // record login time for session duration
            cout << "Login successful! Welcome back.\n";
            return true;
        }
    }

    cout << "Login failed. Email or password incorrect.\n";
    return false;
}

// lets user print documents during session, records page count and updates print cost
void printDocuments(const string& userId, double& totalPrintCost) {
    cout << "\n--- Print Documents ---\n";
    cout << "Enter number of pages to print: ";
    string input;
    getline(cin, input);

    int pages = 0;
    try {
        pages = stoi(input);
        if (pages <= 0) throw invalid_argument("invalid");
    }
    catch (...) {
        cout << "Invalid input. Please enter a positive number.\n";
        return;
    }

    double cost = pages * COST_PER_PAGE;
    totalPrintCost += cost;
    logPrintJob(userId, pages, cost);

    cout << "Printed " << pages << " pages. Cost: $" << fixed << setprecision(2) << cost << "\n";
}

// lets user scan documents during session, records item count and updates scan cost
void scanDocuments(const string& userId, double& totalScanCost) {
    cout << "\n--- Scan Documents ---\n";
    cout << "Enter number of items to scan: ";
    string input;
    getline(cin, input);

    int items = 0;
    try {
        items = stoi(input);
        if (items <= 0) throw invalid_argument("invalid");
    }
    catch (...) {
        cout << "Invalid input. Please enter a positive number.\n";
        return;
    }

    double cost = items * COST_PER_SCAN;
    totalScanCost += cost;
    logScanJob(userId, items, cost);

    cout << "Scanned " << items << " item(s). Cost: $" << fixed << setprecision(2) << cost << "\n";
}

// provides user services menu after login with options to check session time, view session logs, print, scan, or logout
void accessServices(const string& userId, const steady_clock::time_point& sessionStartTime) {
    double totalPrintCost = 0.0;
    double totalScanCost = 0.0;

    while (true) {
        cout << "\n--- Skyline Cyber Cafe Services ---\n";
        cout << "User ID: " << userId << endl;
        cout << "1. Check live session time\n2. View past session logs\n3. Print documents\n4. Scan documents\n5. Logout\nChoose an option: ";
        string choice;
        getline(cin, choice);

        if (choice == "1") {
            auto now = steady_clock::now();
            auto duration = duration_cast<seconds>(now - sessionStartTime);
            int totalSeconds = static_cast<int>(duration.count());
            int minutes = totalSeconds / 60;
            int seconds = totalSeconds % 60;

            cout << "Session time: " << minutes << " minute(s) and " << seconds << " second(s).\n";
        }
        else if (choice == "2") {
            ifstream logFile("session_logs.txt");
            if (!logFile.is_open()) {
                cout << "No session logs found.\n";
            }
            else {
                cout << "\n--- Your Session History ---\n";
                string line;
                bool found = false;
                while (getline(logFile, line)) {
                    if (line.find("UserID: " + userId) != string::npos) {
                        cout << line << endl;
                        found = true;
                    }
                }
                if (!found) {
                    cout << "No previous sessions found.\n";
                }
            }
        }
        else if (choice == "3") {
            printDocuments(userId, totalPrintCost);
        }
        else if (choice == "4") {
            scanDocuments(userId, totalScanCost);
        }
        else if (choice == "5") {
            auto now = steady_clock::now();
            auto duration = duration_cast<seconds>(now - sessionStartTime);
            int totalSeconds = static_cast<int>(duration.count());
            int minutes = totalSeconds / 60;
            int seconds = totalSeconds % 60;

            logSession(userId, minutes, seconds, totalPrintCost, totalScanCost);

            cout << "Logging out. Session lasted: " << minutes << " min, " << seconds << " sec.\n";
            cout << "Print charges: $" << fixed << setprecision(2) << totalPrintCost << "\n";
            cout << "Scan charges: $" << fixed << setprecision(2) << totalScanCost << "\n";
            break;
        }
        else {
            cout << "Invalid option. Please choose between 1 to 5.\n";
        }
    }
}

int main() { //menu
    while (true) {
        cout << "\n====== Skyline Cyber Cafe ======\n";
        cout << "1. Register\n2. Login\n3. Exit\nChoose an option: ";
        string choice;
        getline(cin, choice);

        if (choice == "1") {
            registerUser();
        }
        else if (choice == "2") {
            string userId;
            steady_clock::time_point sessionStartTime;

            if (loginUser(userId, sessionStartTime)) {
                accessServices(userId, sessionStartTime);
            }
        }
        else if (choice == "3") {
            cout << "Goodbye!\n";
            break;
        }
        else {
            cout << "Invalid option. Please choose 1, 2, or 3.\n";
        }
    }

    return 0;
}


// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
