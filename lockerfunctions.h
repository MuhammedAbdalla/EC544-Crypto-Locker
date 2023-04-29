#include <stddef.h>
#ifndef LOCKERSERVER_H
    #define LOCKERSERVER_H

    #define PORT 8080

    #define FAILURE 0
    #define SUCCESS 1

    #define HOURS_IN_DAY 24
    #define MINS_IN_HOURS 60
    #define LOCKER_MIN_TIME_RESERVE 30

    // a user reservation
    struct user_reservation {
        char* name;
        char* start_time;
        char* end_time;

        int KEY; //store the user's RSA key generated by the API call from the webapp that is then transported
        int start_time_idx;
        int end_time_idx;

        int duration_hours;
        int bit_duration;


        struct user_reservation *next;
    };

    // linked list of all reservations attached to locker
    struct locker_reservations {
        struct user_reservation *head;
        struct user_reservation *tail;
        
        int LOCKED;
        char* USER;
    };


     /*
        LOGIC:
        if locker resverations are per hour: then -> 00000000000000000000000 is no reservations
        00011100000000000000000 is a contiguous locker session; sessions are indicated by contiguous 1

        - locker sessions must not overlap
        - locker sessions are bit shifted starting at index provide by the increments
            - if per hour, 0 = 12am, 1 = 1am... 6 = 6am.. 12 = 12pm.. 18 = 6pm.. 23 = 11pm; basically military time
        
        - example: start at 12pm, duration 4 hours
            - we get 4pm as end_time from front end calculations
        - we get to shift 0s 12 times, 1s 4 shifts, 0s rest
    */
    static struct locker_reservations *reservations;

    struct user_reservation set_bit_duration(int start_idx, int end_idx);

    // print all reservation details
    void print_reservations(struct locker_reservations*);
        
    // searches for reservations
    // returns null if none found
    struct user_reservation* search_reservation(char*);

    // create new user reservation if name doesn't exist
    // use name as client IP for uniqueness
    struct user_reservation* create_reservation(char**);

    // delete from LL if exists
    struct user_reservation* delete_reservation(char*);

    // delete from LL if exists
    // set the bits FIRST then do bitwise XOR, then if bits overlap, return overlap error
    struct user_reservation* modify_reservation(char*);


#endif 