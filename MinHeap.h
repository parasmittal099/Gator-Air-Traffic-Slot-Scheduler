#ifndef MIN_HEAP_H
#define MIN_HEAP_H

#include <vector>
#include <utility>

template<typename T>
class MinHeap {
private:
    std::vector<T> heap;
    
    // Get parent index
    int parent(int i) { return (i - 1) / 2; }
    // Get left child index
    int leftChild(int i) { return 2 * i + 1; }
    // Get right child index
    int rightChild(int i) { return 2 * i + 2; }
    
    // Move element up to maintain heap property
    void heapifyUp(int i) {
        while (i > 0 && heap[parent(i)] > heap[i]) {
            std::swap(heap[i], heap[parent(i)]);
            i = parent(i);
        }
    }
    
    // Move element down to maintain heap property
    void heapifyDown(int i) {
        int smallest = i;
        int left = leftChild(i);
        int right = rightChild(i);
        
        if (left < (int)heap.size() && heap[left] < heap[smallest])
            smallest = left;
        if (right < (int)heap.size() && heap[right] < heap[smallest])
            smallest = right;
            
        if (smallest != i) {
            std::swap(heap[i], heap[smallest]);
            heapifyDown(smallest);
        }
    }
    
public:
    MinHeap() {}
    
    // Insert element
    void push(const T& value) {
        heap.push_back(value);
        heapifyUp(heap.size() - 1);
    }
    
    // Get minimum element
    T top() const {
        return heap[0];
    }
    
    // Remove minimum element
    void pop() {
        if (heap.empty()) return;
        heap[0] = heap.back();
        heap.pop_back();
        if (!heap.empty())
            heapifyDown(0);
    }
    
    // Check if heap is empty
    bool empty() const {
        return heap.empty();
    }
    
    // Get heap size
    size_t size() const {
        return heap.size();
    }
    
    // Clear all elements
    void clear() {
        heap.clear();
    }
};

// Runway structure for runway pool
struct Runway {
    int nextFreeTime;
    int runwayID;
    
    Runway(int id, int time) : nextFreeTime(time), runwayID(id) {}
    
    bool operator>(const Runway& other) const {
        if (nextFreeTime != other.nextFreeTime)
            return nextFreeTime > other.nextFreeTime;
        return runwayID > other.runwayID;
    }
    
    bool operator<(const Runway& other) const {
        if (nextFreeTime != other.nextFreeTime)
            return nextFreeTime < other.nextFreeTime;
        return runwayID < other.runwayID;
    }
};

// Timetable entry for completions queue
struct TimetableEntry {
    int ETA;
    int flightID;
    int runwayID;
    
    TimetableEntry(int eta, int fid, int rid) 
        : ETA(eta), flightID(fid), runwayID(rid) {}
    
    bool operator>(const TimetableEntry& other) const {
        if (ETA != other.ETA)
            return ETA > other.ETA;
        return flightID > other.flightID;
    }
    
    bool operator<(const TimetableEntry& other) const {
        if (ETA != other.ETA)
            return ETA < other.ETA;
        return flightID < other.flightID;
    }
};

#endif // MIN_HEAP_H