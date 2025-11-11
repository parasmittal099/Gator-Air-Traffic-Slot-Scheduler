#ifndef MINHEAP_H
#define MINHEAP_H

#include <vector>
#include <functional>
#include <stdexcept>
#include <iostream>
#include <utility>

/**
 * Generic Binary Min-Heap implementation from scratch

 * Usage examples:
 *   MinHeap<RunwayInfo> runwayPool;  // min heap of runways
 *   MinHeap<TimetableEntry> timetable;  // min heap of flight completions
 */

template<typename T, typename Compare = std::less<T>>
class MinHeap {
private:
    std::vector<T> heap;     
    Compare comp;       
    
    // Get parent index
    int parent(int i) const {
        return (i - 1) / 2;
    }
    
    // Get left child index
    int leftChild(int i) const {
        return 2 * i + 1;
    }
    
    // Get right child index
    int rightChild(int i) const {
        return 2 * i + 2;
    }
    
    // Bubble up element at index i to maintain heap property
    void bubbleUp(int i) {
        while (i > 0 && comp(heap[i], heap[parent(i)])) {
            // Swap with parent if current is smaller
            std::swap(heap[i], heap[parent(i)]);
            i = parent(i);
        }
    }
    
    // Bubble down element at index i to maintain heap property
    void bubbleDown(int i) {
        int size = heap.size();
        
        while (true) {
            int smallest = i;
            int left = leftChild(i);
            int right = rightChild(i);
            
            // Check if left child is smaller
            if (left < size && comp(heap[left], heap[smallest])) {
                smallest = left;
            }
            
            // Check if right child is smaller
            if (right < size && comp(heap[right], heap[smallest])) {
                smallest = right;
            }
            
            // If current node is smallest, we're done
            if (smallest == i) {
                break;
            }
            
            // Otherwise, swap and continue
            std::swap(heap[i], heap[smallest]);
            i = smallest;
        }
    }

public:
    // Constructor
    MinHeap() : comp(Compare()) {}
    
    // Constructor with custom comparator
    explicit MinHeap(const Compare& comparator) : comp(comparator) {}
    
    // Insert a new element into the heap
    void push(const T& value) {
        heap.push_back(value);
        bubbleUp(heap.size() - 1);
    }
    
    // Remove and return the minimum element
    T pop() {
        if (isEmpty()) {
            throw std::runtime_error("MinHeap::pop() - Heap is empty");
        }
        
        T minValue = heap[0];
        
        // Move last element to root
        heap[0] = heap.back();
        heap.pop_back();
        
        // Restore heap property if heap is not empty
        if (!isEmpty()) {
            bubbleDown(0);
        }
        
        return minValue;
    }
    
    // Get the minimum element without removing it
    const T& top() const {
        if (isEmpty()) {
            throw std::runtime_error("MinHeap::top() - Heap is empty");
        }
        return heap[0];
    }
    
    // Check if heap is empty
    bool isEmpty() const {
        return heap.empty();
    }
    
    // Get the number of elements in the heap
    int size() const {
        return heap.size();
    }
    
    // Clear all elements from the heap
    void clear() {
        heap.clear();
    }
    
    // Build heap from existing vector (heapify) - O(n)
    void buildHeap(const std::vector<T>& elements) {
        heap = elements;
        
        // Start from last non-leaf node and bubble down
        for (int i = (heap.size() / 2) - 1; i >= 0; --i) {
            bubbleDown(i);
        }
    }
    
    // Extract all elements that satisfy a condition (for Phase 1 completions)
    // Returns elements in sorted order (smallest first)
    std::vector<T> extractWhile(std::function<bool(const T&)> condition) {
        std::vector<T> result;
        
        while (!isEmpty() && condition(top())) {
            result.push_back(pop());
        }
        
        return result;
    }
    
    // Get all elements in the heap (for debugging/inspection)
    // WARNING: This breaks encapsulation, use only for debugging
    const std::vector<T>& getElements() const {
        return heap;
    }
    
    // Print heap structure (for debugging)
    void printHeap() const {
        std::cout << "Heap contents (" << size() << " elements): ";
        for (const auto& elem : heap) {
            std::cout << elem << " ";
        }
        std::cout << std::endl;
    }
};

#endif // MINHEAP_H