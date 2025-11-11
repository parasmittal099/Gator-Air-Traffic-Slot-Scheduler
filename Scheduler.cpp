#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
#include "Flight.h"
#include "PairingHeap.h"
#include "MinHeap.h"
#include "Runway.h"

class Scheduler {
private:
    // Current system time
    int currentTime;
    
    // Next available runway ID for adding new runways
    int nextRunwayID;
    
    // Data structures
    PairingHeap pendingFlights;                                    // Max heap for pending flights
    MinHeap<RunwayInfo> runwayPool;                               // Min heap for runway availability
    MinHeap<TimetableEntry> timetable;                            // Min heap for completions
    std::unordered_map<int, Flight*> activeFlights;               // flightID -> Flight*
    std::unordered_map<int, std::vector<int>> airlineIndex;       // airlineID -> list of flightIDs
    std::unordered_map<int, PairingNode<FlightKey>*> handles;     // flightID -> pairing heap node
    
    // Output stream
    std::ostream& out;
    
    // Helper methods
    void settleCompletions(int time);
    void promoteFlights(int time);
    void rescheduleUnsatisfiedFlights();
    void rebuildRunwayPool();
    std::vector<Flight*> collectUnsatisfiedFlights();
    void advanceTimeTo(int time);
    
public:
    Scheduler(std::ostream& output);
    ~Scheduler();
    
    // Operations
    void Initialize(int runwayCount);
    void SubmitFlight(int flightID, int airlineID, int submitTime, int priority, int duration);
    void CancelFlight(int flightID, int time);
    void Reprioritize(int flightID, int time, int newPriority);
    void AddRunways(int count, int time);
    void GroundHold(int airlineLow, int airlineHigh, int time);
    void PrintActive();
    void PrintSchedule(int t1, int t2);
    void Tick(int t);
    void Quit();
};

#endif // SCHEDULER_H