#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_PARKS 20
#define MAX_PLATE 9
#define MAX_DATE_LENGTH 11
#define MAX_TIME_LENGTH 6
#define MY_BUFSIZ 8192

typedef struct VeichleRecord {
    char plate[MAX_PLATE];
    char entry_date[MAX_DATE_LENGTH];
    char entry_time[MAX_TIME_LENGTH];
    char exit_date[MAX_DATE_LENGTH];
    char exit_time[MAX_TIME_LENGTH];
    float bill;
} VeichleRecord;

typedef struct {
    char *name;
    int capacity;
    int available_spots;
    float costX;
    float costY;
    float costZ;
    struct VeichleRecord *records;
    int num_records;
} Parking;

void quit_program() {
    exit(0);
}

void free_parks(Parking *parks, int num_parks) {
    int i;
    for (i = 0; i < num_parks; i++) {
        free(parks[i].name);
        free(parks[i].records);
    }
}

void create_park(Parking *parks, int *num_parks, char *name, int capacity, float X, float Y,  float Z) {
    int len = strlen(name);
    if (name != NULL) {
        for (int i = 0; i < *num_parks; i++) {
            if (strcmp(parks[i].name, name) == 0) {
                printf("%s: parking already exists.\n", name);
                return;
            }
        }
    }

    if (capacity <= 0) {
        printf("%d: invalid capacity.\n", capacity);
        return;
    }

    if (X < 0 || Y < 0 || Z < 0 || X > Y || Y > Z || X > Z) {
        printf("invalid cost.\n");
        return;
    }

    if (*num_parks >= MAX_PARKS) {
        printf("too many parks.\n");
        return;
    }
    
    parks[*num_parks].name = malloc(len + 1);
    strcpy(parks[*num_parks].name, name);
    parks[*num_parks].capacity = capacity;
    parks[*num_parks].available_spots = capacity;
    parks[*num_parks].costX = X;
    parks[*num_parks].costY = Y;
    parks[*num_parks].costZ = Z;
    parks[*num_parks].num_records = 0;
    parks[*num_parks].records = malloc(capacity * sizeof(VeichleRecord));

    (*num_parks)++;

}

void list_parks(const Parking *parks, int num_parks) {
    for (int i = 0; i < num_parks; i++) {
        int occupied_spots = parks[i].num_records;
        int available_spots = parks[i].capacity - occupied_spots;
        printf("%s %d %d\n", parks[i].name, parks[i].capacity, available_spots);
    }
}

int is_valid_date(const char *date) {
    int day, month, year;
    if (sscanf(date, "%d-%d-%d", &day, &month, &year) != 3) {
        return 0;
    }

    if (day < 1 || day > 31 || month < 1 || month > 12 || year < 1900 || year > 2100) {
        return 0;
    }

    switch (month) {
        case 1: case 3: case 5: case 7: case 8: case 10: case 12:
            if (day > 31) {
                return 0;
            }
            break;
        case 4: case 6: case 9: case 11:
            if (day > 30) {
                return 0;
            }
            break;
        case 2:
            if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
                if (day > 29) {
                    return 0;
                }
            } else {
                if (day > 28) {
                    return 0;
                }
            }
            break;
    }
    return 1;
}

int is_valid_time(const char *time) {
    int hour, minute;
    if (sscanf(time, "%d:%d", &hour, &minute) != 2) {
        return 0;
    }

    if (hour < 0 || hour > 23 || minute < 0 || minute > 59) {
        return 0;
    }
    return 1;
}

int is_valid_plate(const char *plate) {
    int len = strlen(plate), digit = 0, upper = 0, i;
    char last = '\0';

    if (len != 8) {
        return 0;
    }

    for (i = 0; i < len; i++) {
        if (isalpha(plate[i])) {
            upper++;
            if (!isupper(plate[i]))
                return 0;
            if (isdigit(last))
                return 0;
        } else if (isdigit(plate[i])) {
            digit++;
            if (isalpha(last))
                return 0;
        } else if (plate[i] != '-') {
            return 0;
        }
        last = plate[i];
    }

    if (upper < 2 || digit < 2)
        return 0;

    return 1;
}

void veichle_entry(Parking *parks, int *num_parks, char *park_name, char *plate, char *date, char *time) {
    int park_index = -1, i, j;

    for (i = 0; i < *num_parks; i++) {
        if (strcmp(parks[i].name, park_name) == 0) {
            park_index = i;
            break;
        }
    }

    if (park_index == -1) {
        printf("%s: no such parking.\n", park_name);
        return;
    }

    if (parks[park_index].available_spots == 0) {
        printf("%s: parking is full.\n", park_name);
        return;
    }

    if (!is_valid_plate(plate)) {
        printf("%s: invalid licence plate.\n", plate);
        return;
    }

    for (i = 0; i < *num_parks; i++) {
        if (i != park_index) {
            for (j = 0; j < parks[i].num_records; j++) {
                if (strcmp(parks[i].records[j].plate, plate) == 0 && parks[i].records[j].exit_date[0] == '\0') {
                    printf("%s: invalid vehicle entry.\n", plate);
                    return;
                    break;
                }
            }
        } 
    }

    if (!is_valid_time(time) || !is_valid_date(date)) {
        printf("invalid date.\n");
        return;
    }

    if (parks[park_index].num_records > 0) {
        VeichleRecord *last_record = &parks[park_index].records[parks[park_index].num_records - 1];
        if (strcmp(last_record->entry_date, date) == 0) {
            if (strcmp(last_record->entry_time, time) > 0) {
                printf("invalid date.\n");
                return;
            }
        }
    }

    parks[park_index].available_spots--;
    printf("%s %d\n", park_name, parks[park_index].available_spots);

    VeichleRecord new_entry;

    strcpy(new_entry.plate, plate);
    strcpy(new_entry.entry_date, date);
    strcpy(new_entry.entry_time, time);
    new_entry.exit_date[0] = '\0';
    new_entry.exit_time[0] = '\0';
    new_entry.bill = 0.0;

    parks[park_index].records[parks[park_index].num_records] = new_entry;
    parks[park_index].num_records++;    
}

int days_in_month(int month, int year) {
    switch (month) {
        case 1: case 3: case 5: case 7: case 8: case 10: case 12:
            return 31;
            break;
        case 4: case 6: case 9: case 11:
            return 30;
            break;
        case 2:
            if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
                return 29;
            } else {
                return 28;
            }
            break;
    }
    return -1;
}

int days_since_year_start(int month, int day, int year) {
    int days = day;
    for (int m = 1; m < month; m++) {
        days += days_in_month(m, year);
    }
    days += day;
    return days;
}

int minutes_since_reference_date(const char *date, const char *time) {
    int day, month, year, hours, minutes, total_minutes, y = 0, m = 0;

    sscanf(date, "%d-%d-%d", &day, &month, &year);
    sscanf(time, "%d:%d", &hours, &minutes);

    total_minutes = 0;

    for (y = 1900; y < year; y++) {
        total_minutes += 365 * 24 * 60;
        if ((y % 4 == 0 && y % 100 != 0) || (y % 400 == 0)) {
            total_minutes += 24 * 60;
        }
    }
    for (m = 1; m < month; m++) {
        total_minutes += days_in_month(m, year) * 24 * 60;
    }

    total_minutes += (day - 1) * 24 * 60;
    total_minutes += hours * 60;
    total_minutes += minutes;

    return total_minutes;
}

int is_29_day(char *date) {
    int day, month, year;
    sscanf(date, "%d-%d-%d", &day, &month, &year);

    if (month == 2 && day == 29) {
        if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
            return 1; 
        }
    }
    return 0;
}

char *get_last_entry_time(const Parking *park) {
    char *last_entry_time = NULL;

    if (park->num_records > 0) {
        for (int i = park->num_records - 1; i >= 0; i--) {
            if (park->records[i].entry_date[0] != '\0') {
                last_entry_time = park->records[i].entry_time;
                break;
            }
        }
    }

    return last_entry_time;
}

char *get_last_exit_time(const Parking *park) {
    char *last_exit_time = NULL;

    if (park->num_records > 0) {
        for (int i = park->num_records - 1; i >= 0; i--) {
            if (park->records[i].exit_date[0] != '\0') {
                last_exit_time = park->records[i].exit_time;
                break;
            }
        }
    }

    return last_exit_time;
}

char *get_last_entry_date(const Parking *park) {
    char *last_entry_date = NULL;

    if (park->num_records > 0) {
        for (int i = park->num_records - 1; i >= 0; i--) {
            if (park->records[i].entry_date[0] != '\0') {
                last_entry_date = park->records[i].entry_date;
                break;
            }
        }
    }

    return last_entry_date;
}

char *get_last_exit_date(const Parking *park) {
    char *last_exit_date = NULL;

    if (park->num_records > 0) {
        for (int i = park->num_records - 1; i >= 0; i--) {
            if (park->records[i].entry_date[0] != '\0') {
                last_exit_date = park->records[i].exit_date;
                break;
            }
        }
    }

    return last_exit_date;
}

int is_exit_before_entry(const Parking *park, const char *entry_date, const char *entry_time, const char *exit_date, const char *exit_time) {
    int entry_hour, entry_minute, exit_hour, exit_minute;
    int entry_day, entry_month, entry_year, exit_day, exit_month, exit_year;
    int last_entry_hour, last_exit_hour, last_entry_minute, last_exit_minute;
    int last_entry_day, last_entry_month, last_entry_year, last_exit_day, last_exit_month, last_exit_year;

    sscanf(entry_date, "%d-%d-%d", &entry_day, &entry_month, &entry_year);
    sscanf(exit_date, "%d-%d-%d", &exit_day, &exit_month, &exit_year);
    sscanf(entry_time, "%d:%d", &entry_hour, &entry_minute);
    sscanf(exit_time, "%d:%d", &exit_hour, &exit_minute);

    char *last_entry_time = get_last_entry_time(park);
    char *last_exit_time = get_last_exit_time(park);
    char *last_entry_date = get_last_entry_date(park);
    char *last_exit_date = get_last_exit_date(park);

    if (exit_year < entry_year || (exit_year == entry_year && exit_month < entry_month) ||
        (exit_year == entry_year && exit_month == entry_month && exit_day < entry_day)) {
        return 1;
    } else if (exit_year == entry_year && exit_month == entry_month && exit_day == entry_day) {
        if (entry_hour == exit_hour) {
            return exit_minute < entry_minute;
        }
        else {
            return exit_hour < entry_hour;
        }
    }

    // Compare exit with last entry if available, or last exit if available
    if (last_entry_time != NULL && last_exit_time == NULL) {
        sscanf(last_entry_time, "%d:%d", &last_entry_hour, &last_entry_minute);
        sscanf(last_entry_date, "%d-%d-%d", &last_entry_day, &last_entry_month, &last_entry_year);

        if (last_entry_year > exit_year || (last_entry_year == exit_year && last_entry_month > exit_month) ||
            (last_entry_year == exit_year && last_entry_month == exit_month && last_entry_day > exit_day)) {
                return 1;
        } else if (last_entry_year == exit_year && last_entry_month == exit_month && last_entry_day == exit_day) {
            if (last_entry_hour >= exit_hour && last_entry_minute >= exit_minute) {
                return 1;
            }
        }
    } else if (last_exit_time != NULL && last_entry_time == NULL) {
        sscanf(last_exit_time, "%d:%d", &last_exit_hour, &last_exit_minute);
        sscanf(last_exit_date, "%d-%d-%d", &last_exit_day, &last_exit_month, &last_exit_year);

        if (last_exit_year > exit_year || (last_exit_year == exit_year && last_exit_month > exit_month) ||
            (last_exit_year == exit_year && last_exit_month == exit_month && last_exit_day > exit_day)) {
                return 1;
        } else if (last_exit_year == exit_year && last_exit_month == exit_month && last_exit_day == exit_day) {
            if (last_exit_hour >= exit_hour && last_exit_minute >= exit_minute) {
                return 1;
            }
        }
    
        return 0;
    }
}

float calculate_bill(Parking *park, int total_minutes, VeichleRecord *record, float costX, float costY, float costZ) {
    float bill = 0.0;
    int periods = (total_minutes + 14) / 15, full_days, remaining_minutes;

    if (is_29_day(record->entry_date) || is_29_day(record->exit_date)) {
        if (is_exit_before_entry(park, record->entry_date, record->entry_time, record->exit_date, record->exit_time)) {
            bill -= costZ;
        }
    }

    if (total_minutes <= 60) {
        bill = periods * costX;
    } 
    else if (total_minutes < (24 * 60)) {
        bill += 4 * costX;
        bill += (periods - 4) * costY;
        if (bill > costZ) {
            bill = costZ;
        }
    } 
    else if (total_minutes > (24 * 60)){
        full_days = total_minutes / (24 * 60);
        bill += full_days * costZ;
        remaining_minutes = total_minutes - (full_days * 24 * 60);
        if (remaining_minutes < 0) {
            remaining_minutes = 0;
        }
        bill += calculate_bill(park, remaining_minutes, record, costX, costY, costZ);
    }
    return bill;    
}

void add_leading_zero(char *time) {
    int len = strlen(time);
    if (time[0] == '0' || (time[0] <= '9' && time[1] == ':')) {
        memmove(time + 1, time, len + 1);
        time[0] = '0';
    }
}

void veichle_exit(Parking *parks, int *num_parks, char *park_name, char *plate, char *date, char *time) {
    int park_index = -1, veichle_index = -1, i = 0, total_entry_minutes, total_exit_minutes, total_minutes, j = 0;

    for (i = 0; i < *num_parks; i++) {
        if (strcmp(parks[i].name, park_name) == 0) {
            park_index = i;
            break;
        }
    }

    if (park_index == -1) {
        printf("%s: no such parking.\n", park_name);
        return;
    }

    if (!is_valid_plate(plate)) {
        printf("%s: invalid license plate.\n", plate);
        return;
    }

    for (j = 0; j < parks[park_index].num_records; j++) {
        if (strcmp(parks[park_index].records[j].plate, plate) == 0 && parks[park_index].records[j].exit_date[0] == '\0') {
            veichle_index = j;
            break;
        }
    }

    if (veichle_index == -1) {
        printf("%s: invalid vehicle exit.\n", plate);
        return;
    }


    if (!is_valid_date(date) || !is_valid_time(time)) {
        printf("invalid date.\n");
        return;
    }

    if (is_exit_before_entry(parks, parks[park_index].records[veichle_index].entry_date, parks[park_index].records[veichle_index].entry_time, date, time)) {
        printf("invalid date.\n");
        return;
    }

    total_entry_minutes = minutes_since_reference_date(parks[park_index].records[veichle_index].entry_date, parks[park_index].records[veichle_index].entry_time);
    total_exit_minutes = minutes_since_reference_date(date, time);
    total_minutes = total_exit_minutes - total_entry_minutes;

    float bill = calculate_bill(parks, total_minutes, &parks[park_index].records[veichle_index], parks[park_index].costX, parks[park_index].costY, parks[park_index].costZ);
    parks[park_index].records[veichle_index].bill = bill;

    if (isdigit(parks[park_index].records[veichle_index].entry_time[0]) && parks[park_index].records[veichle_index].entry_time[1] == ':') {
        add_leading_zero(parks[park_index].records[veichle_index].entry_time);
    }
    if (isdigit(time[0]) && time[1] == ':') {
        add_leading_zero(time);
    }

    strcpy(parks[park_index].records[veichle_index].exit_date, date);
    strcpy(parks[park_index].records[veichle_index].exit_time, time);

    parks[park_index].num_records--;
    parks[park_index].available_spots++;
    
    printf("%s %s %s %s %s %.2f\n", plate, parks[park_index].records[veichle_index].entry_date, parks[park_index].records[veichle_index].entry_time,
    date, time, bill);

}

void print_car_records(Parking *parks, int *num_parks, const char *plate) {
    int found = 0, i = 0, j = 0;

    if(!(is_valid_plate(plate))) {
        printf("%s: invalid plate.\n", plate);
    }

     for (i = 0; i < *num_parks; i++) {
        for (j = 0; j < parks[i].num_records; j++) {
            if (strcmp(parks[i].records[j].plate, plate) == 0) {
                found = 1;
                if (parks[i].records[j].exit_date[0] != '\0') {
                    printf("%s %s %s %s %s\n", parks[i].records[j].plate,
                           parks[i].records[j].entry_date,
                           parks[i].records[j].entry_time,
                           parks[i].records[j].exit_date,
                           parks[i].records[j].exit_time);
                } else {
                    printf("%s %s\n", parks[i].records[j].plate,
                           parks[i].records[j].entry_date);
                }
            }
        }
    }
    if (!found) {
        printf("%s: no entries found in any parking.\n", plate);
    }
}

void show_billing(const Parking *parks, int *num_parks, char *park_name, char *date) {
    int park_index = -1, i, found = 0;
    int has_date = (date[0] != '\0');

    for (i = 0; i < *num_parks; i++) {
        if (strcmp(parks[i].name, park_name) == 0) {
            park_index = i;
            break;
        }
    }

    if (park_index == -1) {
        printf("%s: no such parking.\n", park_name);
        return;
    }

    if (has_date && !is_valid_date(date)) {
        printf("invalid date.\n");
        return;
    }

    if (!has_date) {
        for (i = 0; i < parks[park_index].num_records; i++) {
            VeichleRecord *record = &parks[park_index].records[i];
            printf("%s %.2f\n", record->exit_date, record->bill);
        }
    } else {
        found = 0;
        for (i = 0; i < parks[park_index].num_records; i++) {
            VeichleRecord *record = &parks[park_index].records[i];
            if (strcmp(record->exit_date, date) == 0) {
                printf("%s %s %.2f\n", record->plate, record->exit_time, record->bill);
                found = 1;
            }
        }
        if (!found) {
            printf("No billing details found for %s on %s.\n", park_name, date);
        }
    }
}

void remove_park(Parking *parks, int *num_parks, char *park_name) {
    int park_index = -1, i = 0;
    for (i = 0; i < *num_parks; i++) {
        if (strcmp(parks[i].name, park_name) == 0) {
            park_index = i;
            break;
        }
    }
    if (park_index == -1) {
        printf("%s: no such parking.\n", park_name);
        return;
    }
    free(parks[park_index].name);
    free(parks[park_index].records);
    for (int i = park_index; i < *num_parks - 1; i++) {
        parks[i] = parks[i + 1];
    }
    (*num_parks)--;
}


int main() {
    Parking parks[MAX_PARKS];
    int num_parks = 0, capacity, arguments;
    char *name, *park_name;
    float X, Y, Z;
    char comand, plate[MAX_PLATE], date[MAX_DATE_LENGTH], time[MAX_TIME_LENGTH];
    name = (char*)malloc(MY_BUFSIZ * sizeof(char));
    park_name = (char*)malloc(MY_BUFSIZ * sizeof(char));

    while (1) {
        scanf("%c", &comand);

        switch (comand) {
            case 'q':
                free_parks(parks, num_parks);
                free(name);
                free(park_name);
                quit_program();
                break;
            case 'p':
                if (getchar() == ' ') {
                    arguments = scanf(" \"%[^\"]\" %d %f %f %f", name, &capacity, &X, &Y, &Z);
                    if (arguments == 5) {
                        create_park(parks, &num_parks, name, capacity, X, Y, Z);
                    }
                    else {
                        scanf("%s %d %f %f %f", name, &capacity, &X, &Y, &Z);
                        create_park(parks, &num_parks, name, capacity, X, Y, Z);
                    }
                }
                else {
                    list_parks(parks, num_parks);
                }
                break;
            case 'e':
                arguments = scanf(" \"%[^\"]\" %8s %s %5s", park_name, plate, date, time);
                if (arguments == 4) {
                    veichle_entry(parks, &num_parks, park_name, plate, date, time);
                } else {
                    scanf("%s %8s %s %5s", park_name, plate, date, time);
                    veichle_entry(parks, &num_parks, park_name, plate, date, time);
                }
                break;
            case 's':
                arguments = scanf(" \"%[^\"]\" %8s %s %5s", park_name, plate, date, time);
                if (arguments == 4) {
                    veichle_exit(parks, &num_parks, park_name, plate, date, time);
                } else {
                    scanf(" %s %8s %s %5s", park_name, plate, date, time);
                    veichle_exit(parks, &num_parks, park_name, plate, date, time);
                }
                break;
            case 'v':
                scanf(" %s", plate);
                show_billing(parks, &num_parks, park_name, date);
                break;
            case 'f':
                arguments = scanf(" \"%[^\"]\"", park_name);
                if (arguments == 1) {
                    show_billing(parks, &num_parks, park_name, "");
                } else {
                    scanf(" %s", date);
                    show_billing(parks, &num_parks, park_name, date);
                }
                break;
            case 'r':
                arguments = scanf(" \"%[^\"]\"", park_name);
                if (arguments == 1) {
                    remove_park(parks, &num_parks, park_name);
                } else {
                    scanf("%s", park_name);
                    remove_park(parks, &num_parks, park_name);
                }
                break;
        }
    }
    return 0;
}
