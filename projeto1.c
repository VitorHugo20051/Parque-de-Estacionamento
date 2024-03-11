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

void create_park(char *name, int capacity, float X, float Y,  float Z) {
    Parking park;
    park.name = name;
    park.capacity = capacity;
    park.costX = X;
    park.costY = Y;
    park.costZ = Z;
}

int main() {
    char comand;
    scanf("%s", &comand);
    while (1) {
        scanf("%s", &comand);
        switch (comand) {
            case 'q':
                quit_program();
            case 'p':
                char *name = malloc(sizeof(BUFSIZ));
                int capacity;
                float X, Y, Z;
                scanf("%s %d %f %f %f", name, &capacity, &X, &Y, &Z);
                create_park(name, capacity, X, Y, Z);
        }
    }
    return 0;
}
