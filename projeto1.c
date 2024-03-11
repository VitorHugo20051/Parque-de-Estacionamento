#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_PARKS 20
#define MAX_PLATE_NAME 10
#define MAX_DATE_LENGTH 11
#define MAX_TIME_LENGTH 6
#define BUFSIZ 8192

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
    if (*num_parks > MAX_PARKS) {
        printf("too many parks.\n");
        return;
    }

    for (int i = 0; i < *num_parks; i++) {
        if (strcomp(parks[i].name, name)) {
            printf("parking already exists.\n");
            return;
        }
    }
    
    if (capacity < 0) {
        printf("invalid capacity.\n");
        return;
    }

    if (X < 0 || Y < 0 || Z < 0 || !(X > Y && Y > Z)) {
        printf("invalid cost.\n");
        return;
    }

    parks[*num_parks].name = name;
    parks[*num_parks].capacity = capacity;
    parks[*num_parks].costX = X;
    parks[*num_parks].costY = Y;
    parks[*num_parks].costZ = Z;
    parks[*num_parks].num_records = 0;

    (*num_parks)++;

    for (int i = 0; i < *num_parks; i++) {
        printf("%s %d %f %f %f\n", parks[i].name, parks[i].capacity, parks[i].costX, parks[i].costY, parks[i].costZ);
    }
}

int main() {
    Parking parks[MAX_PARKS];
    int num_parks = 0;
    char comand;

    scanf("%s", &comand);
    while (1) {
        switch (comand) {
            case 'q':
                quit_program();
            case 'p':
                char *name = malloc(BUFSIZ);
                int capacity;
                float X, Y, Z;
                scanf("%s %d %f %f %f", name, &capacity, &X, &Y, &Z);
                create_park(name, capacity, X, Y, Z);
                break;
        }
        scanf("%s", &comand);
    }
    return 0;
}
