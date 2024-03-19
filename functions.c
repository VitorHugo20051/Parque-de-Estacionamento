#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "functions.h"

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

    if (!isupper(plate[0]) || !isupper(plate[1]) || plate[2] != '-' ||
        !isdigit(plate[3]) || !isdigit(plate[4]) || plate[5] != '-' ||
        !isupper(plate[6]) || !isupper(plate[7])) {
            return 0;
    }
    return 1;
}

void veichle_entry(Parking *parks, int *num_parks, char *park_name, char *plate, char *date, char *time) {
    int park_index = -1, i;
    
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
        printf("%s: invalid license plate.\n", plate);
        return;
    }

    for (i = 0; i < parks[park_index].num_records; i++) {
        if (strcmp(parks[park_index].records[i].plate, plate) == 0) {
            printf("%s: invalid veichle entry.\n", plate);
            break;
        }
    }

    if (!is_valid_time(time) || !is_valid_date(date)) {
        printf("invalid date.\n");
        return;
    }

    strcpy(parks[park_index].records[parks[park_index].num_records].plate, plate);
    strcpy(parks[park_index].records[parks[park_index].num_records].entry_date, date);
    strcpy(parks[park_index].records[parks[park_index].num_records].entry_time, time);
    parks[park_index].num_records++;
    parks[park_index].available_spots--;

    printf("%s %d\n", park_name, parks[park_index].available_spots);
}