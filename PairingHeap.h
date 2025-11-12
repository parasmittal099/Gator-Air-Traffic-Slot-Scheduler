#ifndef PAIRING_HEAP_H
#define PAIRING_HEAP_H

#include "Flight.h"
#include <vector>

struct PairingNode {
    int priority;
    int negSubmitTime;
    int negFlightID;
    Flight* flight;
    PairingNode* leftChild;
    PairingNode* nextSibling;
    
    PairingNode(Flight* f) 
        : priority(f->priority), 
          negSubmitTime(-f->submitTime), 
          negFlightID(-f->flightID),
          flight(f), 
          leftChild(nullptr), 
          nextSibling(nullptr) {}
    
    bool operator>(const PairingNode& other) const {
        if (priority != other.priority) return priority > other.priority;
        if (negSubmitTime != other.negSubmitTime) return negSubmitTime > other.negSubmitTime;
        return negFlightID > other.negFlightID;
    }
};

class PairingHeap {
private:
    PairingNode* root;
    
    // Merge two heap trees
    PairingNode* merge(PairingNode* h1, PairingNode* h2) {
        if (!h1) return h2;
        if (!h2) return h1;
        
        if (*h1 > *h2) {
            h2->nextSibling = h1->leftChild;
            h1->leftChild = h2;
            return h1;
        } else {
            h1->nextSibling = h2->leftChild;
            h2->leftChild = h1;
            return h2;
        }
    }
    
    // Two-pass merge: pair children, then merge right to left
    PairingNode* mergePairs(PairingNode* node) {
        if (!node || !node->nextSibling) return node;
        
        // Two-pass scheme
        std::vector<PairingNode*> trees;
        
        // First pass: merge pairs left to right
        while (node) {
            PairingNode* first = node;
            PairingNode* second = node->nextSibling;
            
            if (second) {
                node = second->nextSibling;
                first->nextSibling = nullptr;
                second->nextSibling = nullptr;
                trees.push_back(merge(first, second));
            } else {
                node = nullptr;
                trees.push_back(first);
            }
        }
        
        // Second pass: merge right to left
        PairingNode* result = trees.back();
        for (int i = trees.size() - 2; i >= 0; i--) {
            result = merge(trees[i], result);
        }
        
        return result;
    }
    
    // Recursively delete all nodes
    void clearHelper(PairingNode* node) {
        if (!node) return;
        clearHelper(node->leftChild);
        clearHelper(node->nextSibling);
        delete node;
    }
    
public:
    PairingHeap() : root(nullptr) {}
    
    ~PairingHeap() {
        clear();
    }
    
    // Delete all nodes
    void clear() {
        clearHelper(root);
        root = nullptr;
    }
    
    // Insert flight into heap
    PairingNode* push(Flight* flight) {
        PairingNode* newNode = new PairingNode(flight);
        root = merge(root, newNode);
        return newNode;
    }
    
    // Get minimum priority flight
    Flight* top() {
        return root ? root->flight : nullptr;
    }
    
    // Remove and return minimum
    void pop() {
        if (!root) return;
        PairingNode* oldRoot = root;
        root = mergePairs(root->leftChild);
        delete oldRoot;
    }
    
    // Check if heap is empty
    bool empty() const {
        return root == nullptr;
    }
};

#endif // PAIRING_HEAP_H