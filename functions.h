#ifndef _FUNCTIONS_
#define _FUNCTIONS_

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

void quit_program();
void create_park(Parking *parks, int *num_parks, char *name, int capacity, float X, float Y,  float Z);
void list_parks(const Parking *parks, int num_parks);
int is_valid_date(const char *date);
int is_valid_time(const char *time);
int is_valid_plate(const char *plate);
void veichle_entry(Parking *parks, int *num_parks, char *park_name, char *plate, char *date, char *time);


#endif