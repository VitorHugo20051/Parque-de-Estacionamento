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

void free_parks(Parking *parks, int num_parks) {
    int i;
    for (i = 0; i < num_parks; i++) {
        free(parks[i].name);
    }
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
    
    parks[*num_parks].name = malloc(strlen(name) + 1);
    strcpy(parks[*num_parks].name, name);
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
    int entry_hour, entry_minute, exit_hour, exit_minute, total_hours;
    int entry_minutes, exit_minutes, total_minutes, full_days, periods;

    sscanf(record->entry_time, "%d:%d", &entry_hour, &entry_minute);
    sscanf(record->exit_time, "%d:%d", &exit_hour, &exit_minute);

    entry_minutes = entry_hour * 60 + entry_minute;
    exit_minutes = exit_hour * 60 + exit_minute;
    total_minutes = (atoi(record->exit_date) - atoi(record->entry_date)) * 24 * 60;
    total_minutes += exit_minutes - entry_minutes;
    full_days = total_minutes / (24 * 60);
    total_hours = total_minutes / 60;

    if ((atoi(record->entry_date) < 2 || (atoi(record->entry_date) == 2 && atoi(record->entry_date + 3) < 29)) &&
        (atoi(record->exit_date) > atoi(record->entry_date) || atoi(record->exit_date + 3) > 2 ||
         (atoi(record->exit_date + 3) == 2 && atoi(record->exit_date) > 29))) {
        bill = costZ;
    }

    periods = (total_minutes + 14) / 15;

    if (total_hours < 1) {
        bill += periods * costX;
    } else if (total_hours < 24) {
        bill += 4 * costX; 
        bill += (periods - 4) * costY;
        if (bill > costZ) {
            bill = costZ;
        }
    } else {
        bill = costZ;
        int full_days_minutes = full_days * 24 * 60;
        bill += full_days * costZ;
        int remaining_minutes = total_minutes - full_days_minutes;
        if (remaining_minutes < 0) {
            remaining_minutes = 0;
        }
        bill += calculate_bill(record, costX, costY, costZ);

    }
    return bill;
}

char *get_last_entry_time(const Parking *park) {
    const char *last_entry_time = NULL;

    if (park->num_records > 0) {
        for (int i = park->num_records - 1; i >= 0; i--) {
            if (park->records[i].entry_date[0] != '\0') {
                last_entry_time = park->records[i].entry_time;
                break;
            }
        }
    }

    return (char *)last_entry_time;
}

char *get_last_exit_time(const Parking *park) {
    const char *last_exit_time = NULL;

    if (park->num_records > 0) {
        for (int i = park->num_records - 1; i >= 0; i--) {
            if (park->records[i].exit_date[0] != '\0') {
                last_exit_time = park->records[i].exit_time;
                break;
            }
        }
    }

    return (char *)last_exit_time;
}

int is_exit_before_entry(const Parking *park, const char *entry_date, const char *entry_time, const char *exit_date, const char *exit_time) {
    int entry_hour, entry_minute, exit_hour, exit_minute;

    sscanf(entry_time, "%d:%d", &entry_hour, &entry_minute);
    sscanf(exit_time, "%d:%d", &exit_hour, &exit_minute);

    const char *last_entry_time = get_last_entry_time(park);
    const char *last_exit_time = get_last_exit_time(park);

    if (last_entry_time != NULL && last_exit_time == NULL) {
        int last_entry_hour = atoi(last_entry_time);
        int last_entry_minute = atoi(last_entry_time + 3); 

        if (last_entry_hour >= exit_hour && last_entry_minute >= exit_minute) {
            return 1;
        }

        else if (strcmp(exit_date, entry_date) < 0) {
            return 1;
        }
        else if (strcmp(exit_date, entry_date) == 0) {
            if (entry_hour == exit_hour) {
                return exit_minute < entry_minute;
            }
            else {
                return exit_hour < entry_hour;
            }
        }
    } else { 
        int last_exit_hour = atoi(last_exit_time);
        int last_exit_minute = atoi(last_exit_time + 3);

        if (last_exit_hour == exit_hour && last_exit_minute == exit_minute) {
            return 1;
        }

        else if (strcmp(exit_date, entry_date) < 0) {
            return 1;
        }
        else if (strcmp(exit_date, entry_date) == 0) {
            if (entry_hour == exit_hour) {
                return exit_minute < entry_minute;
            }
            else {
                return exit_hour < entry_hour;
            }
        }
    }
    return 0; 
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
    char *name, *park_name;
    float X, Y, Z;
    char comand, plate[MAX_PLATE], date[MAX_DATE_LENGTH], time[MAX_TIME_LENGTH];
    name = (char*)malloc(MY_BUFSIZ * sizeof(char));
    park_name = (char*)malloc(MY_BUFSIZ * sizeof(char));

    while (1) {
        comand = getchar();

        switch (comand) {
            case 'q':
                free_parks(parks, num_parks);
                free(name);
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
    return 0;
}
