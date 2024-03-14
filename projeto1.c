#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "functions.h"

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
                char *name = (char*)malloc(BUFSIZ * sizeof(char));
                int capacity;
                float X, Y, Z;
                if (scanf("%s %d %f %f %f", name, &capacity, &X, &Y, &Z) != 1){
                    create_park(parks, &num_parks, NULL, 0, 0, 0, 0);
                }
                else {
                    create_park(parks, &num_parks, name, capacity, X, Y, Z);
                }
                break;
        }
        scanf("%s", &comand);
    }
    return 0;
}
