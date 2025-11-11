#ifndef RUNWAY_H
#define RUNWAY_H

#include <iostream>

/**
 * Runway structure for tracking runway availability
 * Used in the Runway Pool (MinHeap)
 * Sorted by: (nextFreeTime, runwayID)
 */
struct RunwayInfo {
    int runwayID;        
    int nextFreeTime;    
    
    // Constructor
    RunwayInfo(int id = 0, int freeTime = 0) 
        : runwayID(id), nextFreeTime(freeTime) {}
    
    // Comparison operator for min-heap
    // Primary: earlier nextFreeTime
    // Tie-breaker: smaller runwayID
    bool operator<(const RunwayInfo& other) const {
        if (nextFreeTime != other.nextFreeTime) {
            return nextFreeTime < other.nextFreeTime;
        }
        return runwayID < other.runwayID;
    }
    
    bool operator>(const RunwayInfo& other) const {
        if (nextFreeTime != other.nextFreeTime) {
            return nextFreeTime > other.nextFreeTime;
        }
        return runwayID > other.runwayID;
    }
    
    // friend std::ostream& operator<<(std::ostream& os, const RunwayInfo& r) {
    //     os << "[R" << r.runwayID << "@" << r.nextFreeTime << "]";
    //     return os;
    // }
};

/**
 * Timetable entry for tracking flight completions
 * Used in the Timetable (MinHeap)
 * Sorted by: (ETA, flightID)
 */
struct TimetableEntry {
    int ETA;            
    int flightID;        
    int runwayID;      
    
    // Constructor
    TimetableEntry(int eta = 0, int fid = 0, int rid = 0)
        : ETA(eta), flightID(fid), runwayID(rid) {}
    
    // Comparison operator for min-heap
    // Primary: earlier ETA
    // Tie-breaker: smaller flightID
    bool operator<(const TimetableEntry& other) const {
        if (ETA != other.ETA) {
            return ETA < other.ETA;
        }
        return flightID < other.flightID;
    }
    
    bool operator>(const TimetableEntry& other) const {
        if (ETA != other.ETA) {
            return ETA > other.ETA;
        }
        return flightID > other.flightID;
    } 
    
    // // For debugging
    // friend std::ostream& operator<<(std::ostream& os, const TimetableEntry& t) {
    //     os << "[F" << t.flightID << "@ETA" << t.ETA << "]";
    //     return os;
    // }
};

#endif 