#include <iostream>
#include <vector>
#include <cassert>
#include <cmath>

// WARNING: Do not change this.
#define VAL_C 2
#define VAL_T_L 0.5
#define VAL_T_0 1.0
// WARNING

typedef unsigned int uint32;

template <class E> 
class PackedMemoryArray {
    // The actual array
    std::vector<E> store;
    // A bitmask to check if an element exists or not
    std::vector<bool> exists_bitmask;
    // Upper thresholds for the level 0, and level l
    double t_0, t_l;
    // The space requirement for n elements would be cn
    // NOTE: c should be a power of 2 for easier math
    int c;
    // Number of levels = l+1
    int l;
    // Number of elements in the PMA (the size)
    uint32 s;
    // Smallest window size
    // Basically round up log2(n) to a power of 2
    int smallest_window_size;
    // Total number of moves
    int total_moves;

    public:
    PackedMemoryArray();
    PackedMemoryArray(E e);
    PackedMemoryArray(std::vector<E> v);
    ~PackedMemoryArray();

    // Insert after the element elem
    void insert_element_after(E e, E after);
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
    assert(level <= l);
    return t_0 - ((t_0 - t_l) * 1.0 * level) / l; 
}

template <class E>
bool PackedMemoryArray<E>::elem_exists_at(int index) const {
    assert(index < (sizeof(int)*exists_bitmask.size()));
    return (exists_bitmask[index]);
}

template <class E>
bool PackedMemoryArray<E>::is_too_full() const {
    // TODO Will change when we get lower thresholds
    return is_out_of_balance(s, l);
}

template <class E>
bool PackedMemoryArray<E>::is_out_of_balance(int n_elems, int level) const {
   // TODO Will change when we get lower thresholds
   return ((int)floor(upper_threshold_at(level) * capacity_at(level)) < n_elems);
}

template <class E>
E PackedMemoryArray<E>::elem_at(int index) const {
    assert(elem_exists_at(index));
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
    return smallest_window_size << level;
}

template <class E>
PackedMemoryArray<E>::PackedMemoryArray(E e) : c(VAL_C), t_0(VAL_T_0), t_l(VAL_T_L) {
    // Assert that c is a power of 2 and > 1
    assert(c > 1 && !(c & (c-1)));
    s = 0;
    // Get the new store
    store.resize(c*1);
    // Resize the bitmask as well
    exists_bitmask.resize((size_t)ceil(c));
    insert_element_at(e, 0);
    
    // One liner log2 since c is a power of 2 :-P
    int log2n = __builtin_popcount(store.size()-1);
    if(log2n & (log2n-1)) {
        // log2n is not a power of 2, round it up to the nearest power of 2.
        smallest_window_size = (int)floor(log2(1<<(log2n+1)));
    }
    else {
        // log2n is a power of 2, so, all is fine.
        smallest_window_size = log2n;
    }
    l = log2n - log2(smallest_window_size);

    // Now assert that the upper thresholds are sane, and you do not go out of balance the very first time.
    assert(!is_too_full());
    // And we have set this thing in motion. Pray!
}

template <class E>
PackedMemoryArray<E>::~PackedMemoryArray() {
}

template <class E>
void PackedMemoryArray<E>::print() const {
    int empty = 0;
    for (int i = 0; i < store_size(); i++) {
        if(!elem_exists_at(i)) 
            std::cerr << "-- ", empty++;
        else
            std::cerr << store[i] << " ";
    }
    std::cerr << std::endl;
    std::cerr << empty << "/" << store.size() << std::endl;
}

template <class E>
void PackedMemoryArray<E>::insert_element_at(E e, int index) {
    // There is no element at index 'index'
    assert(!elem_exists_at(index));
    // Actually putting the element
    store[index] = e;
    // Marking the entry in the bitmask
    exists_bitmask[index] = 1;
    // The bitmask works fine
    assert(elem_exists_at(index));
    // Increase the size
    ++s;
}

template <class E>
int PackedMemoryArray<E>::find(E e) const {
    // TODO Make this binary search
    for(int i = 0; i < store.size(); i++) {
        if(elem_exists_at(i)) {
            if(store[i] == e)
                return i;
            else if(store[i] > e)
                return -1;
        }
    }
    return -1;
}

template <class E>
void PackedMemoryArray<E>::insert_element_after(E e, E after) {
    // Find where we can insert
    int loc = find(after);
    assert(loc != -1);
    int insert_at = ++loc;
    // Do we have space at the location we want to insert?
    if(insert_at < store.size() && !elem_exists_at(insert_at)) {
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
int PackedMemoryArray<E>::smallest_interval_in_balance(int index, int * node_index, int * node_level) const {
    // If we are trying to insert at the end of the PMA
    if (index == store.size()) {
        index = store.size() - 1;
    }

    int level = -1;
    int start = index;
    int end = index, sz = smallest_window_size, count = 1;
    bool found = false;
    do {
        // Get the boundaries of the next interval
        int left = start - (start % sz);
        int right = left + sz;

        // Count only the necessary parts
        for(int i = left; i < start; i++)
            if(elem_exists_at(i))
                count++;
        for(int i = end + 1; i <= right; i++)
            if(elem_exists_at(i))
                count++;
        
        start = left;
        end = right;

        ++level;
        bool is_balanced = !is_out_of_balance(count + 1, level);
        std::cout << "Level: " << level << ", from " << left << " to " << right << ", having " << count+1 << ", elements, is balanced?: " << is_balanced << ", smallest_window_size: " << smallest_window_size << std::endl;
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
    std::vector<bool> new_exists_bitmask;
    new_exists_bitmask.resize((int)ceil(s+1));
    
    int count = 0, i;
    // Insert all elements less than e
    for(i = 0; i < store.size(); i++) 
        if(elem_exists_at(i)) {
            if(store[i] > e)
                break;
            new_exists_bitmask[count] = 1;
            new_store[count++] = store[i];
        }
    
    // Insert the element we wanted
    new_exists_bitmask[count] = 1;
    new_store[count++] = e;
    
    // Insert rest of the elements
    for(; i < store.size(); i++) 
        if(elem_exists_at(i)) {
            new_exists_bitmask[count] = 1;
            new_store[count++] = store[i];
        }

    // Replace the existing store and bitmask
    store = new_store;
    exists_bitmask = new_exists_bitmask;
 
    // Increment the number of elements in the PMA
    s++;
    
    std::cout << "Old smallest window size: " << smallest_window_size << std::endl;
    // Recalculate l and smallest_window_size
    int log2n = __builtin_popcount(store.size()-1);
    if(log2n & (log2n-1)) {
        // log2n is not a power of 2, round it up to the nearest power of 2.
        smallest_window_size = 1<<((int)floor(log2(log2n<<1)));
    }
    else {
        // log2n is a power of 2, so, all is fine.
        smallest_window_size = 1<<log2n;
    }
    l = log2n - log2(smallest_window_size);
    std::cout << "New smallest window size: " << smallest_window_size << std::endl;

    // Now rebalance the entire PMA 
    rebalance(0, l);
}

template<class E>
void PackedMemoryArray<E>::rebalance(int index, int level, E e) {
    std::cout << "Rebalance at level " << level << " " << capacity_at(level) << std::endl;
    assert(level <= l);
    int c = capacity_at(level);
    //print();
    // Move all the elements to one side
    int last = index + c - 1, count = 0;
    bool element_inserted = false;
    std::vector<E> level_copy;
    for(int i = last; i >= index; i--) {
        if(elem_exists_at(i)) {
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
    
    // Now copy
    double k = (c*1.0)/(level_copy.size()), p = 0;
    int actual_index = 0, correct_index;
    for(int i = level_copy.size()-1; i >= 0; i--) {
        p += k;
        // Now insert the element at the right position
        correct_index = index + (int)p - 1;
        insert_element_at(level_copy[i], correct_index);
    } 
}


template<class E>
void PackedMemoryArray<E>::rebalance(int index, int level) {
    assert(level <= l);
    int c = capacity_at(level);
    // Move all the elements to one side
    int last = index + c - 1, count = 0;
    for(int i = last; i >= index; i--) {
        if(elem_exists_at(i)) {
            if(i != last) {
                // Copy the element to the leftmost position
                insert_element_at(store[i], last);
                // Delete the original copy of the element
                delete_element_at(i);
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
        delete_element_at(actual_index);
    } 
}

template <class E>
void PackedMemoryArray<E>::delete_element_at(int index) {
    assert(elem_exists_at(index));
    // Just mark it non existent
    exists_bitmask[index] = 0;
}

int main() {
    float e = 3;
    PackedMemoryArray<float> pma(e);
    
    pma.print();
    pma.insert_element_after(4, 3);
    pma.print();
    pma.insert_element_after(5, 4);
    pma.print();
    pma.insert_element_after(4, 3);
    pma.print();
    pma.insert_element_after(4, 3);
    pma.print();
    pma.insert_element_after(4, 3);
    pma.print();
    pma.insert_element_after(4, 3);
    pma.print();
    pma.insert_element_after(4, 3);
    pma.print();
    pma.insert_element_after(4, 3);
    pma.print();
    pma.insert_element_after(4, 3);
    pma.print();
    pma.insert_element_after(4, 3);
    pma.print();
    pma.insert_element_after(4, 3);
    pma.print();
    pma.insert_element_after(3.5, 3);
    pma.print();
    pma.insert_element_after(3.6, 3.5);
    pma.print();
    pma.insert_element_after(3.7, 3.6);
    pma.print();
    pma.insert_element_after(3.8, 3.7);
    pma.print();
    pma.insert_element_after(5.2, 5);
    pma.print();
    pma.insert_element_after(5.3, 5.2);
    pma.print();
    pma.insert_element_after(3.65, 3.6);
    pma.print();







    /*
    pma.insert_element_after(9, 7);
    pma.print();
    pma.insert_element_after(8, 7);
    pma.print(); 
    pma.insert_element_after(6, 5);
    pma.print();
    pma.insert_element_after(9, 8);
    pma.print();
    pma.insert_element_after(9, 8);
    pma.print();
    pma.insert_element_after(9, 8);
    pma.print();
    pma.insert_element_after(5, 4);
    pma.print();
    pma.insert_element_after(5, 4);
    pma.print();
    pma.insert_element_after(5, 4);
    pma.print();

 
    pma.insert_element_after(5, 4);
    pma.print();
    */
}

