#include "Scheduler.h"
#include <algorithm>
#include <sstream>

Scheduler::Scheduler() : currentTime(0), nextRunwayID(1) {}

// Process flights that have completed their scheduling
void Scheduler::settleCompletions(int time, std::vector<std::string>& output) {
    std::vector<TimetableEntry> completed;
    
    while (!timetable.empty() && timetable.top().ETA <= time) {
        completed.push_back(timetable.top());
        timetable.pop();
    }
    
    std::sort(completed.begin(), completed.end());
    
    for (const auto& entry : completed) {
        output.push_back("Flight " + std::to_string(entry.flightID) + 
                        " has landed at time " + std::to_string(entry.ETA));
        
        auto it = activeFlights.find(entry.flightID);
        if (it != activeFlights.end()) {
            int airlineID = it->second.airlineID;
            activeFlights.erase(it);
            
            auto ait = airlineIndex.find(airlineID);
            if (ait != airlineIndex.end()) {
                ait->second.erase(entry.flightID);
                if (ait->second.empty()) {
                    airlineIndex.erase(ait);
                }
            }
        }
    }
}

// Transition scheduled flights to in-progress when their start time arrives
void Scheduler::promoteToInProgress(int time) {
    for (auto& pair : activeFlights) {
        Flight& flight = pair.second;
        if (flight.state == SCHEDULED && flight.startTime <= time) {
            flight.state = IN_PROGRESS;
        }
    }
}

// Identify flights that haven't been assigned or need rescheduling
std::vector<int> Scheduler::getUnsatisfiedFlights() {
    std::vector<int> unsatisfied;
    
    for (auto& pair : activeFlights) {
        Flight& flight = pair.second;
        if (flight.state == PENDING || 
            (flight.state == SCHEDULED && flight.startTime > currentTime)) {
            unsatisfied.push_back(flight.flightID);
        }
    }
    
    return unsatisfied;
}

// Reassign unsatisfied flights to available runways using greedy scheduling
void Scheduler::rescheduleUnsatisfied(std::vector<std::string>& output) {
    std::vector<int> unsatisfiedIDs = getUnsatisfiedFlights();
    if (unsatisfiedIDs.empty()) return;
    
    std::unordered_map<int, int> oldETAs;
    for (int fid : unsatisfiedIDs) {
        auto it = activeFlights.find(fid);
        if (it != activeFlights.end() && it->second.ETA != -1) {
            oldETAs[fid] = it->second.ETA;
        }
    }
    
    // Clear old assignments
    for (int fid : unsatisfiedIDs) {
        auto it = activeFlights.find(fid);
        if (it != activeFlights.end()) {
            Flight& flight = it->second;
            flight.state = PENDING;
            flight.runwayID = -1;
            flight.startTime = -1;
            flight.ETA = -1;
        }
    }
    
    // Build pending heap with all unsatisfied flights
    pendingFlights.clear();
    
    for (int fid : unsatisfiedIDs) {
        auto it = activeFlights.find(fid);
        if (it != activeFlights.end()) {
            pendingFlights.push(&it->second);
        }
    }
    
    // Rebuild runway pool
    MinHeap<Runway> newRunwayPool;
    for (int i = 1; i < nextRunwayID; i++) {
        int nextFree = currentTime;
        
        for (auto& pair : activeFlights) {
            Flight& flight = pair.second;
            if (flight.state == IN_PROGRESS && flight.runwayID == i) {
                nextFree = std::max(nextFree, flight.ETA);
            }
        }
        
        newRunwayPool.push(Runway(i, nextFree));
    }
    runwayPool = newRunwayPool;
    
    // Clear timetable for unsatisfied flights
    MinHeap<TimetableEntry> newTimetable;
    std::unordered_set<int> unsatisfiedSet(unsatisfiedIDs.begin(), unsatisfiedIDs.end());
    
    std::vector<TimetableEntry> allEntries;
    while (!timetable.empty()) {
        allEntries.push_back(timetable.top());
        timetable.pop();
    }
    
    for (const auto& entry : allEntries) {
        if (unsatisfiedSet.find(entry.flightID) == unsatisfiedSet.end()) {
            newTimetable.push(entry);
        }
    }
    timetable = newTimetable;
    
    // Schedule flights greedily
    while (!pendingFlights.empty()) {
        Flight* flight = pendingFlights.top();
        pendingFlights.pop();
        
        if (runwayPool.empty()) break;
        
        Runway runway = runwayPool.top();
        runwayPool.pop();
        
        int startTime = std::max(currentTime, runway.nextFreeTime);
        int ETA = startTime + flight->duration;
        
        flight->runwayID = runway.runwayID;
        flight->startTime = startTime;
        flight->ETA = ETA;
        flight->state = SCHEDULED;
        
        runway.nextFreeTime = ETA;
        runwayPool.push(runway);
        
        timetable.push(TimetableEntry(ETA, flight->flightID, runway.runwayID));
    }
    
    // Check for ETA changes
    std::vector<std::pair<int, int>> changedETAs;
    for (int fid : unsatisfiedIDs) {
        auto it = activeFlights.find(fid);
        if (it != activeFlights.end() && it->second.ETA != -1) {
            auto oldIt = oldETAs.find(fid);
            if (oldIt != oldETAs.end()) {  // Only if there WAS an old ETA
                int oldETA = oldIt->second;
                if (oldETA != it->second.ETA) {
                    changedETAs.push_back({fid, it->second.ETA});
                }
            }
        }
    }
    
    if (!changedETAs.empty()) {
        std::sort(changedETAs.begin(), changedETAs.end());
        
        std::ostringstream oss;
        oss << "Updated ETAs: [";
        for (size_t i = 0; i < changedETAs.size(); i++) {
            if (i > 0) oss << ", ";
            oss << changedETAs[i].first << ": " << changedETAs[i].second;
        }
        oss << "]";
        output.push_back(oss.str());
    }
}

// Update scheduler state and process all events at given time
void Scheduler::advanceTime(int time, std::vector<std::string>& output) {
    if (time == currentTime) {
        promoteToInProgress(time);
        rescheduleUnsatisfied(output);
        return;
    }
    
    currentTime = time;
    settleCompletions(time, output);
    promoteToInProgress(time);
    rescheduleUnsatisfied(output);
}

// Remove flight from all data structures
void Scheduler::removeFlightFromStructures(int flightID) {
    auto it = activeFlights.find(flightID);
    if (it == activeFlights.end()) return;
    
    Flight& flight = it->second;
    
    // Remove from airline index
    auto ait = airlineIndex.find(flight.airlineID);
    if (ait != airlineIndex.end()) {
        ait->second.erase(flightID);
        if (ait->second.empty()) {
            airlineIndex.erase(ait);
        }
    }
    
    // Remove from timetable
    std::vector<TimetableEntry> remaining;
    while (!timetable.empty()) {
        TimetableEntry entry = timetable.top();
        timetable.pop();
        if (entry.flightID != flightID) {
            remaining.push_back(entry);
        }
    }
    for (const auto& entry : remaining) {
        timetable.push(entry);
    }
    
    activeFlights.erase(it);
}

// Initialize scheduler with available runways
void Scheduler::initialize(int runwayCount, std::vector<std::string>& output) {
    if (runwayCount <= 0) {
        output.push_back("Invalid input. Please provide a valid number of runways.");
        return;
    }
    
    currentTime = 0;
    nextRunwayID = 1;
    
    for (int i = 0; i < runwayCount; i++) {
        runwayPool.push(Runway(nextRunwayID++, 0));
    }
    
    output.push_back(std::to_string(runwayCount) + " Runways are now available");
}

// Add new flight to scheduler and assign runway
void Scheduler::submitFlight(int flightID, int airlineID, int submitTime, 
                            int priority, int duration, std::vector<std::string>& output) {
    advanceTime(submitTime, output);
    
    if (activeFlights.find(flightID) != activeFlights.end()) {
        output.push_back("Duplicate FlightID");
        return;
    }
    
    Flight flight(flightID, airlineID, submitTime, priority, duration);
    activeFlights[flightID] = flight;
    
    airlineIndex[airlineID].insert(flightID);
    
    // Store size before to check if we need Updated ETAs output
    size_t outputSizeBefore = output.size();
    rescheduleUnsatisfied(output);
    
    // Print the new flight's ETA first
    auto it = activeFlights.find(flightID);
    if (it != activeFlights.end()) {
        // If reschedule printed Updated ETAs, we need to reorder output
        if (output.size() > outputSizeBefore && 
            output.back().find("Updated ETAs:") == 0) {
            std::string updatedETAs = output.back();
            output.pop_back();
            output.push_back("Flight " + std::to_string(flightID) + 
                            " scheduled - ETA: " + std::to_string(it->second.ETA));
            output.push_back(updatedETAs);
        } else {
            output.push_back("Flight " + std::to_string(flightID) + 
                            " scheduled - ETA: " + std::to_string(it->second.ETA));
        }
    }
}

// Remove flight from schedule if not already in progress
void Scheduler::cancelFlight(int flightID, int time, std::vector<std::string>& output) {
    advanceTime(time, output);
    
    auto it = activeFlights.find(flightID);
    if (it == activeFlights.end()) {
        output.push_back("Flight " + std::to_string(flightID) + " does not exist");
        return;
    }
    
    Flight& flight = it->second;
    
    if (flight.state == IN_PROGRESS || flight.state == COMPLETED) {
        output.push_back("Cannot cancel. Flight " + std::to_string(flightID) + 
                        " has already departed");
        return;
    }
    
    removeFlightFromStructures(flightID);
    
    output.push_back("Flight " + std::to_string(flightID) + " has been canceled");
    
    rescheduleUnsatisfied(output);
}

// Update flight priority and reschedule if pending
void Scheduler::reprioritize(int flightID, int time, int newPriority, 
                            std::vector<std::string>& output) {
    advanceTime(time, output);
    
    auto it = activeFlights.find(flightID);
    if (it == activeFlights.end()) {
        output.push_back("Flight " + std::to_string(flightID) + " not found");
        return;
    }
    
    Flight& flight = it->second;
    
    if (flight.state == IN_PROGRESS || flight.state == COMPLETED) {
        output.push_back("Cannot reprioritize. Flight " + std::to_string(flightID) + 
                        " has already departed");
        return;
    }
    
    flight.priority = newPriority;
    
    output.push_back("Priority of Flight " + std::to_string(flightID) + 
                    " has been updated to " + std::to_string(newPriority));
    
    rescheduleUnsatisfied(output);
}

// Add runways and reschedule affected flights
void Scheduler::addRunways(int count, int time, std::vector<std::string>& output) {
    advanceTime(time, output);
    
    if (count <= 0) {
        output.push_back("Invalid input. Please provide a valid number of runways.");
        return;
    }
    
    for (int i = 0; i < count; i++) {
        runwayPool.push(Runway(nextRunwayID++, currentTime));
    }
    
    output.push_back("Additional " + std::to_string(count) + " Runways are now available");
    
    rescheduleUnsatisfied(output);
}

// Cancel all pending flights for airlines in given range
void Scheduler::groundHold(int airlineLow, int airlineHigh, int time, 
                          std::vector<std::string>& output) {
    advanceTime(time, output);
    
    if (airlineHigh < airlineLow) {
        output.push_back("Invalid input. Please provide a valid airline range.");
        return;
    }
    
    std::vector<int> toRemove;
    
    for (int airlineID = airlineLow; airlineID <= airlineHigh; airlineID++) {
        auto it = airlineIndex.find(airlineID);
        if (it != airlineIndex.end()) {
            for (int flightID : it->second) {
                auto fit = activeFlights.find(flightID);
                if (fit != activeFlights.end()) {
                    Flight& flight = fit->second;
                    if (flight.state == PENDING || 
                        (flight.state == SCHEDULED && flight.startTime > currentTime)) {
                        toRemove.push_back(flightID);
                    }
                }
            }
        }
    }
    
    for (int flightID : toRemove) {
        removeFlightFromStructures(flightID);
    }
    
    output.push_back("Flights of the airlines in the range [" + 
                    std::to_string(airlineLow) + ", " + 
                    std::to_string(airlineHigh) + "] have been grounded");
    
    rescheduleUnsatisfied(output);
}

// Display all active flights sorted by ID
void Scheduler::printActive(std::vector<std::string>& output) {
    if (activeFlights.empty()) {
        output.push_back("No active flights");
        return;
    }
    
    std::vector<Flight> flights;
    for (auto& pair : activeFlights) {
        flights.push_back(pair.second);
    }
    
    std::sort(flights.begin(), flights.end(), 
              [](const Flight& a, const Flight& b) { return a.flightID < b.flightID; });
    
    for (const auto& flight : flights) {
        std::ostringstream oss;
        oss << "[flight" << flight.flightID 
            << ", airline" << flight.airlineID 
            << ", runway" << flight.runwayID 
            << ", start" << flight.startTime 
            << ", ETA" << flight.ETA << "]";
        output.push_back(oss.str());
    }
}

// Display scheduled flights within time range, sorted by ETA
void Scheduler::printSchedule(int t1, int t2, std::vector<std::string>& output) {
    std::vector<Flight> scheduled;
    
    for (auto& pair : activeFlights) {
        Flight& flight = pair.second;
        if (flight.state == SCHEDULED && 
            flight.startTime > currentTime &&
            flight.ETA >= t1 && flight.ETA <= t2) {
            scheduled.push_back(flight);
        }
    }
    
    if (scheduled.empty()) {
        output.push_back("There are no flights in that time period");
        return;
    }
    
    std::sort(scheduled.begin(), scheduled.end(), 
              [](const Flight& a, const Flight& b) {
                  if (a.ETA != b.ETA) return a.ETA < b.ETA;
                  return a.flightID < b.flightID;
              });
    
    for (const auto& flight : scheduled) {
        output.push_back("[" + std::to_string(flight.flightID) + "]");
    }
}

// Advance time and process all state changes
void Scheduler::tick(int time, std::vector<std::string>& output) {
    advanceTime(time, output);
}