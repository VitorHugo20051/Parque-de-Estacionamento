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
    else {
        for (int i = 0; i < *num_parks; i++) {
            int occupied_spots = parks[i].num_records;
            int available_spots = parks[i].capacity - occupied_spots;
            printf("%s %d %d", parks[i].name, parks[i].capacity, available_spots);
        }
    }
}