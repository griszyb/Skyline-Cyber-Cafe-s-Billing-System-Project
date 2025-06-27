# Skyline Cyber Cafe Billing System - Project
Scenario 1:
Project: Skyline Cyber Café’s billing system
Silver Star Enterprises Ltd. plans to establish an internet café named "Skyline Cyber Café" in the heart of Auckland CBD, targeting 
tourists and local university students. The café will offer a range of services including internet browsing, gaming, printing and 
scanning. They have secured a 100-square meter space in the CBD and have already purchased all necessary infrastructure 
including computers, printers, scanners, modems, switches, etc. The estimated establishment cost for the cyber cafe is NZD 
350,000. This project entails the development of an automated billing system to track usage duration and printing/scanning activity 
for each customer session, and to calculate the total amount payable at the end of the session. 
To use the cafes services users will need to register with their name, email and password. Passwords must follow the 'best practice' 
rules. Once the user is registered the software will create a unique id for each user and store that id for later use along with their 
joining date. Once the user has successfully logged in the software will track user duration. Users will create many possible
sessions so your software will need to track all sessions for that user. Logged in users will be allowed to print and scan but will be 
presented with a bill for these services at the end of each session. 
The bill amount will be tracked for each user. The costs of printing and scanning will differ.
Allow Admin users to log in based upon a list of stored admin emails. 
Admins can view, edit or delete any user. Admins can also see a list of all usernames, the total time of all user durations and the 
total value of all prints and scans.

# Skyline Cyber Café Billing System

Skyline Cyber Café Billing System is a C++ console-based application built for **Silver Star Enterprises Ltd.**, designed to manage user sessions, print and scan operations, and provide automated billing for an internet café in Auckland CBD. It supports both regular users and admins with tailored functionality.

---

# Features

# User Functionality
- **Registration** with secure password validation
- **Login** with session time tracking
- **Unique User ID** and join date on registration
- Start a **session** and:
  - Perform **print jobs** (charged per page)
  - Perform **scan jobs** (charged per item)
  - End session to see **detailed billing summary**
- View **cumulative billing total** across sessions

# Admin Functionality
- **Admin registration** restricted to pre-approved emails
- Admin login with access to:
  - View **all registered users**
  - View **individual user reports**:
    - Total session time (charged per minute)
    - Total printed pages and scanned items
    - Total billing
  - **Edit or delete users** and all associated data
  - View **system-wide usage summary**

---

# Billing Rates

| Service      | Rate           |
|--------------|----------------|
| Session Time | $0.20 per min  |
| Printing     | $0.50 per page |
| Scanning     | $0.20 per item |

---

# Files Created & Used

- `users.txt` – stores registered users' details
- `admins.txt` – list of authorized admin emails
- `billing_totals.txt` – stores total billed amount for each user
- `session_logs.txt` – logs session duration and charges
- `print_logs.txt` – logs all print jobs
- `scan_logs.txt` – logs all scan jobs

---

