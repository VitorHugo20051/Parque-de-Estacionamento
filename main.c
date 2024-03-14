#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "functions.h"

int main() {
    Parking parks[MAX_PARKS];
    int num_parks = 0;
    char *name = (char*)malloc(MY_BUFSIZ * sizeof(char));
    int capacity;
    float X, Y, Z;
    char comand;

    while (1) {
        comand = getchar();

        switch (comand) {
            case 'q':
                quit_program();
            case 'p':
                if (scanf("%s %d %f %f %f", name, &capacity, &X, &Y, &Z) != 5) {
                    create_park(parks, &num_parks, NULL, 0, 0, 0, 0);
                }
                else {
                    create_park(parks, &num_parks, name, capacity, X, Y, Z);
                }
                break;
        }
    }
    free(name);
    return 0;
}
