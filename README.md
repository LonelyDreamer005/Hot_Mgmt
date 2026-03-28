# Hotel Management System

A Hotel Management System built in C that efficiently manages room bookings, customer records, and provides a clear log of transactions stored in a text file for persistence. It includes both a **Command-Line Interface (CLI)** and a **Native Windows GUI**.

## Features

- **Room Booking System:**
  - Collects customer details (Name, Phone Number, Check-in/Check-out dates).
  - Assigns a unique room number automatically.
  - Calculates bills automatically based on length of stay.
  - Stores booking details in a log file for future reference.

- **Multiple Interfaces:**
  - **CLI Mode:** Fast and easy-to-use text-based menu system.
  - **GUI Mode:** Native Windows Desktop application with windowed forms and buttons.

- **Persistent Log System:**
  - Ensures booking records are stored securely in a `booking_log.txt` file.
  - Data remains available even after closing the program.

## How to Run This Project

You must have a C compiler like **GCC** (e.g., MinGW) installed on your system.

### Option 1: Run the Command-Line Version (CLI)
1. Open your terminal/command prompt and navigate to the project directory.
2. Compile the CLI source code:
   ```bash
   gcc hotle.c -o hotle
   ```
3. Run the compiled executable:
   ```bash
   .\hotle.exe
   ```

### Option 2: Run the Native Windows GUI Version
1. Open your terminal/command prompt and navigate to the project directory.
2. Compile the GUI source code with the `-mwindows` flag to prevent a black console window from opening in the background:
   ```bash
   gcc hotel_gui.c -o hotel_gui.exe -mwindows
   ```
3. Run the compiled executable:
   ```bash
   .\hotel_gui.exe
   ```

This is an academic project.
