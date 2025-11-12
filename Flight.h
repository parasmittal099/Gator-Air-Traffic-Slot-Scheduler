#ifndef FLIGHT_H
#define FLIGHT_H

#include <string>

// Flight state throughout its lifecycle
enum FlightState {
    PENDING,        // Pending assignment
    SCHEDULED,      // Scheduled on runway
    IN_PROGRESS,    // In progress
    COMPLETED       // Completed
};

// Flight with attributes and metadata
struct Flight {
    int flightID;
    int airlineID;
    int submitTime;
    int priority;
    int duration;
    int runwayID;
    int startTime;
    int ETA;
    FlightState state;
    
    // Default constructor
    Flight() : flightID(-1), airlineID(-1), submitTime(-1), priority(-1), 
               duration(-1), runwayID(-1), startTime(-1), ETA(-1), 
               state(PENDING) {}
    
    // Constructor with request details
    Flight(int fid, int aid, int st, int p, int d) 
        : flightID(fid), airlineID(aid), submitTime(st), priority(p), 
          duration(d), runwayID(-1), startTime(-1), ETA(-1), 
          state(PENDING) {}
};

#endif // FLIGHT_H