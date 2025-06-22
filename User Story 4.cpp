// User Story 4.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <cstdlib> // for rand
#include <chrono>  // for high-resolution clock

using namespace std;

// validate password with basic rules (at least 8 characters)
bool validatePassword(const string& password) {
    return password.length() >= 8;
}

// generate a unique user ID based on the current timestamp
string generateUniqueId() {
    time_t now = time(0);
    return to_string(now);
}

// checks if email already exists in the users file
bool emailExists(const string& email) {
    ifstream file("users.txt");
    string line;
    while (getline(file, line)) {
        if (line.find(email) != string::npos) {
            return true;
        }
    }
    return false;
}

// get current date and time
string getJoinDate() {
    time_t now = time(0);
    char dateStr[100];
    errno_t err = ctime_s(dateStr, sizeof(dateStr), &now);
    if (err != 0) {
        cout << "Error getting current time." << endl;
        return "";
    }
    return string(dateStr);
}

// generate a random session token
string generateSessionToken() {
    auto now = chrono::high_resolution_clock::now().time_since_epoch().count();
    srand(static_cast<unsigned int>(now));
    string token = "SID-";
    for (int i = 0; i < 8; ++i) {
        char ch = 'A' + rand() % 26;
        token += ch;
    }
    return token;
}

// register a new user
void registerUser() {
    string name, email, password;

    cout << "\n--- User Registration ---\n";
    cout << "Enter your full name: ";
    getline(cin, name);

    cout << "Enter your email: ";
    getline(cin, email);

    if (emailExists(email)) {
        cout << "Email already registered. Try logging in instead.\n";
        return;
    }

    cout << "Enter your password (at least 8 characters): ";
    getline(cin, password);

    if (!validatePassword(password)) {
        cout << "Password too short. Must be at least 8 characters.\n";
        return;
    }

    string userId = generateUniqueId();
    string joinDate = getJoinDate();
    if (joinDate.empty()) return;

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

// login a registered user
bool loginUser(string& userId, string& sessionToken) {
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
        string storedId, name, storedEmail, storedPassword, rest;
        getline(iss, storedId, ' ');
        getline(iss, name, ' ');
        getline(iss, storedEmail, ' ');
        getline(iss, storedPassword, ' ');

        if (email == storedEmail && password == storedPassword) {
            userId = storedId;
            sessionToken = generateSessionToken();
            cout << "Login successful! Welcome back.\n";
            return true;
        }
    }

    cout << "Login failed. Email or password incorrect.\n";
    return false;
}

// provide user services (only if session is active)
void accessServices(const string& userId, const string& sessionToken) {
    cout << "\n--- Accessing Services ---\n";
    cout << "Session Token: " << sessionToken << endl;
    cout << "User ID: " << userId << endl;

    
    cout << "You now have access to Skyline Cyber Cafe services.\n";
}

int main() {
    while (true) {
        cout << "\n====== Skyline Cyber Cafe ======\n";
        cout << "1. Register\n2. Login\n3. Exit\nChoose an option: ";
        string choice;
        getline(cin, choice);

        if (choice == "1") {
            registerUser();
        }
        else if (choice == "2") {
            string userId, sessionToken;
            if (loginUser(userId, sessionToken)) {
                accessServices(userId, sessionToken);
            }
        }
        else if (choice == "3") {
            cout << "Goodbye!\n";
            break;
        }
        else { // error validation 
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
