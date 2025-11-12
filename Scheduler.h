#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "Flight.h"
#include "PairingHeap.h"
#include "MinHeap.h"
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>

class Scheduler {
private:
    int currentTime;
    int nextRunwayID;
    
    // Data structures
    PairingHeap pendingFlights;
    MinHeap<Runway> runwayPool;
    std::unordered_map<int, Flight> activeFlights;
    MinHeap<TimetableEntry> timetable;
    std::unordered_map<int, std::unordered_set<int>> airlineIndex;
    
    // Helper methods
    // Process completed flights
    void settleCompletions(int time, std::vector<std::string>& output);
    // Transition scheduled flights to in-progress
    void promoteToInProgress(int time);
    // Reschedule unsatisfied flights
    void rescheduleUnsatisfied(std::vector<std::string>& output);
    // Advance scheduler time and process events
    void advanceTime(int time, std::vector<std::string>& output);
    // Get list of unsatisfied flights
    std::vector<int> getUnsatisfiedFlights();
    // Remove flight from all data structures
    void removeFlightFromStructures(int flightID);
    
public:
    Scheduler();
    
    // Initialize scheduler with runway count
    void initialize(int runwayCount, std::vector<std::string>& output);

    void submitFlight(int flightID, int airlineID, int submitTime, 
                     int priority, int duration, std::vector<std::string>& output);

    void cancelFlight(int flightID, int time, std::vector<std::string>& output);

    void reprioritize(int flightID, int time, int newPriority, 
                     std::vector<std::string>& output);

    void addRunways(int count, int time, std::vector<std::string>& output);
    // Ground flights for airline range
    void groundHold(int airlineLow, int airlineHigh, int time, 
                   std::vector<std::string>& output);
    // Print all active flights
    void printActive(std::vector<std::string>& output);
    // Print scheduled flights in time range
    void printSchedule(int t1, int t2, std::vector<std::string>& output);
    // Advance time and process events
    void tick(int time, std::vector<std::string>& output);
};

#endif // SCHEDULER_H