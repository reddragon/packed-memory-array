#include <iostream>
#include <vector>
#include <cassert>
#include <cmath>
#include <cstdlib>
#include "include/timer.hpp"
#include <iostream>

// WARNING: Do not change this.
#define VAL_C 2
#define VAL_T_L 0.5
#define VAL_T_0 1.0
#define ELEM_EXISTS_AT(i) exists[i]
#define OPTIMIZE 1 
#define CAPACITY_AT(l) ((int)(segment_size<<l))
// WARNING

typedef unsigned int uint32;

template <class E> 
class PackedMemoryArray {
    // The actual array
    std::vector<E> store;
    // A bitmask to check if an element exists or not
    std::vector<bool> exists;
    // Upper thresholds for the level 0, and level l
    double t_0, t_l;
    // The space requirement for n elements would be cn
    // NOTE: c should be a power of 2 for easier math
    int c;
    // Number of levels = l+1
    int l;
    // Number of elements in the PMA (the size)
    uint32 s;
    // Segment size
    // Basically round up log2(n) to a power of 2
    int segment_size;
    // Total number of moves
    int total_moves;

    public:
    PackedMemoryArray();
    PackedMemoryArray(E e);
    PackedMemoryArray(std::vector<E> v);
    ~PackedMemoryArray();

    int upper_bound_in_segment(E e, int v);
    int upper_bound(E e);
    // A generic insert
    void insert_element(E e);
    // Insert after the element elem
    void insert_element_after(E e, E after, int pos = -1);
    // Insert at index
    void insert_element_at(E e, int index);
    
    // TODO Delete the element at index 'index'
    //      Support this later.
    //      bool delete_elem(int index);
    void delete_element_at(int index);

    // Return the element at index 'index'
    E elem_at(int index) const;
    // Does an element exist at position index?
    bool elem_exists_at(int index) const;
    // Find the location of element 'e'
    int find(E e) const;
    // Capacity at level 'level'
    uint32 capacity_at(int level) const;
    // Size of the PMA
    uint32 size() const;
    // Actual size of the store
    uint32 store_size() const;
    // Print the PMA
    void print() const;

    private:
    // Is the current PMA too full?
    bool is_too_full() const;
    // Is the 'level' level out of balance with n_elems elems?
    bool is_out_of_balance(int n_elems, int level) const;
    // Expand (double up) the current PMA and insert element e
    void expand_PMA(E e);
    // Rebalance from the index 'index' at level 'level'
    void rebalance(int index, int level);
    // Rebalance from the index 'index' at level 'level', and insert element 'e'
    void rebalance(int index, int level, E e);
    // Return the threshold at 'level'
    double upper_threshold_at(int level) const;
    // Find the smallest interval encompassing index 'index' which is not out of balance
    int smallest_interval_in_balance(int index, int * node_index, int * node_level) const;
};

template <class E>
double PackedMemoryArray<E>::upper_threshold_at(int level) const {
#ifndef OPTIMIZE
    assert(level <= l);
#endif
    return t_0 - ((t_0 - t_l) * 1.0 * level) / l; 
}

template <class E>
bool PackedMemoryArray<E>::elem_exists_at(int index) const {
#ifndef OPTIMIZE
    assert(index < (sizeof(int)*exists.size()));
#endif
    return (exists[index]);
}

template <class E>
bool PackedMemoryArray<E>::is_too_full() const {
    // TODO Will change when we get lower thresholds
    return is_out_of_balance(s, l);
}

template <class E>
bool PackedMemoryArray<E>::is_out_of_balance(int n_elems, int level) const {
   // TODO Will change when we get lower thresholds
   return ((int)floor(upper_threshold_at(level) * CAPACITY_AT(level)) < n_elems);
}

template <class E>
E PackedMemoryArray<E>::elem_at(int index) const {
#ifndef OPTIMIZE
    assert(ELEM_EXISTS_AT(index));
#endif
    return store[index];
}

template <class E>
uint32 PackedMemoryArray<E>::size() const {
    return s;
}

template <class E>
uint32 PackedMemoryArray<E>::store_size() const {
    return (uint32)(store.size());
}

template <class E>
uint32 PackedMemoryArray<E>::capacity_at(int level) const {
    return segment_size << level;
}

template <class E>
PackedMemoryArray<E>::PackedMemoryArray(E e) : t_0(VAL_T_0), t_l(VAL_T_L), c(VAL_C) {
    // Assert that c is a power of 2 and > 1
#ifndef OPTIMIZE
    assert(c > 1 && !(c & (c-1)));
#endif
    s = 0;
    // Get the new store
    store.resize(c*1);
    // Resize the bitmask as well
    exists.resize((size_t)ceil(c));
    insert_element_at(e, 0);
    
    // One liner log2 since c is a power of 2 :-P
    int log2n = __builtin_popcount(store.size()-1);
    if(log2n & (log2n-1)) {
        // log2n is not a power of 2, round it up to the nearest power of 2.
        segment_size = (int)floor(log2(1<<(log2n+1)));
    }
    else {
        // log2n is a power of 2, so, all is fine.
        segment_size = log2n;
    }
    l = log2n - log2(segment_size);

    // Now assert that the upper thresholds are sane, and you do not go out of balance the very first time.
#ifndef OPTIMIZE
    assert(!is_too_full());
#endif
    // And we have set this thing in motion. Pray!
}

template <class E>
PackedMemoryArray<E>::~PackedMemoryArray() {
}

template <class E>
void PackedMemoryArray<E>::print() const {
    int empty = 0;
    for (int i = 0; i < store_size(); i++) {
        if(!ELEM_EXISTS_AT(i)) 
            std::cerr << "-- ", empty++;
        else
            std::cerr << store[i] << " ";
    }
    std::cerr << std::endl;
    std::cerr << empty << "/" << store.size() << std::endl;
}

template <class E>
inline void PackedMemoryArray<E>::insert_element_at(E e, int index) {
    // There is no element at index 'index'
#ifndef OPTIMIZE
    assert(!ELEM_EXISTS_AT(index));
#endif
    // Actually putting the element
    store[index] = e;
    // Marking the entry in the bitmask
    exists[index] = 1;
    // The bitmask works fine
#ifndef OPTIMIZE
    assert(ELEM_EXISTS_AT(index));
#endif
    // Increase the size
    ++s;
}

template <class E>
int PackedMemoryArray<E>::find(E e) const {
    // TODO Make this binary search
    for(int i = 0; i < store.size(); i++) {
        if(ELEM_EXISTS_AT(i)) {
            if(store[i] == e)
                return i;
            else if(store[i] > e)
                return -1;
        }
    }
    return -1;
}

template <class E>
void PackedMemoryArray<E>::insert_element_after(E e, E after, int pos) {
    // Find where we can insert
    int loc;
    loc = pos;
#ifndef OPTIMIZE
    assert(loc != -1);
#endif
    int insert_at = ++loc;
    // Do we have space at the location we want to insert?
    if(insert_at < (int)store.size() && !ELEM_EXISTS_AT(insert_at)) {
        // Great! Now insert it there.
        insert_element_at(e, insert_at);
        return;
    }
    // The not so nice part begins here.
    int node_index, node_level;
    if(smallest_interval_in_balance(insert_at, &node_index, &node_level) == -1) {
        // No more space left in the PMA. Resize!
        expand_PMA(e);
    }
    else {
        // Rebalance one particular level
        rebalance(node_index, node_level, e);
    }
}

template <class E>
int PackedMemoryArray<E>::upper_bound_in_segment(E e, int v) {
    int best = -1;
    for(int i = v*segment_size; i < (v+1)*segment_size; i++)
        if(ELEM_EXISTS_AT(i) && store[i] <= e) 
            return best = i;
    return best;
}

template <class E>
int PackedMemoryArray<E>::upper_bound(E e) {
    int l = 0, r = ((int)store.size())/segment_size, pos; 
    while(l != r) {
        int m = l + (r - l + 1)/2;
        pos = upper_bound_in_segment(e, m);
        if (pos == -1) 
            r = m-1;
        else
            l = m; 
    }
    pos = upper_bound_in_segment(e, l);
    return pos;
}

template <class E>
inline void PackedMemoryArray<E>::insert_element(E e) {
    int pos = upper_bound(e);
    insert_element_after(e, store[pos], pos);
}

template <class E>
int PackedMemoryArray<E>::smallest_interval_in_balance(int index, int * node_index, int * node_level) const {
    // If we are trying to insert at the end of the PMA
    if (index == (int)store.size()) {
        index = (int)store.size() - 1;
    }

    int level = -1;
    int start = index;
    int end = index, count = 1;
    unsigned int sz = segment_size;
    bool found = false;
    do {
        // Get the boundaries of the next interval
        int left = start - (start % sz);
        int right = left + sz;

        // Count only the necessary parts
        for(int i = left; i < start; i++)
            if(ELEM_EXISTS_AT(i))
                count++;
        for(int i = end + 1; i <= right; i++)
            if(ELEM_EXISTS_AT(i))
                count++;
        
        start = left;
        end = right;

        ++level;
        bool is_balanced = !is_out_of_balance(count + 1, level);
        // std::cout << "Level: " << level << ", from " << left << " to " << right << ", having " << count+1 << ", elements, is balanced?: " << is_balanced << ", segment_size: " << smallest_window_size << std::endl;
        // Would be able to fit another element?
        if(is_balanced) {
            found = true;
            break;
        }
        sz <<= 1;
        
    } while(sz <= store.size());
    if(!found) {
        // We did not find a balanced interval
        *node_index = *node_level = -1;
        return -1;
    }

    *node_index = start;
    *node_level = level;
    return 1;
}

template <class E>
void PackedMemoryArray<E>::expand_PMA(E e) {
    // Create a new store
    std::vector<E> new_store;
    new_store.resize(store.size() * 2);
    std::vector<bool> new_exists;
    new_exists.resize((int)ceil(s+1));
    
    int count = 0, i;
    // Insert all elements less than e
    for(i = 0; i < (int)store.size(); i++) 
        if(ELEM_EXISTS_AT(i)) {
            if(store[i] > e)
                break;
            new_exists[count] = 1;
            new_store[count++] = store[i];
        }
    
    // Insert the element we wanted
    new_exists[count] = 1;
    new_store[count++] = e;
    
    // Insert rest of the elements
    for(; i < (int)store.size(); i++) 
        if(ELEM_EXISTS_AT(i)) {
            new_exists[count] = 1;
            new_store[count++] = store[i];
        }

    // Replace the existing store and bitmask
    store = new_store;
    exists = new_exists;
 
    // Increment the number of elements in the PMA
    s++;
    
    // std::cout << "Old smallest window size: " << segment_size << std::endl;
    // Recalculate l and segment_size
    int log2n = __builtin_popcount(store.size()-1);
    if(log2n & (log2n-1)) {
        // log2n is not a power of 2, round it up to the nearest power of 2.
        segment_size = 1<<((int)floor(log2(log2n<<1)));
    }
    else {
        // log2n is a power of 2, so, all is fine.
        segment_size = 1<<log2n;
    }
    l = log2n - log2(segment_size);
    // std::cout << "New smallest window size: " << segment_size << std::endl;

    // Now rebalance the entire PMA 
    rebalance(0, l);
}

template<class E>
void PackedMemoryArray<E>::rebalance(int index, int level, E e) {
#ifndef OPTIMZE
    assert(level <= l);
#endif
    int c = CAPACITY_AT(level);
    // Move all the elements to one side
    int last = index + c - 1, count = 0;
    bool element_inserted = false;
    std::vector<E> level_copy;
    for(int i = last; i >= index; i--) {
        if(ELEM_EXISTS_AT(i)) {
            if(!element_inserted && store[i] < e) {
                level_copy.push_back(e);
                element_inserted = true;
            }
            level_copy.push_back(store[i]);
            delete_element_at(i);
            --last;
            count++;
        }
    }

    if(!element_inserted)
        level_copy.push_back(e);

    // Now copy
    double k = (c*1.0)/(level_copy.size()), p = 0;
    int correct_index;
    for(int i = level_copy.size()-1; i >= 0; i--) {
        p += k;
        // Now insert the element at the right position
        correct_index = index + (int)p - 1;
        insert_element_at(level_copy[i], correct_index);
    } 
}


template<class E>
void PackedMemoryArray<E>::rebalance(int index, int level) {
#ifndef OPTIMIZE 
    assert(level <= l);
#endif
    int c = CAPACITY_AT(level);
    // Move all the elements to one side
    int last = index + c - 1, count = 0;
    for(int i = last; i >= index; i--) {
        if(ELEM_EXISTS_AT(i)) {
            if(i != last) {
                // Copy the element to the leftmost position
                insert_element_at(store[i], last);
                // Delete the original copy of the element
                #ifndef OPTIMIZE
                    delete_element_at(i);
                #else
                    exists[i] = 0;
                #endif
               // Update the leftmost pointer, and count of elements moved
            }
            --last;
            count++;
        }
    }

    // Now copy
    double k = (c*1.0)/count, p = 0;
    int actual_index = last, correct_index;
    for(int i = 0; i < count; i++) {
        p += k;
        actual_index++;
        // Now insert the element at the right position
        correct_index = index + (int)p - 1;
        if (correct_index == actual_index)
            continue;
        if(actual_index != correct_index)
            insert_element_at(store[actual_index], correct_index);
        // Remove the left most copy
#ifndef OPTIMIZE
        delete_element_at(actual_index);
#else
        exists[actual_index] = 0;
#endif
    } 
}

template <class E>
void PackedMemoryArray<E>::delete_element_at(int index) {
#ifndef OPTIMIZE
    assert(ELEM_EXISTS_AT(index));
#endif
    // Just mark it non existent
    exists[index] = 0;
}

int main() {
    
    PackedMemoryArray<int> pma(2);
    
    Timer t;
    t.start();
    for(int i = 3; i < 10000000; i++) {
        pma.insert_element(i);
    }
    double time_taken = t.stop();
    std::cout << "Head Inserts: " << time_taken/10000000.0 << std::endl;
    //pma.print();

}

