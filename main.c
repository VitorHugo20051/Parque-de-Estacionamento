#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "functions.h"

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