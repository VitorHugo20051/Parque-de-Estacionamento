#ifndef _FUNCTIONS_
#define _FUNCTIONS_

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

void quit_program();
void free_parks(Parking *parks, int num_parks);
void create_park(Parking *parks, int *num_parks, char *name, int capacity, float X, float Y,  float Z);
void list_parks(const Parking *parks, int num_parks);
int is_valid_date(const char *date);
int is_valid_time(const char *time);
int is_valid_plate(const char *plate);
void veichle_entry(Parking *parks, int *num_parks, char *park_name, char *plate, char *date, char *time);
float calculate_bill(VeichleRecord *record, float costX, float costY, float costZ);
char *get_last_entry_time(const Parking *park);
char *get_last_exit_time(const Parking *park);
int is_exit_before_entry(const Parking *park, const char *entry_date, const char *entry_time, const char *exit_date, const char *exit_time);
void add_leading_zero(char *time);
void veichle_exit(Parking *parks, int *num_parks, char *park_name, char *plate, char *date, char *time);


#endif