#ifndef PAIRINGHEAP_H
#define PAIRINGHEAP_H

#include <vector>
#include <stdexcept>
#include <algorithm>
#include "Flight.h"

/**
 * Pairing Heap Node
 * Structure: Each node has a pointer to its leftmost child and right sibling
 * This creates a "child-sibling" tree representation
 */
template<typename KeyType>
struct PairingNode {
    KeyType key;                    // Priority key for comparison
    Flight* flight;                 // Pointer to the flight data (payload)
    PairingNode* child;             // Leftmost child
    PairingNode* sibling;           // Right sibling
    
    // Constructor
    PairingNode(const KeyType& k, Flight* f)
        : key(k), flight(f), child(nullptr), sibling(nullptr) {}
};

/**
 * Key structure for flight priority comparison
 * Ordering: (priority, -submitTime, -flightID)
 * Higher priority comes first (max-heap)
 */
struct FlightKey {
    int priority;
    int submitTime;
    int flightID;
    
    FlightKey(int p, int s, int f) 
        : priority(p), submitTime(s), flightID(f) {}
    
    // Comparison for MAX heap (higher priority should be "less" for max heap semantics)
    // We want: higher priority > lower priority
    bool operator<(const FlightKey& other) const {
        if (priority != other.priority) {
            return priority < other.priority;  // Higher priority is "greater"
        }
        if (submitTime != other.submitTime) {
            return submitTime > other.submitTime;  // Earlier submitTime is "greater"
        }
        return flightID > other.flightID;  // Smaller flightID is "greater"
    }
    
    bool operator>(const FlightKey& other) const {
        if (priority != other.priority) {
            return priority > other.priority;
        }
        if (submitTime != other.submitTime) {
            return submitTime < other.submitTime;
        }
        return flightID < other.flightID;
    }
};

/**
 * Max Pairing Heap Implementation (Two-Pass Scheme)
 * Used for Pending Flights Queue
 * 
 * Key Operations:
 * - insert: O(1) amortized
 * - extractMax: O(log n) amortized
 * - increaseKey: O(log n) amortized (for priority increase)
 * - erase: O(log n) amortized
 */
class PairingHeap {
private:
    PairingNode<FlightKey>* root;
    int heapSize;
    
    /**
     * Merge two pairing heap trees
     * The tree with larger key becomes the parent
     */
    PairingNode<FlightKey>* merge(PairingNode<FlightKey>* h1, PairingNode<FlightKey>* h2) {
        if (h1 == nullptr) return h2;
        if (h2 == nullptr) return h1;
        
        // Compare keys: larger key should be root (max-heap)
        if (h2->key > h1->key) {
            std::swap(h1, h2);
        }
        
        // Make h2 the leftmost child of h1
        h2->sibling = h1->child;
        h1->child = h2;
        
        return h1;
    }
    
    /**
     * Two-pass merge for combining children
     * Pass 1: Pair up siblings from left to right
     * Pass 2: Merge pairs from right to left
     */
    PairingNode<FlightKey>* mergePairs(PairingNode<FlightKey>* firstChild) {
        if (firstChild == nullptr || firstChild->sibling == nullptr) {
            return firstChild;
        }
        
        // Collect all children into a vector for easier two-pass processing
        std::vector<PairingNode<FlightKey>*> children;
        PairingNode<FlightKey>* current = firstChild;
        
        while (current != nullptr) {
            PairingNode<FlightKey>* next = current->sibling;
            current->sibling = nullptr;  // Break the sibling link
            children.push_back(current);
            current = next;
        }
        
        // Pass 1: Pair adjacent children from left to right
        std::vector<PairingNode<FlightKey>*> paired;
        for (size_t i = 0; i + 1 < children.size(); i += 2) {
            paired.push_back(merge(children[i], children[i + 1]));
        }
        
        // If odd number of children, add the last one
        if (children.size() % 2 == 1) {
            paired.push_back(children.back());
        }
        
        // Pass 2: Merge all pairs from right to left
        PairingNode<FlightKey>* result = nullptr;
        for (int i = paired.size() - 1; i >= 0; --i) {
            result = merge(paired[i], result);
        }
        
        return result;
    }
    
    /**
     * Delete a subtree recursively
     */
    void deleteTree(PairingNode<FlightKey>* node) {
        if (node == nullptr) return;
        
        deleteTree(node->child);
        deleteTree(node->sibling);
        delete node;
    }
    
    /**
     * Find and remove a node from the tree
     * Returns true if found and removed
     */
    bool removeNode(PairingNode<FlightKey>* node) {
        if (node == nullptr) return false;
        
        // If it's the root, use extractMax logic
        if (node == root) {
            extractMax();
            return true;
        }
        
        // Find the node's parent and remove it from parent's child list
        // This requires traversing from root
        return removeNodeHelper(root, node);
    }
    
    /**
     * Helper to remove a specific node from the tree
     */
    bool removeNodeHelper(PairingNode<FlightKey>* parent, PairingNode<FlightKey>* target) {
        if (parent == nullptr) return false;
        
        // Check if target is a direct child
        if (parent->child == target) {
            // Remove from child list and merge its subtrees
            parent->child = target->sibling;
            
            // Merge target's children back into the heap
            if (target->child != nullptr) {
                PairingNode<FlightKey>* merged = mergePairs(target->child);
                root = merge(root, merged);
            }
            
            target->child = nullptr;
            target->sibling = nullptr;
            delete target;
            heapSize--;
            return true;
        }
        
        // Check siblings
        PairingNode<FlightKey>* current = parent->child;
        while (current != nullptr && current->sibling != target) {
            current = current->sibling;
        }
        
        if (current != nullptr && current->sibling == target) {
            // Remove from sibling list
            current->sibling = target->sibling;
            
            // Merge target's children back
            if (target->child != nullptr) {
                PairingNode<FlightKey>* merged = mergePairs(target->child);
                root = merge(root, merged);
            }
            
            target->child = nullptr;
            target->sibling = nullptr;
            delete target;
            heapSize--;
            return true;
        }
        
        // Recursively search in children
        current = parent->child;
        while (current != nullptr) {
            if (removeNodeHelper(current, target)) {
                return true;
            }
            current = current->sibling;
        }
        
        return false;
    }

public:
    // Constructor
    PairingHeap() : root(nullptr), heapSize(0) {}
    
    // Destructor
    ~PairingHeap() {
        clear();
    }
    
    /**
     * Insert a new flight into the heap
     * Returns a handle (pointer) to the node for future operations
     */
    PairingNode<FlightKey>* push(Flight* flight) {
        FlightKey key(flight->priority, flight->submitTime, flight->flightID);
        PairingNode<FlightKey>* newNode = new PairingNode<FlightKey>(key, flight);
        
        root = merge(root, newNode);
        heapSize++;
        
        return newNode;
    }
    
    /**
     * Extract the maximum priority flight
     */
    Flight* extractMax() {
        if (isEmpty()) {
            throw std::runtime_error("PairingHeap::extractMax() - Heap is empty");
        }
        
        Flight* maxFlight = root->flight;
        PairingNode<FlightKey>* oldRoot = root;
        
        // Merge all children using two-pass scheme
        root = mergePairs(root->child);
        
        delete oldRoot;
        heapSize--;
        
        return maxFlight;
    }
    
    /**
     * Get the maximum priority flight without removing it
     */
    Flight* top() const {
        if (isEmpty()) {
            throw std::runtime_error("PairingHeap::top() - Heap is empty");
        }
        return root->flight;
    }
    
    /**
     * Increase the priority of a flight
     * Used when Reprioritize increases priority
     */
    void increaseKey(PairingNode<FlightKey>* node, int newPriority) {
        if (node == nullptr) return;
        
        // Update the key
        node->key.priority = newPriority;
        node->flight->priority = newPriority;
        
        // If it's already root or doesn't violate heap property, done
        if (node == root) return;
        
        // Otherwise, cut the node and merge it back to potentially become new root
        // For simplicity in pairing heaps, we can remove and reinsert
        // (More efficient implementations exist, but this is correct)
        Flight* flight = node->flight;
        erase(node);
        push(flight);
    }
    
    /**
     * Erase a specific node from the heap
     * Used for cancellation or when priority decreases
     */
    void erase(PairingNode<FlightKey>* node) {
        if (node == nullptr) return;
        removeNode(node);
    }
    
    /**
     * Check if heap is empty
     */
    bool isEmpty() const {
        return root == nullptr;
    }
    
    /**
     * Get the number of elements
     */
    int size() const {
        return heapSize;
    }
    
    /**
     * Clear all elements
     */
    void clear() {
        deleteTree(root);
        root = nullptr;
        heapSize = 0;
    }
    
    /**
     * Collect all flights in the heap (for rescheduling)
     * Returns vector of all flights
     */
    std::vector<Flight*> getAllFlights() {
        std::vector<Flight*> flights;
        collectFlights(root, flights);
        return flights;
    }
    
private:
    /**
     * Helper to collect all flights from tree
     */
    void collectFlights(PairingNode<FlightKey>* node, std::vector<Flight*>& flights) {
        if (node == nullptr) return;
        
        flights.push_back(node->flight);
        collectFlights(node->child, flights);
        collectFlights(node->sibling, flights);
    }
};

#endif