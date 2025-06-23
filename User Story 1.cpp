#include <iostream>
#include <fstream>
#include <string>
#include <cctype>

using namespace std;

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
    cout << "🏢 Welcome to Skyline Cyber Cafe Registration!\n" << endl;
    registerUser();
    return 0;
}
