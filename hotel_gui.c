#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_ROOMS 20
#define LOG_FILE "booking_log.txt"
#define ROOM_RATE 100.0

// UI Component IDs
#define ID_BTN_BOOK 1
#define ID_BTN_LOG 2
#define ID_BTN_AVAILABILITY 3

// UI Handles
HWND hName, hPhone, hCheckin, hCheckout, hDisplayBox;

// Application State
int rooms[MAX_ROOMS] = {0};

struct Customer {
    int room_number;
    char name[40];
    char phone[15];
    char checkin[11];
    char checkout[11];
    float bill;
};

// --- CORE HOTEL LOGIC ---

void parse_date(const char *date, int *day, int *month, int *year) {
    if (sscanf(date, "%d-%d-%d", day, month, year) != 3) {
        *day = 0; *month = 0; *year = 0;
    }
}

int calculate_days(const char *checkin, const char *checkout) {
    int day1, month1, year1;
    int day2, month2, year2;

    parse_date(checkin, &day1, &month1, &year1);
    parse_date(checkout, &day2, &month2, &year2);

    if (day1 == 0 || day2 == 0) return 1; // Fallback if format is invalid

    struct tm checkin_tm = {0, 0, 0, day1, month1 - 1, year1 - 1900};
    struct tm checkout_tm = {0, 0, 0, day2, month2 - 1, year2 - 1900};

    time_t checkin_time = mktime(&checkin_tm);
    time_t checkout_time = mktime(&checkout_tm);

    double seconds = difftime(checkout_time, checkin_time);
    int days = (int)(seconds / (86400));
    return days <= 0 ? 1 : days;
}

int assign_room() {
    for (int i = 0; i < MAX_ROOMS; i++) {
        if (rooms[i] == 0) {
            rooms[i] = 1;
            return i + 1;
        }
    }
    return -1;
}

void save_booking(struct Customer cust) {
    FILE *file = fopen(LOG_FILE, "a");
    if (file) {
        fprintf(file, "Room %d | Name: %s | Phone: %s | Check-in: %s | Check-out: %s | Bill: $%.2f\n",
                cust.room_number, cust.name, cust.phone, cust.checkin, cust.checkout, cust.bill);
        fclose(file);
    }
}

// --- WIN32 UI CALLBACK ---

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CREATE: {
            // Form Labels
            CreateWindow("STATIC", "Customer Name:", WS_VISIBLE | WS_CHILD, 20, 20, 120, 20, hwnd, NULL, NULL, NULL);
            CreateWindow("STATIC", "Phone Number:", WS_VISIBLE | WS_CHILD, 20, 60, 120, 20, hwnd, NULL, NULL, NULL);
            CreateWindow("STATIC", "Check-in (DD-MM-YYYY):", WS_VISIBLE | WS_CHILD, 20, 100, 160, 20, hwnd, NULL, NULL, NULL);
            CreateWindow("STATIC", "Check-out (DD-MM-YYYY):", WS_VISIBLE | WS_CHILD, 20, 140, 160, 20, hwnd, NULL, NULL, NULL);

            // Form Inputs
            hName = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER, 190, 20, 150, 20, hwnd, NULL, NULL, NULL);
            hPhone = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER, 190, 60, 150, 20, hwnd, NULL, NULL, NULL);
            hCheckin = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER, 190, 100, 150, 20, hwnd, NULL, NULL, NULL);
            hCheckout = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER, 190, 140, 150, 20, hwnd, NULL, NULL, NULL);

            // Buttons
            CreateWindow("BUTTON", "Book Room", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 20, 180, 320, 40, hwnd, (HMENU)ID_BTN_BOOK, NULL, NULL);
            CreateWindow("BUTTON", "View Booking Log", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 20, 240, 150, 30, hwnd, (HMENU)ID_BTN_LOG, NULL, NULL);
            CreateWindow("BUTTON", "Check Availability", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 190, 240, 150, 30, hwnd, (HMENU)ID_BTN_AVAILABILITY, NULL, NULL);

            // Output Display Area
            hDisplayBox = CreateWindow("EDIT", "Welcome to the Hotel Management System!", 
                WS_VISIBLE | WS_CHILD | WS_BORDER | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY, 
                380, 20, 450, 300, hwnd, NULL, NULL, NULL);
            break;
        }

        case WM_COMMAND: {
            if (LOWORD(wParam) == ID_BTN_BOOK) {
                struct Customer cust;
                GetWindowText(hName, cust.name, 40);
                GetWindowText(hPhone, cust.phone, 15);
                GetWindowText(hCheckin, cust.checkin, 11);
                GetWindowText(hCheckout, cust.checkout, 11);

                if (strlen(cust.name) == 0 || strlen(cust.phone) == 0) {
                    MessageBox(hwnd, "Please enter all details!", "Error", MB_ICONERROR);
                    break;
                }

                cust.room_number = assign_room();
                if (cust.room_number == -1) {
                    MessageBox(hwnd, "Sorry, no rooms are available.", "Full", MB_ICONWARNING);
                    break;
                }

                cust.bill = calculate_days(cust.checkin, cust.checkout) * ROOM_RATE;
                save_booking(cust);

                char success_msg[256];
                sprintf(success_msg, "Booking Successful!\r\nRoom Number: %d\r\nTotal Bill: $%.2f", cust.room_number, cust.bill);
                SetWindowText(hDisplayBox, success_msg);
                MessageBox(hwnd, "Room Booked Successfully!", "Success", MB_ICONINFORMATION);

                // Clear fields
                SetWindowText(hName, ""); SetWindowText(hPhone, ""); 
                SetWindowText(hCheckin, ""); SetWindowText(hCheckout, "");
            }
            else if (LOWORD(wParam) == ID_BTN_LOG) {
                FILE *file = fopen(LOG_FILE, "r");
                if (file) {
                    char buffer[8192] = "--- Booking Log ---\r\n\r\n";
                    char line[256];
                    while (fgets(line, sizeof(line), file)) {
                        strcat(buffer, line);
                        strcat(buffer, "\r"); // Win32 edits prefer \r\n
                    }
                    fclose(file);
                    SetWindowText(hDisplayBox, buffer);
                } else {
                    SetWindowText(hDisplayBox, "No booking log found.");
                }
            }
            else if (LOWORD(wParam) == ID_BTN_AVAILABILITY) {
                char buffer[2048] = "--- Room Availability ---\r\n\r\n";
                char line[64];
                for (int i = 0; i < MAX_ROOMS; i++) {
                    sprintf(line, "Room %d: %s\r\n", i + 1, rooms[i] == 0 ? "Available" : "Occupied");
                    strcat(buffer, line);
                }
                SetWindowText(hDisplayBox, buffer);
            }
            break;
        }

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
            
        case WM_CTLCOLORSTATIC: {
            HDC hdcStatic = (HDC)wParam;
            SetBkColor(hdcStatic, GetSysColor(COLOR_WINDOW));
            return (INT_PTR)GetSysColorBrush(COLOR_WINDOW);
        }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

// --- MAIN ENTRY POINT ---
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    const char CLASS_NAME[] = "HotelApp";

    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0, CLASS_NAME, "Hotel Management Native UI", WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX, 
        CW_USEDEFAULT, CW_USEDEFAULT, 870, 380, 
        NULL, NULL, hInstance, NULL
    );

    if (hwnd == NULL) return 0;

    ShowWindow(hwnd, nCmdShow);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
