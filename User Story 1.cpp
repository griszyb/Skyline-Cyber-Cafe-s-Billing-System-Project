#include <iostream>
#include <fstream>
#include <string>
#include <cctype>
#include <vector>

using namespace std;

// List of pre-authorized admin emails
vector<string> adminEmails = {
    "admin@skylinecafe.com",
    "superuser@skylinecafe.com"
};

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

bool validatePassword(const string& password) {
    if (password.length() < 8) return false;

    bool hasUpper = false, hasLower = false, hasDigit = false, hasSpecial = false;

    for (char ch : password) {
        if (isupper(ch)) hasUpper = true;
        else if (islower(ch)) hasLower = true;
        else if (isdigit(ch)) hasDigit = true;
        else hasSpecial = true;
    }

    return hasUpper && hasLower && hasDigit && hasSpecial;
}

bool isAdminEmail(const string& email) {
    for (const string& admin : adminEmails) {
        if (email == admin) return true;
    }
    return false;
}

bool verifyCredentials(const string& email, const string& password) {
    ifstream file("users.txt");
    string line;
    while (getline(file, line)) {
        if (line.find(email) != string::npos && line.find(password) != string::npos) {
            return true;
        }
    }
    return false;
}

void adminLogin() {
    string email, password;

    cout << "🔐 Admin Login\n";
    cout << "Enter admin email: ";
    getline(cin, email);

    if (!isAdminEmail(email)) {
        cout << "❌ Access denied: Not an authorized admin email.\n";
        return;
    }

    cout << "Enter password: ";
    getline(cin, password);

    if (verifyCredentials(email, password)) {
        cout << "✅ Admin login successful. Access granted.\n";
        // Admin-specific functionality can go here
    } else {
        cout << "❌ Incorrect password.\n";
    }
}

void registerUser() {
    string name, email, password;

    cout << "Enter your full name: ";
    getline(cin, name);

    cout << "Enter your email: ";
    getline(cin, email);

    if (emailExists(email)) {
        cout << "Email already registered. Please try again with a different email." << endl;
        return;
    }

    cout << "Enter your password (min 8 characters, include uppercase, lowercase, digit, and special character): ";
    getline(cin, password);

    if (!validatePassword(password)) {
        cout << "\n❌ Password does not meet the security requirements.\n";
        cout << "Your password must contain:\n";
        cout << "- At least 8 characters\n";
        cout << "- At least one uppercase letter (A-Z)\n";
        cout << "- At least one lowercase letter (a-z)\n";
        cout << "- At least one number (0-9)\n";
        cout << "- At least one special character (e.g., !@#$%^&*)\n";
        return;
    }

    ofstream file("users.txt", ios::app);
    if (file.is_open()) {
        file << name << " " << email << " " << password << endl;
        file.close();
        cout << "✅ Registration successful!" << endl;
    } else {
        cout << "⚠️ Error saving data. Please try again." << endl;
    }
}

int main() {
    int choice;
    cout << "🏢 Welcome to Skyline Cyber Cafe System\n\n";
    cout << "1. Register User\n";
    cout << "2. Admin Login\n";
    cout << "Choose an option (1 or 2): ";
    cin >> choice;
    cin.ignore(); // Clear newline character from buffer

    if (choice == 1) {
        registerUser();
    } else if (choice == 2) {
        adminLogin();
    } else {
        cout << "❌ Invalid option selected.\n";
    }

    return 0;
}
