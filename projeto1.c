#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_PARKS 20
#define MAX_PLATE_NAME 10
#define MAX_DATE_LENGTH 11
#define MAX_TIME_LENGTH 6
#define MY_BUFSIZ 8192

typedef struct {
    char plate[MAX_PLATE_NAME];
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
    if (*num_parks >= MAX_PARKS) {
        printf("too many parks.\n");
        return;
    }

    if (name != NULL) {
        for (int i = 0; i < *num_parks; i++) {
            if (strcmp(parks[i].name, name) == 0) {
                printf("%s: parking already exists.\n", name);
                return;
            }
        }
    }

    if (capacity < 0) {
        printf("%d: invalid capacity.\n", capacity);
        return;
    }

    if (X < 0 || Y < 0 || Z < 0 || (X > Y && Y > Z)) {
        printf("invalid cost.\n");
        return;
    }

    if (name != NULL && capacity != 0 && X != 0 && Y != 0 && Z != 0) {
        parks[*num_parks].name = strdup(name);
        parks[*num_parks].capacity = capacity;
        parks[*num_parks].available_spots = capacity;
        parks[*num_parks].costX = X;
        parks[*num_parks].costY = Y;
        parks[*num_parks].costZ = Z;
        parks[*num_parks].num_records = 0;

        (*num_parks)++;
    }
    else {
        for (int i = 0; i < *num_parks; i++) {
            int occupied_spots = parks[i].num_records;
            int available_spots = parks[i].capacity - occupied_spots;
            printf("%s %d %d\n", parks[i].name, parks[i].capacity, available_spots);
        }
    }
}

int is_valid_date(const char *date) {
    int day, month, year;
    if (sscanf(date, "-%d-%d-%d", &day, &month, &year) != 3) {
        return 0;
    }

    if (day < 1 || day > 31 || month < 1 || month > 12 || year < 1900 || year > 2100) {
        return 0;
    }
    return 1;
}

int is_valid_time(const *time) {
    int hour, minute;
    if (sscanf(time, "%d:%d", %hour, %minute) != 2) {
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

    if (!isupper(plate[0]) || !isupper(plate[1]) || plate[2] != '-' ||
        !isdigit(plate[3]) || !isdigit(plate[4]) || plate[5] != '-' ||
        !isupper(plate[6]) || !isupper(plate[7])) {
            return 0;
    }
    return 1;
}

void veichle_entry(Parking *parks, int num_parks, const char *park_name, const char *plate, const char *date, const char *time) {
    int park_index = -1, i;
    
    for (i = 0; i < num_parks; i++) {
        if (strcmp(parks[i].name, park_name) == 0) {
            park_index = i;
            break;
        }
    }

    if (park_index == -1) {
        printf("%s: no such parking.", park_name);
        return;
    }

    if (parks[park_index].available_spots == 0) {
        printf("%s: parking is full.", park_name);
        return;
    }

    if (!is_valid_plate(plate)) {
        printf("%s: invalid license plate.", plate);
        return;
    }

    for (i = 0; i < parks[park_index].num_records; i++) {
        if (strcmp(parks[park_index].records[i], plate) == 0) {
            printf("%s: invalid veichle entry.", plate);
            break;
        }
    }

    if (!is_valid_time(time) || !is_valid_date(date)) {
        printf("invalid date.");
        return;
    }

    strcpy(parks[park_index].records[parks[park_index].num_records].plate, plate);
    strcpy(parks[park_index].records[parks[park_index].num_records].entry_date, date);
    strcpy(parks[park_index].records[parks[park_index].num_records].entry_time, time);
    parks[park_index].num_records++;
    parks[park_index].available_spots--;

    printf("%s %d", park_name, parks[park_index].available_spots);
}

int main() {
    Parking parks[MAX_PARKS];
    int num_parks = 0;
    char *name = (char*)malloc(MY_BUFSIZ * sizeof(char));
    int capacity, arguments;
    float X, Y, Z;
    char comand;

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
                    create_park(parks, &num_parks, NULL, 0, 0, 0, 0);
                }
                break;
            case 'e':
                scanf("%s %s %s %s", park_name, plate, date, time);
                veichle_entry(parks, num_parks, park_name, plate, date, time);
                break;
        }
    }
    free(name);
    return 0;
}
