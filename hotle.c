#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_ROOMS 20
#define LOG_FILE "booking_log.txt"
#define ROOM_RATE 100.0  // Rate per day per room

// Customer Structure
struct Customer {
    int room_number;
    char name[40];
    char phone[15];
    char checkin[11];
    char checkout[11];
    float bill;
};

// Room Availability Array
int rooms[MAX_ROOMS] = {0};

// Utility Functions
void save_booking(struct Customer cust) {
    FILE *file = fopen(LOG_FILE, "a"); // Append mode to preserve past logs
    if (!file) {
        perror("Error opening log file");
        return;
    }
    fprintf(file, "Room %d | Name: %s | Phone: %s | Check-in: %s | Check-out: %s | Bill: $%.2f\n",
            cust.room_number, cust.name, cust.phone, cust.checkin, cust.checkout, cust.bill);
    fclose(file);
}

void display_log() {
    FILE *file = fopen(LOG_FILE, "r");
    if (!file) {
        printf("No booking log found.\n");
        return;
    }

    char buffer[200];
    printf("\nBooking Log:\n");
    printf("------------------------------------------------------------\n");
    while (fgets(buffer, sizeof(buffer), file)) {  // Fixed syntax here
        printf("%s", buffer);
    }
    fclose(file);
    printf("------------------------------------------------------------\n");
}

int assign_room() {
    for (int i = 0; i < MAX_ROOMS; i++) {
        if (rooms[i] == 0) {
            rooms[i] = 1;  // Mark room as occupied
            return i + 1;  // Room number starts from 1
        }
    }
    return -1;  // No room available
}

void check_room_availability() {
    printf("\nRoom Availability:\n");
    printf("------------------\n");
    for (int i = 0; i < MAX_ROOMS; i++) {
        printf("Room %d: %s\n", i + 1, rooms[i] == 0 ? "Available" : "Occupied");
    }
}

// Function to parse date string into day, month, and year
void parse_date(const char *date, int *day, int *month, int *year) {
    sscanf(date, "%d-%d-%d", day, month, year);
}

// Function to calculate the difference in days between two dates
int calculate_days(const char *checkin, const char *checkout) {
    int day1, month1, year1;
    int day2, month2, year2;

    // Parse check-in and check-out dates
    parse_date(checkin, &day1, &month1, &year1);
    parse_date(checkout, &day2, &month2, &year2);

    // Convert dates to time_t (seconds since epoch)
    struct tm checkin_tm = {0, 0, 0, day1, month1 - 1, year1 - 1900};
    struct tm checkout_tm = {0, 0, 0, day2, month2 - 1, year2 - 1900};

    time_t checkin_time = mktime(&checkin_tm);
    time_t checkout_time = mktime(&checkout_tm);

    // Calculate difference in seconds and convert to days
    double seconds = difftime(checkout_time, checkin_time);
    return (int)(seconds / (60 * 60 * 24));
}

void calculate_bill(struct Customer *cust) {
    int days = calculate_days(cust->checkin, cust->checkout);
    cust->bill = days * ROOM_RATE;
}

void book_room() {
    struct Customer cust;
    cust.room_number = assign_room();

    if (cust.room_number == -1) {
        printf("Sorry, no rooms are available.\n");
        return;
    }

    printf("Enter Customer Name: ");
    scanf(" %39[^\n]", cust.name);

    printf("Enter Phone Number: ");
    scanf(" %14s", cust.phone);

    printf("Enter Check-in Date (DD-MM-YYYY): ");
    scanf(" %10s", cust.checkin);

    printf("Enter Check-out Date (DD-MM-YYYY): ");
    scanf(" %10s", cust.checkout);

    calculate_bill(&cust);
    save_booking(cust);
    printf("\nBooking Successful! Room Number: %d\n", cust.room_number);
    printf("Total Bill: $%.2f\n", cust.bill);
}

void view_customer_details() {
    int room_number;
    printf("Enter Room Number: ");
    scanf("%d", &room_number);

    FILE *file = fopen(LOG_FILE, "r");
    if (!file) {
        printf("No booking log found.\n");
        return;
    }

    char buffer[200];
    int found = 0;
    while (fgets(buffer, sizeof(buffer), file)) {  // Fixed syntax here
        int logged_room_number;
        sscanf(buffer, "Room %d", &logged_room_number);
        if (logged_room_number == room_number) {
            printf("\nCustomer Details:\n");
            printf("-----------------\n");
            printf("%s", buffer);
            found = 1;
            break;
        }
    }
    fclose(file);

    if (!found) {
        printf("No customer found in Room %d.\n", room_number);
    }
}

void menu() {
    int choice;

    do {
        printf("\n================ HOTEL MANAGEMENT SYSTEM ================\n");
        printf("1. Book a Room\n");
        printf("2. View Booking Log\n");
        printf("3. Check Room Availability\n");
        printf("4. View Customer Details\n");
        printf("5. Exit\n");
        printf("==========================================================\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                book_room();
                break;
            case 2:
                display_log();
                break;
            case 3:
                check_room_availability();
                break;
            case 4:
                view_customer_details();
                break;
            case 5:
                printf("\nThank you for using the Hotel Management System!\n");
                return;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 5);
}

int main() {
    menu();
    return 0;
}