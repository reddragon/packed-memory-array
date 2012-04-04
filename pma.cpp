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
    std::vector<int> exists_bitmask;
    // Upper thresholds for the level 0, and level l
    double t_0, t_l;
    // The space requirement for n elements would be cn
    // NOTE: c should be a power of 2 for easier math
    int c;
    // Number of levels = l+1
    int l;
    // Number of elements in the PMA (the size)
    uint32 s;

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
    assert(index < (8*sizeof(int)*exists_bitmask.size()));
    return (exists_bitmask[index/(8*sizeof(int))] & (1<<(index % (8*sizeof(int)))));
}

template <class E>
bool PackedMemoryArray<E>::is_too_full() const {
    // TODO Will change when we get lower thresholds
    return is_out_of_balance(s, l);
}

template <class E>
bool PackedMemoryArray<E>::is_out_of_balance(int n_elems, int level) const {
   // TODO Will change when we get lower thresholds
   std::cout << "Max capacity at level " << level << ": " << (int)floor(upper_threshold_at(level) * capacity_at(level))  << " " << n_elems << std::endl;
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
    return 1<<level;
}

template <class E>
PackedMemoryArray<E>::PackedMemoryArray(E e) : c(VAL_C), t_0(VAL_T_0), t_l(VAL_T_L) {
    // Assert that c is a power of 2 and > 1
    assert(c > 1 && !(c & (c-1)));
    s = 0;
    // Get the new store
    store.resize(c*1);
    // Resize the bitmask as well
    exists_bitmask.resize((size_t)ceil((c*1)*1.0/(8*sizeof(int))));
    insert_element_at(e, 0);
    // One liner log2 since c is a power of 2 :-P
    l = __builtin_popcount(c-1);
    // Now assert that the upper thresholds are sane, and you do not go out of balance the very first time.
    assert(!is_too_full());
    // And we have set this thing in motion. Pray!
}

template <class E>
PackedMemoryArray<E>::~PackedMemoryArray() {
}

template <class E>
void PackedMemoryArray<E>::print() const {
    //std::cout << s << " " << store_size() << " " << exists_bitmask.size() << std::endl;
    for (int i = 0; i < store_size(); i++) {
        if(!elem_exists_at(i))
            std::cerr << "-- ";
        else
            std::cerr << store[i] << " ";
    }
    std::cerr << std::endl;
}

template <class E>
void PackedMemoryArray<E>::insert_element_at(E e, int index) {
    // There is no element at index 'index'
    assert(!elem_exists_at(index));
    // Actually putting the element
    store[index] = e;
    // Marking the entry in the bitmask
    exists_bitmask[index/(8*sizeof(int))] |= (1<<(index % (8*sizeof(int))));
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
    std::cout << "Inserting " << e << " after " << after << std::endl;
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
    // TODO Put the search for rebalance logic here
    int node_index, node_level;
    if(smallest_interval_in_balance(insert_at, &node_index, &node_level) == -1) {
        // No more space left in the PMA. Resize!
        std::cout << "Need to expand the PMA" << std::endl;
        expand_PMA(e);
    }
    else {
        // Rebalance one particular level
        std::cout << "Is balanced from " << node_index << " at level " << node_level << std::endl;
        assert(false);
    }
}

template <class E>
int PackedMemoryArray<E>::smallest_interval_in_balance(int index, int * node_index, int * node_level) const {
    // If we are trying to insert at the end of the PMA
    if (index == store.size()) {
        index = store.size() - 1;
    }

    int level = -1, start = index, end = index, sz = 1, count = 1;
    bool found = false;
    do {
        // Get the boundaries of the next interval
        int left = start - (start % sz);
        int right = start + (sz - (start % sz)-1);

        // Count only the necessary parts
        for(int i = left; i < start; i++)
            count++;
        for(int i = end + 1; i <= right; i++)
            count++;
        
        start = left;
        end = right;
        
        ++level;
        std::cout << start << " " << end << " " << count << " " << is_out_of_balance(count+1, level) << " " << upper_threshold_at(level) << std::endl;
        // Would be able to fit another element?
        if(!is_out_of_balance(count+1, level)) {
            found = true;
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
    std::vector<E> new_exists_bitmask;
    new_exists_bitmask.resize((int)ceil((s+1)*1.0/(8*sizeof(int))));
    
    int count = 0, i;
    // Insert all elements less than e
    for(i = 0; i < store.size(); i++) 
        if(elem_exists_at(i)) {
            if(store[i] > e)
                break;
            new_exists_bitmask[count/(8*sizeof(int))] |= (1<<(count % (8*sizeof(int))));
            new_store[count++] = store[i];
        }
    
    // Insert the element we wanted
    new_exists_bitmask[count/(8*sizeof(int))] |= (1<<(count % (8*sizeof(int))));
    new_store[count++] = e;
    
    // Insert rest of the elements
    for(; i < store.size(); i++) 
        if(elem_exists_at(i)) {
            new_exists_bitmask[count/(8*sizeof(int))] |= (1<<(count % (8*sizeof(int))));
            new_store[count++] = store[i];
        }

    // Replace the existing store and bitmask
    store = new_store;
    exists_bitmask = new_exists_bitmask;
 
    // Increment the number of elements in the PMA
    s++;

    // Increment the levels in the PMA
    l++;
    
    // Now rebalance the entire PMA 
    std::cout << "Now rebalancing the entire PMA" << std::endl;
    rebalance(0, l);
}

template<class E>
void PackedMemoryArray<E>::rebalance(int index, int level) {
    assert(level <= l);
    int c = capacity_at(level);
    // Move all the elements to one side
    int last = index + c - 1, count = 0;
    for(int i = index + c - 1; i >= 0; i--) {
        if(elem_exists_at(i)) {
            // Copy the element to the leftmost position
            insert_element_at(store[i], last);
            // Delete the original copy of the element
            delete_element_at(i);
            // Update the leftmost pointer, and count of elements moved
            --last;
            count++;
        }
    }

    std::cout << "After moving to one side: " << std::endl;
    print();
     
    // Now copy
    double k = (c*1.0)/count, p = 0;
    int actual_index = last, correct_index;
    for(int i = 0; i < count; i++) {
        p += k;
        std::cout << p << std::endl;
        actual_index++;
        // Now insert the element at the right position
        correct_index = index + (int)p - 1;
        std::cout << "Add at " << correct_index << " and remove from " << actual_index << std::endl;
        if (correct_index == actual_index)
            continue;
        insert_element_at(store[actual_index], correct_index);
        // Remove the left most copy
        delete_element_at(actual_index);
    } 
}

template <class E>
void PackedMemoryArray<E>::delete_element_at(int index) {
    assert(elem_exists_at(index));
    // Just mark it non existent
    exists_bitmask[index/(8*sizeof(int))] ^= (1<<(index % (8*sizeof(int))));
}

/*
void print_intervals(int i, int n) {
    assert(!(n & (n-1)));
    int sz = 1;
    do {
        if(i % sz == 0)
            std::cout << i << " " << i + (sz - 1) << std::endl;
        else {
            std::cout << i - (i%sz) << " " << i + (sz - (i%sz)- 1) << std::endl;
        }
        sz <<= 1;
    } while(sz <= n);
}*/

int main() {
    int e = 3;
    PackedMemoryArray<int> pma(e);
    pma.print();
    pma.insert_element_after(4, 3);
    pma.print();
    //print_intervals(63, 128);
    pma.insert_element_after(5, 4);
    pma.print();
    pma.insert_element_after(6, 5);
    pma.print();
    pma.insert_element_after(7, 6);
    pma.print();
    pma.insert_element_after(9, 7);
    pma.print();
    pma.insert_element_after(8, 7);
    pma.print();

}
