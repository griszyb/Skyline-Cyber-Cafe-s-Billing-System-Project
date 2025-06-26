// User Story 12.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>   // for input/output
#include <fstream>    // file input/output operations (reading/writing user data)
#include <string>     // to use the string class for handling text data
#include <ctime>      // for handling time functions (getting current time for user ID and join date)
#include <iomanip>    // provides input/output manipulators
#include <sstream>    // for string stream classes
#include <cstdlib>    // for random number generation
#include <chrono>     // used for session timing

using namespace std; // so that i dont have to type std again
using namespace chrono; // so that i dont have to type chrono again

// cost constants for each service
const double COST_PER_PAGE = 0.50;   // cost per printed page in dollars
const double COST_PER_SCAN = 0.20;   // cost per scanned item in dollars
const double COST_PER_MINUTE = 0.20; // cost per minute of session time

// validates password with basic rules (at least 8 characters)
bool validatePassword(const string& password) {
    return password.length() >= 8;
}

// generates a unique user ID based on the current timestamp
string generateUniqueId() {
    time_t now = time(0);
    return to_string(now);
}

// checks if email already exists in the users.txt file to prevent duplicate registration
bool emailExists(const string& email) {
    ifstream file("users.txt");
    string line;
    while (getline(file, line)) {
        istringstream iss(line);
        string id, name, storedEmail, password, joinDate;
        iss >> id >> ws;
        getline(iss, name, ' ');
        iss >> storedEmail >> password;
        if (storedEmail == email) {
            return true;
        }
    }
    return false;
}

// checks if email already exists in admins.txt to prevent duplicate admin registration
bool adminEmailExists(const string& email) {
    ifstream file("admins.txt");
    string line;
    while (getline(file, line)) {
        if (line == email) {
            return true;
        }
    }
    return false;
}

// gets the current date and time formatted as a string for join date
string getJoinDate() {
    time_t now = time(0);
    char dateStr[100];
    errno_t err = ctime_s(dateStr, sizeof(dateStr), &now); // converts to readable string safely
    if (err != 0) {
        cout << "Error getting current time.\n";
        return "";
    }
    return string(dateStr);
}

// logs the session billing summary to session_logs.txt
void logSession(const string& userId, int minutes, int seconds, double timeCost, double printCost, double scanCost, double total) {
    ofstream logFile("session_logs.txt", ios::app);  // Append mode
    if (logFile.is_open()) {
        time_t now = time(0);
        tm localTime;
        localtime_s(&localTime, &now);
        char buffer[80];
        strftime(buffer, sizeof(buffer), "%d/%m/%Y %H:%M:%S", &localTime); // Format: day/month/year

        logFile << "UserID: " << userId
            << " | Duration: " << minutes << " min " << seconds << " sec"
            << " | Time Cost: $" << fixed << setprecision(2) << timeCost
            << " | Print: $" << printCost
            << " | Scan: $" << scanCost
            << " | Total: $" << total
            << " | Date: " << buffer << "\n";

        logFile.close();
    }
    else {
        cout << "Failed to write session log.\n";
    }
}

// updates or adds user's cumulative billing amount in billing_totals.txt
void updateCumulativeBilling(const string& userId, double sessionTotal) {
    ifstream inFile("billing_totals.txt"); // open file for reading
    string line;
    ostringstream updatedContent;
    bool found = false;

    // read file and update existing total if found
    while (getline(inFile, line)) {
        istringstream iss(line);
        string id;
        double total;
        iss >> id >> total;
        if (id == userId) {
            total += sessionTotal; // add this session's total
            updatedContent << id << " " << fixed << setprecision(2) << total << "\n";
            found = true;
        }
        else {
            updatedContent << line << "\n";
        }
    }
    inFile.close();

    // append new user if not found
    if (!found) {
        updatedContent << userId << " " << fixed << setprecision(2) << sessionTotal << "\n";
    }

    ofstream outFile("billing_totals.txt"); // overwrite with updated data
    outFile << updatedContent.str();
    outFile.close();
}

// logs print job details to print_logs.txt
void logPrintJob(const string& userId, int pages, double cost) {
    ofstream printLog("print_logs.txt", ios::app);
    if (printLog.is_open()) {
        time_t now = time(0);
        tm localTime;
        localtime_s(&localTime, &now);
        char buffer[80];
        strftime(buffer, sizeof(buffer), "%d/%m/%Y %H:%M:%S", &localTime);

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
        strftime(buffer, sizeof(buffer), "%d/%m/%Y %H:%M:%S", &localTime);

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

// checks if the email belongs to an admin user
bool isAdmin(const string& email) {
    ifstream adminFile("admins.txt");
    string line;
    while (getline(adminFile, line)) {
        if (line == email) {
            return true;
        }
    }
    return false;
}

// handles user registration: input, validation, and saving user data
void registerUser() {
    string name, email, password, userId, joinDate;

    cout << "Enter full name: ";
    cin.ignore();
    getline(cin, name);

    cout << "Enter email: ";
    cin >> email;

    if (emailExists(email)) {
        cout << "Email already registered.\n";
        return;
    }

    cout << "Enter password (min 8 characters): ";
    cin >> password;

    if (!validatePassword(password)) {
        cout << "Password too short.\n";
        return;
    }

    userId = generateUniqueId();
    joinDate = getJoinDate();

    ofstream file("users.txt", ios::app);
    if (file.is_open()) {
        // Format: userId name email password joinDate
        file << userId << " " << name << " " << email << " " << password << " " << joinDate;
        file.close();
        cout << "Registration successful! Your user ID: " << userId << "\n";
    }
    else {
        cout << "Failed to save user.\n";
    }
}

// handles admin registration: input, validation, and saving admin email and user data
void registerAdmin() {
    string name, email, password, userId, joinDate;

    cout << "Enter full name: ";
    cin.ignore();
    getline(cin, name);

    cout << "Enter admin email: ";
    cin >> email;

    if (emailExists(email) || adminEmailExists(email)) {
        cout << "Email already registered as user or admin.\n";
        return;
    }

    cout << "Enter password (min 8 characters): ";
    cin >> password;

    if (!validatePassword(password)) {
        cout << "Password too short.\n";
        return;
    }

    // Save admin email in admins.txt
    ofstream adminFile("admins.txt", ios::app);
    if (!adminFile.is_open()) {
        cout << "Failed to open admins.txt for writing.\n";
        return;
    }
    adminFile << email << "\n";
    adminFile.close();

    // Also save admin as a user in users.txt so they can login (with name)
    userId = generateUniqueId();
    joinDate = getJoinDate();

    ofstream userFile("users.txt", ios::app);
    if (!userFile.is_open()) {
        cout << "Failed to open users.txt for writing.\n";
        return;
    }
    userFile << userId << " " << name << " " << email << " " << password << " " << joinDate;
    userFile.close();

    cout << "Admin registration successful! Your user ID: " << userId << "\n";
}

// handles user login by validating credentials and recording session start time
bool loginUser(string& userId, string& userEmail, steady_clock::time_point& sessionStartTime) {
    string email, password;

    cout << "Enter email: ";
    cin >> email;
    cout << "Enter password: ";
    cin >> password;

    ifstream file("users.txt");
    string line;
    while (getline(file, line)) {
        istringstream iss(line);
        string id, name, storedEmail, storedPassword, joinDate;

        iss >> id;
        iss >> ws;
        getline(iss, name, ' '); // read name until first space

        // restart parsing:
        istringstream lineStream(line);
        lineStream >> id >> name >> storedEmail >> storedPassword;
        getline(lineStream, joinDate);

        if (storedEmail == email && storedPassword == password) {
            userId = id;
            userEmail = storedEmail;
            sessionStartTime = steady_clock::now();
            cout << "Login successful!\n";
            return true;
        }
    }

    cout << "Invalid email or password.\n";
    return false;
}

// provides printing option and adds to session cost
void printDocuments(const string& userId, double& totalPrintCost) {
    int pages;
    cout << "Enter number of pages to print: ";
    cin >> pages;

    if (pages <= 0) {
        cout << "Invalid page number.\n";
        return;
    }

    double cost = pages * COST_PER_PAGE;
    totalPrintCost += cost;
    logPrintJob(userId, pages, cost);
    cout << "Printed " << pages << " pages for $" << fixed << setprecision(2) << cost << "\n";
}

// provides scanning option and adds to session cost
void scanDocuments(const string& userId, double& totalScanCost) {
    int items;
    cout << "Enter number of items to scan: ";
    cin >> items;

    if (items <= 0) {
        cout << "Invalid number of items.\n";
        return;
    }

    double cost = items * COST_PER_SCAN;
    totalScanCost += cost;
    logScanJob(userId, items, cost);
    cout << "Scanned " << items << " items for $" << fixed << setprecision(2) << cost << "\n";
}

// main user services menu for printing, scanning, logout
void accessServices(const string& userId, steady_clock::time_point sessionStartTime) {
    double totalPrintCost = 0.0;
    double totalScanCost = 0.0;

    while (true) {
        cout << "\n--- Services Menu ---\n";
        cout << "1. Print Documents\n";
        cout << "2. Scan Documents\n";
        cout << "3. Logout\n";
        cout << "Enter choice: ";

        string choice;
        cin >> choice;

        if (choice == "1") {
            printDocuments(userId, totalPrintCost);
        }
        else if (choice == "2") {
            scanDocuments(userId, totalScanCost);
        }
        else if (choice == "3") {
            steady_clock::time_point sessionEndTime = steady_clock::now();
            duration<double> elapsed_seconds = sessionEndTime - sessionStartTime;
            int totalSeconds = static_cast<int>(elapsed_seconds.count());
            int minutes = totalSeconds / 60;
            int seconds = totalSeconds % 60;

            double timeCost = elapsed_seconds.count() * COST_PER_MINUTE;
            double sessionTotal = timeCost + totalPrintCost + totalScanCost;

            cout << "\nSession Summary:\n";
            cout << "Duration: " << minutes << " minutes, " << seconds << " seconds\n";
            cout << "Time cost: $" << fixed << setprecision(2) << timeCost << "\n";
            cout << "Print cost: $" << totalPrintCost << "\n";
            cout << "Scan cost: $" << totalScanCost << "\n";
            cout << "Total amount due: $" << sessionTotal << "\n";

            logSession(userId, minutes, seconds, timeCost, totalPrintCost, totalScanCost, sessionTotal);
            updateCumulativeBilling(userId, sessionTotal);

            cout << "Logged out successfully.\n";
            break;
        }
        else {
            cout << "Invalid choice. Please try again.\n";
        }
    }
}

// allows user to view their cumulative billing total across sessions
void viewBillingHistory(const string& userId) {
    ifstream file("billing_totals.txt");
    string line;
    bool found = false;

    while (getline(file, line)) {
        istringstream iss(line);
        string id;
        double total;
        iss >> id >> total;

        if (id == userId) {
            cout << "Your cumulative billing total: $" << fixed << setprecision(2) << total << "\n";
            found = true;
            break;
        }
    }

    if (!found) {
        cout << "No billing history found.\n";
    }
}

// Admin function to view all registered users (User Story 12)
void viewAllUsers() {
    ifstream file("users.txt");
    if (!file.is_open()) {
        cout << "No user data found.\n";
        return;
    }

    cout << "\n--- Registered Users ---\n";
    cout << left << setw(15) << "User ID" << setw(25) << "Name" << setw(30) << "Email" << "Join Date\n";
    cout << "---------------------------------------------------------------------------------\n";

    string line;
    while (getline(file, line)) {
        istringstream iss(line);
        string id, name, email, password, joinDate;

        iss >> id;
        iss >> ws;
        getline(iss, name, ' ');

        // to parse correctly, re-parse line
        istringstream lineStream(line);
        lineStream >> id;
        lineStream >> ws;
        getline(lineStream, name, ' ');
        lineStream >> email >> password;
        getline(lineStream, joinDate);

        cout << left << setw(15) << id
            << setw(25) << name
            << setw(30) << email
            << joinDate << "\n";
    }

    file.close();
}

// admin menu with new option to view all registered users
void adminMenu(const string& adminEmail) {
    while (true) {
        cout << "\n--- Admin Menu ---\n";
        cout << "1. View All Registered Users\n";
        cout << "2. Logout\n";
        cout << "Enter choice: ";

        string choice;
        cin >> choice;

        if (choice == "1") {
            viewAllUsers();
        }
        else if (choice == "2") {
            cout << "Admin logged out.\n";
            break;
        }
        else {
            cout << "Invalid choice.\n";
        }
    }
}

// main menu
int main() {
    while (true) {
        cout << "\n--- Skyline Cyber Cafe ---\n";
        cout << "1. Register User\n";
        cout << "2. Register Admin\n";
        cout << "3. Login\n";
        cout << "4. Exit\n";
        cout << "Enter choice: ";

        string choice;
        cin >> choice;

        if (choice == "1") {
            registerUser();
        }
        else if (choice == "2") {
            registerAdmin();
        }
        else if (choice == "3") {
            string userId, userEmail;
            steady_clock::time_point sessionStartTime;

            if (loginUser(userId, userEmail, sessionStartTime)) {
                if (isAdmin(userEmail)) {
                    adminMenu(userEmail);
                }
                else {
                    accessServices(userId, sessionStartTime);
                }
            }
        }
        else if (choice == "4") {
            cout << "Exiting...\n";
            break;
        }
        else {
            cout << "Invalid choice.\n";
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
