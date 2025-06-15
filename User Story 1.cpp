// User Story 1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

// To check if email already exists in the users file
bool emailExists(const string& email) {
    ifstream file("users.txt");
    string line;
    while (getline(file, line)) {
        if (line.find(email) != string::npos) {
            return true; // Email already exists in file
        }
    }
    return false;
}

// To validate password length (min 8 characters)
bool validatePassword(const string& password) {
    return password.length() >= 8;
}

// To register a new user
void registerUser() {
    string name, email, password;

    // Get user input
    cout << "Enter your full name: ";
    getline(cin, name);

    cout << "Enter your email: ";
    getline(cin, email);

    // Check if email is unique
    if (emailExists(email)) {
        cout << "Email already registered. Please try again with a different email." << endl;
        return;
    }

    // Get password and validate
    cout << "Enter your password (at least 8 characters): ";
    getline(cin, password);

    if (!validatePassword(password)) {
        cout << "Password is too short. It must be at least 8 characters long." << endl;
        return;
    }

    // Store user data in the file
    ofstream file("users.txt", ios::app); // Open file in append mode
    if (file.is_open()) {
        file << name << " " << email << " " << password << endl;
        file.close();
        cout << "Registration successful!" << endl;
    }
    else {
        cout << "Error saving data. Please try again." << endl;
    }
}

int main() {
    cout << "Welcome to Skyline Cyber Cafe Registration!" << endl;
    registerUser();
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
