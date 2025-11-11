#ifndef FLIGHT_H
#define FLIGHT_H

#include <string>

enum FlightState {
    PENDING,         
    SCHEDULED,
    IN_PROGRESS,       
    COMPLETED
};

class Flight {
public:
    int flightID;
    int airlineID;
    int submitTime;
    int priority;
    int duration;
    int runwayID;      // Fixed: was runwayId
    int startTime;
    int ETA;

    FlightState state; // Fixed: was flightstatus

    // Default constructor
    Flight() {
        this->flightID = -1;
        this->airlineID = -1;
        this->submitTime = 0;
        this->priority = 0;
        this->duration = 0;
        this->runwayID = -1;
        this->startTime = -1;
        this->ETA = -1;
        this->state = PENDING;
    }

    Flight(int flightID, int airlineID, int submitTime, int priority, int duration) {
        this->flightID = flightID;
        this->airlineID = airlineID;
        this->submitTime = submitTime;
        this->priority = priority;
        this->duration = duration;
        this->runwayID = -1;
        this->startTime = -1;
        this->ETA = -1;
        this->state = PENDING;
    }

    bool isUnsatisfied(int currentTime) const {
        return state == PENDING || (state == SCHEDULED && startTime > currentTime);
    }

    bool hasStarted(int currentTime) const {
        return state == IN_PROGRESS || state == COMPLETED || 
            (state == SCHEDULED && startTime <= currentTime);
    }

    bool hasCompleted(int currentTime) const {
        return state == COMPLETED || ETA <= currentTime;
    }

    void updateState(int currentTime) {
        if (state == COMPLETED) {
            return;
        }
        if (ETA != -1 && currentTime >= ETA) {
            state = COMPLETED;
        } else if (startTime != -1 && currentTime >= startTime) {
            state = IN_PROGRESS;
        } else if (startTime != -1) {
            state = SCHEDULED;
        } else {
            state = PENDING;
        }
    }

    void assignSchedule(int rID, int sTime, int eta) {
        runwayID = rID;
        startTime = sTime;
        ETA = eta;
        state = SCHEDULED;
    }

    // ADDED: Clear schedule method
    void clearSchedule() {
        runwayID = -1;
        startTime = -1;
        ETA = -1;
        if (state == SCHEDULED) {
            state = PENDING;
        }
    }

    // ADDED: Comparison operators for priority
    bool operator<(const Flight& other) const {
        if (priority != other.priority) {
            return priority < other.priority;
        }
        if (submitTime != other.submitTime) {
            return submitTime > other.submitTime;
        }
        return flightID > other.flightID;
    }

    bool operator>(const Flight& other) const {
        if (priority != other.priority) {
            return priority > other.priority;
        }
        if (submitTime != other.submitTime) {
            return submitTime < other.submitTime;
        }
        return flightID < other.flightID;
    }
};

#endif