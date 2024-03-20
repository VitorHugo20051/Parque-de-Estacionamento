#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_PARKS 20
#define MAX_PLATE 9
#define MAX_DATE_LENGTH 11
#define MAX_TIME_LENGTH 6
#define MY_BUFSIZ 8192

typedef struct {
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
    VeichleRecord records[MAX_PARKS];
    int num_records;
} Parking;

void quit_program() {
    exit(0);
}

void create_park(Parking *parks, int *num_parks, char *name, int capacity, float X, float Y,  float Z) {
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

    parks[*num_parks].name = strdup(name);
    parks[*num_parks].capacity = capacity;
    parks[*num_parks].available_spots = capacity;
    parks[*num_parks].costX = X;
    parks[*num_parks].costY = Y;
    parks[*num_parks].costZ = Z;
    parks[*num_parks].num_records = 0;

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
    int len = strlen(plate);

    if (len != 8) {
        return 0;
    }

    if ((!isupper(plate[0]) || !isupper(plate[1]) || !isdigit(plate[3]) || 
        !isdigit(plate[4]) || !isupper(plate[6]) || !isupper(plate[7])) && 
        (!isdigit(plate[0]) || !isdigit(plate[1]) || !isupper(plate[3]) || 
        !isupper(plate[4]) || !isdigit(plate[6]) || !isdigit(plate[7]))) {
        return 0;
    }

    return 1;
}

void veichle_entry(Parking *parks, int *num_parks, char *park_name, char *plate, char *date, char *time) {
    int park_index = -1, i, j;
    
    for (i = 0; i < *num_parks; i++) {
        if (strcasecmp(parks[i].name, park_name) == 0) {
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

    for (j = 0; j < *num_parks; j++) {
        if (j == park_index) {
            continue;
        }
        for (i = 0; i < parks[j].num_records; i++) {
            if (strcmp(parks[j].records[i].plate, plate) == 0) {
                printf("%s: invalid vehicle entry.\n", plate);
                return;
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

    strcpy(parks[park_index].records[parks[park_index].num_records].plate, plate);
    strcpy(parks[park_index].records[parks[park_index].num_records].entry_date, date);
    strcpy(parks[park_index].records[parks[park_index].num_records].entry_time, time);
    parks[park_index].num_records++;
    parks[park_index].available_spots--;

    printf("%s %d\n", park_name, parks[park_index].available_spots);
}

float calculate_bill(VeichleRecord *record, float costX, float costY, float costZ) {
    float bill = 0.0;
    int entry_hour, entry_minute, exit_hour, exit_minute;

    sscanf(record->entry_time, "%d:%d", &entry_hour, &entry_minute);
    sscanf(record->exit_time, "%d:%d", &exit_hour, &exit_minute);

    int total_minutes = (atoi(record->exit_date) - atoi(record->entry_date)) * 24 * 60;
    total_minutes += (exit_hour - entry_hour) * 60 + (exit_minute - entry_minute);

    int full_days = total_minutes / (24 * 60);
    bill += full_days * costZ;

    int remaining_minutes = total_minutes % (24 * 60);
    if (remaining_minutes > 0) {
        int first_hour_minutes = 60 - entry_minute;
        if (remaining_minutes <= 60) {
            bill += costX;
        } else {
            bill += costX;
            remaining_minutes -= first_hour_minutes;
            int additional_hours = (remaining_minutes + 14) / 15; // Arredondar para cima
            bill += additional_hours * costY;
        }
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
    int park_index = -1, veichle_index = -1, i;

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

    for (i = 0; i < parks[park_index].num_records; i++) {
        if (strcmp(parks[park_index].records[i].plate, plate) == 0) {
            veichle_index = i;
            break;
        }
    }

    if (veichle_index == -1) {
        printf("%s: invalid veichle exit.\n", plate);
        return;
    }

    if (!is_valid_date(date) || !is_valid_time(time)) {
        printf("invalid date\n");
        return;
    }

    if (parks[park_index].num_records > 0) {
        VeichleRecord *last_record = &parks[park_index].records[parks[park_index].num_records - 1];
        if ((strcmp(last_record->exit_date, date) == 0 && strcmp(last_record->exit_time, time) > 0) ||
            strcmp(last_record->exit_date, date) > 0) {
            printf("invalid date.\n");
            return;
        }
    }

    strcpy(parks[park_index].records[veichle_index].exit_date, date);
    strcpy(parks[park_index].records[veichle_index].exit_time, time);

    float bill = calculate_bill(&parks[park_index].records[veichle_index], parks[park_index].costX, parks[park_index].costY, parks[park_index].costZ);
    parks[park_index].records[veichle_index].bill = bill;

    add_leading_zero(parks[park_index].records[veichle_index].entry_time);
    add_leading_zero(time);

    printf("%s %s %s %s %s %.2f\n", plate, parks[park_index].records[veichle_index].entry_date, parks[park_index].records[veichle_index].entry_time,
    date, time, bill);

    parks[park_index].num_records--;
    parks[park_index].available_spots++;
}

int main() {
    Parking parks[MAX_PARKS];
    int num_parks = 0, capacity, arguments;
    char *name = (char*)malloc(MY_BUFSIZ * sizeof(char)), *park_name = (char*)malloc(MY_BUFSIZ * sizeof(char));
    float X, Y, Z;
    char comand, plate[MAX_PLATE], date[MAX_DATE_LENGTH], time[MAX_TIME_LENGTH];

    while (1) {
        comand = getchar();

        switch (comand) {
            case 'q':
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
                if (getchar() == ' ') {
                    arguments = scanf(" \"%[^\"]\" %s %s %s", park_name, plate, date, time);
                    if (arguments == 4) {
                        scanf(" \"%[^\"]\" %s %s %s", park_name, plate, date, time);
                        veichle_entry(parks, &num_parks, park_name, plate, date, time);
                    } else {
                        scanf("%s %s %s %s", park_name, plate, date, time);
                        veichle_entry(parks, &num_parks, park_name, plate, date, time);
                    }
                }
                break;
            case 's':
                if (getchar() == ' ') {
                    arguments = scanf(" \"%[^\"]\" %s %s %s", park_name, plate, date, time);
                    if (arguments == 4) {
                        scanf(" \"%[^\"]\" %s %s %s", park_name, plate, date, time);
                        veichle_exit(parks, &num_parks, park_name, plate, date, time);
                    } else {
                        scanf("%s %s %s %s", park_name, plate, date, time);
                        veichle_exit(parks, &num_parks, park_name, plate, date, time);
                    }
                }
                break;
        }
    }
    free(name);
    return 0;
}
