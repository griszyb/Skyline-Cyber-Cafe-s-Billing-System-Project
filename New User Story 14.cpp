// New User Story 14.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>   // for input/output
#include <fstream>    // file input/output operations (reading/writing user data)
#include <string>     // to use the string class for handling text data
#include <ctime>      // for handling time functions (getting current time for user ID and join date)
#include <iomanip>    // provides input/output manipulators
#include <sstream>    // for string stream classes
#include <cstdlib>    // for random number generation
#include <chrono>     // used for session timing
#include <vector>     // for managing collections of strings
#include <algorithm>  // for std::remove_if

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
        strftime(buffer, sizeof(buffer), "%d/%m/%Y %H:%M:%S", &localTime); // format: day/month/year

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
        // format: userId name email password joinDate
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

    // save admin email in admins.txt
    ofstream adminFile("admins.txt", ios::app);
    if (!adminFile.is_open()) {
        cout << "Failed to open admins.txt for writing.\n";
        return;
    }
    adminFile << email << "\n";
    adminFile.close();

    // also save admin as a user in users.txt so they can login
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
        // Note: this assumes no spaces in name for simplicity

        if (storedEmail == email && storedPassword == password) {
            userId = id;
            userEmail = storedEmail;
            sessionStartTime = steady_clock::now();
            cout << "Login successful! Welcome " << name << "\n";
            return true;
        }
    }
    cout << "Invalid email or password.\n";
    return false;
}

// handles the print job input and calculates cost
double handlePrintJob(const string& userId) {
    int pages = 0;
    cout << "Enter number of pages to print: ";
    cin >> pages;
    if (pages < 0) {
        cout << "Invalid number of pages.\n";
        return 0.0;
    }
    double cost = pages * COST_PER_PAGE;
    logPrintJob(userId, pages, cost);
    cout << "Print cost: $" << fixed << setprecision(2) << cost << "\n";
    return cost;
}

// handles the scan job input and calculates cost
double handleScanJob(const string& userId) {
    int items = 0;
    cout << "Enter number of items to scan: ";
    cin >> items;
    if (items < 0) {
        cout << "Invalid number of items.\n";
        return 0.0;
    }
    double cost = items * COST_PER_SCAN;
    logScanJob(userId, items, cost);
    cout << "Scan cost: $" << fixed << setprecision(2) << cost << "\n";
    return cost;
}

// displays a detailed billing summary at the end of a session
void displayBillingSummary(int totalSeconds, double timeCost, double printCost, double scanCost, double totalAmount) {
    int minutes = totalSeconds / 60;
    int seconds = totalSeconds % 60;

    cout << "\n--- Billing Summary ---\n";
    cout << "Session duration: " << minutes << " minutes " << seconds << " seconds\n";
    cout << "Time cost: $" << fixed << setprecision(2) << timeCost << "\n";
    cout << "Print cost: $" << printCost << "\n";
    cout << "Scan cost: $" << scanCost << "\n";
    cout << "Total amount due: $" << totalAmount << "\n";
    cout << "-----------------------\n";
}

// retrieves and shows cumulative billing total for the user
void showUserBillingTotal(const string& userId) {
    ifstream billingFile("billing_totals.txt");
    string line;
    bool found = false;

    while (getline(billingFile, line)) {
        istringstream iss(line);
        string id;
        double total;
        iss >> id >> total;
        if (id == userId) {
            cout << "Your cumulative billed amount: $" << fixed << setprecision(2) << total << "\n";
            found = true;
            break;
        }
    }
    if (!found) {
        cout << "No billing records found for you.\n";
    }
}

// admin function: view all registered users
void adminViewUsers() {
    ifstream file("users.txt");
    string line;
    cout << "\n--- Registered Users ---\n";
    while (getline(file, line)) {
        istringstream iss(line);
        string id, name, email, password, joinDate;
        iss >> id >> ws;
        getline(iss, name, ' '); // for names without spaces only (simplification)
        iss >> email >> password;
        cout << "ID: " << id << ", Name: " << name << ", Email: " << email << "\n";
    }
    cout << "-------------------------\n";
}

// admin function: view detailed report of a specific user
void adminViewUserReport() {
    string targetUserId;
    cout << "Enter User ID to view report: ";
    cin >> targetUserId;

    // Total session time, print count, scan count, and total billed amount initialization
    int totalSessionSeconds = 0;
    int totalPrintPages = 0;
    int totalScanItems = 0;
    double totalBilled = 0.0;

    // Read session logs and sum session durations for user
    ifstream sessionFile("session_logs.txt");
    string line;
    while (getline(sessionFile, line)) {
        if (line.find("UserID: " + targetUserId) != string::npos) {
            // parse minutes and seconds from line: "Duration: X min Y sec"
            size_t durPos = line.find("Duration:");
            if (durPos != string::npos) {
                int min = 0, sec = 0;
                sscanf_s(line.c_str() + durPos, "Duration: %d min %d sec", &min, &sec);
                totalSessionSeconds += (min * 60 + sec);
            }
            // parse total amount from line
            size_t totalPos = line.find("Total: $");
            if (totalPos != string::npos) {
                double amt = 0.0;
                sscanf_s(line.c_str() + totalPos, "Total: $%lf", &amt);
                totalBilled += amt;
            }
        }
    }
    sessionFile.close();

    // read print logs and sum pages for user
    ifstream printFile("print_logs.txt");
    while (getline(printFile, line)) {
        if (line.find("UserID: " + targetUserId) != string::npos) {
            int pages = 0;
            size_t pagesPos = line.find("Pages:");
            if (pagesPos != string::npos) {
                sscanf_s(line.c_str() + pagesPos, "Pages: %d", &pages);
                totalPrintPages += pages;
            }
        }
    }
    printFile.close();

    // read scan logs and sum items for user
    ifstream scanFile("scan_logs.txt");
    while (getline(scanFile, line)) {
        if (line.find("UserID: " + targetUserId) != string::npos) {
            int items = 0;
            size_t itemsPos = line.find("Items:");
            if (itemsPos != string::npos) {
                sscanf_s(line.c_str() + itemsPos, "Items: %d", &items);
                totalScanItems += items;
            }
        }
    }
    scanFile.close();

    // display report
    cout << "\n--- User Report for ID: " << targetUserId << " ---\n";
    cout << "Total session time: " << totalSessionSeconds / 60 << " minutes and " << totalSessionSeconds % 60 << " seconds\n";
    cout << "Total printed pages: " << totalPrintPages << "\n";
    cout << "Total scanned items: " << totalScanItems << "\n";
    cout << "Total billed amount: $" << fixed << setprecision(2) << totalBilled << "\n";
    cout << "----------------------------------------------\n";
}

// admin function: edit or delete a user
void adminEditDeleteUser() {
    string targetUserId;
    cout << "Enter User ID to edit/delete: ";
    cin >> targetUserId;

    // Read all users into a vector for editing
    ifstream file("users.txt");
    if (!file.is_open()) {
        cout << "Failed to open users file.\n";
        return;
    }

    vector<string> lines;
    string line;
    bool userFound = false;
    string currentName, currentEmail, currentPassword, currentJoinDate;

    while (getline(file, line)) {
        if (line.find(targetUserId) == 0) {
            userFound = true;
            // parse user line: id name email password joinDate
            istringstream iss(line);
            string id, name, email, password, joinDate;
            iss >> id >> ws;
            getline(iss, name, ' ');  // name - simplified, assumes no spaces in name
            iss >> email >> password;
            size_t pos = line.find(password);
            if (pos != string::npos) {
                joinDate = line.substr(pos + password.length() + 1);
            }
            currentName = name;
            currentEmail = email;
            currentPassword = password;
            currentJoinDate = joinDate;
        }
        lines.push_back(line);
    }
    file.close();

    if (!userFound) {
        cout << "User ID not found.\n";
        return;
    }

    cout << "Current user details:\n";
    cout << "Name: " << currentName << "\n";
    cout << "Email: " << currentEmail << "\n";
    cout << "Password: " << currentPassword << "\n";
    cout << "Join Date: " << currentJoinDate << "\n";

    cout << "Do you want to (1) Edit or (2) Delete this user? Enter 1 or 2: ";
    int choice;
    cin >> choice;

    if (choice == 1) {
        cout << "Enter new name (or '.' to keep unchanged): ";
        string newName;
        cin.ignore();
        getline(cin, newName);
        if (newName != ".") {
            currentName = newName;
        }

        cout << "Enter new email (or '.' to keep unchanged): ";
        string newEmail;
        getline(cin, newEmail);
        if (newEmail != ".") {
            // check for duplicate email except current user
            bool emailExistsFlag = false;
            for (const auto& l : lines) {
                if (l.find(targetUserId) == 0) continue; // skip current user
                if (l.find(newEmail) != string::npos) {
                    emailExistsFlag = true;
                    break;
                }
            }
            if (emailExistsFlag) {
                cout << "Email already used by another user. Edit aborted.\n";
                return;
            }
            currentEmail = newEmail;
        }

        cout << "Enter new password (or '.' to keep unchanged): ";
        string newPassword;
        getline(cin, newPassword);
        if (newPassword != ".") {
            if (!validatePassword(newPassword)) {
                cout << "Password too short. Edit aborted.\n";
                return;
            }
            currentPassword = newPassword;
        }

        // rebuild user line
        stringstream updatedLine;
        updatedLine << targetUserId << " " << currentName << " " << currentEmail << " " << currentPassword << " " << currentJoinDate;

        // update vector with new user info
        for (auto& l : lines) {
            if (l.find(targetUserId) == 0) {
                l = updatedLine.str();
                break;
            }
        }

        // rewrite users.txt file
        ofstream outFile("users.txt");
        for (const auto& l : lines) {
            outFile << l << "\n";
        }
        outFile.close();

        cout << "User details updated successfully.\n";
    }
    else if (choice == 2) {
        cout << "Are you sure you want to delete this user and all associated data? (yes/no): ";
        string confirm;
        cin >> confirm;
        if (confirm != "yes") {
            cout << "Deletion cancelled.\n";
            return;
        }

        // remove user line from lines vector
        lines.erase(remove_if(lines.begin(), lines.end(),
            [&](const string& l) { return l.find(targetUserId) == 0; }),
            lines.end());

        // rewrite users.txt without the deleted user
        ofstream outFile("users.txt");
        for (const auto& l : lines) {
            outFile << l << "\n";
        }
        outFile.close();

        // remove from billing_totals.txt
        ifstream billingIn("billing_totals.txt");
        vector<string> billingLines;
        while (getline(billingIn, line)) {
            if (line.find(targetUserId) != 0) {
                billingLines.push_back(line);
            }
        }
        billingIn.close();

        ofstream billingOut("billing_totals.txt");
        for (const auto& l : billingLines) {
            billingOut << l << "\n";
        }
        billingOut.close();

        // remove related entries from session_logs.txt
        ifstream sessionIn("session_logs.txt");
        vector<string> sessionLines;
        while (getline(sessionIn, line)) {
            if (line.find("UserID: " + targetUserId) == string::npos) {
                sessionLines.push_back(line);
            }
        }
        sessionIn.close();

        ofstream sessionOut("session_logs.txt");
        for (const auto& l : sessionLines) {
            sessionOut << l << "\n";
        }
        sessionOut.close();

        // remove related entries from print_logs.txt
        ifstream printIn("print_logs.txt");
        vector<string> printLines;
        while (getline(printIn, line)) {
            if (line.find("UserID: " + targetUserId) == string::npos) {
                printLines.push_back(line);
            }
        }
        printIn.close();

        ofstream printOut("print_logs.txt");
        for (const auto& l : printLines) {
            printOut << l << "\n";
        }
        printOut.close();

        // remove related entries from scan_logs.txt
        ifstream scanIn("scan_logs.txt");
        vector<string> scanLines;
        while (getline(scanIn, line)) {
            if (line.find("UserID: " + targetUserId) == string::npos) {
                scanLines.push_back(line);
            }
        }
        scanIn.close();

        ofstream scanOut("scan_logs.txt");
        for (const auto& l : scanLines) {
            scanOut << l << "\n";
        }
        scanOut.close();

        cout << "User and all associated data deleted successfully.\n";
    }
    else {
        cout << "Invalid choice.\n";
    }
}

// user menu after login (print, scan, logout)
void userMenu(const string& userId) {
    steady_clock::time_point sessionStart = steady_clock::now();
    double totalPrintCost = 0.0;
    double totalScanCost = 0.0;

    while (true) {
        cout << "\nUser Menu:\n";
        cout << "1. Print\n";
        cout << "2. Scan\n";
        cout << "3. Logout\n";
        cout << "Choose an option: ";
        int option;
        cin >> option;

        if (option == 1) {
            totalPrintCost += handlePrintJob(userId);
        }
        else if (option == 2) {
            totalScanCost += handleScanJob(userId);
        }
        else if (option == 3) {
            steady_clock::time_point sessionEnd = steady_clock::now();
            auto sessionDuration = duration_cast<seconds>(sessionEnd - sessionStart);
            int totalSeconds = (int)sessionDuration.count();

            double timeCost = totalSeconds * COST_PER_MINUTE;
            double totalAmount = timeCost + totalPrintCost + totalScanCost;

            displayBillingSummary(totalSeconds, timeCost, totalPrintCost, totalScanCost, totalAmount);
            logSession(userId, totalSeconds / 60, totalSeconds % 60, timeCost, totalPrintCost, totalScanCost, totalAmount);
            updateCumulativeBilling(userId, totalAmount);

            break;
        }
        else {
            cout << "Invalid option.\n";
        }
    }
}

// admin menu after login
void adminMenu() {
    while (true) {
        cout << "\nAdmin Menu:\n";
        cout << "1. View all users\n";
        cout << "2. View user report\n";
        cout << "3. Edit or delete a user\n";
        cout << "4. Logout\n";
        cout << "Choose an option: ";
        int option;
        cin >> option;

        if (option == 1) {
            adminViewUsers();
        }
        else if (option == 2) {
            adminViewUserReport();
        }
        else if (option == 3) {
            adminEditDeleteUser();
        }
        else if (option == 4) {
            break;
        }
        else {
            cout << "Invalid option.\n";
        }
    }
}

int main() {
    cout << "Welcome to Skyline Cyber Café Billing System\n";

    while (true) {
        cout << "\nMain Menu:\n";
        cout << "1. Register User\n";
        cout << "2. Register Admin\n";
        cout << "3. Login\n";
        cout << "4. Exit\n";
        cout << "Choose an option: ";
        int choice;
        cin >> choice;

        if (choice == 1) {
            registerUser();
        }
        else if (choice == 2) {
            registerAdmin();
        }
        else if (choice == 3) {
            string userId, userEmail;
            steady_clock::time_point sessionStart;
            if (loginUser(userId, userEmail, sessionStart)) {
                if (isAdmin(userEmail)) {
                    adminMenu();
                }
                else {
                    userMenu(userId);
                }
            }
        }
        else if (choice == 4) {
            cout << "Exiting program. Goodbye!\n";
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
