#include <cstdio>
#include <vector>
#include <cassert>

#define DEFAULT_C 2
#define DEFAULT_T_L 0.5
#define DEFAULT_T_0 1.0

template <class E> 
class PackedMemoryArray {
    // The actual array
    std::vector<E> store;
    // A bitmask to check if an element exists or not
    std::vector<int> exists_bitmask;
    // Upper thresholds for the level 0, and level l
    double T_0, T_l;
    // The space requirement for n elements would be cn
    int c;
    // Number of levels = l+1
    int l;

    public:
    PackedMemoryArray();
    PackedMemoryArray(E e);
    PackedMemoryArray(std::vector<E> v);
    ~PackedMemoryArray();

    // Insert after the element elem
    void insert_elem(E elem);
    
    // TODO Delete the element at index 'index'
    //      Support this later.
    //      bool delete_elem(int index);
    
    // Return the element at index 'index'
    E elem_at(int index) const;
    // Does an element exist at position index?
    bool elem_exists_at(int index) const;
    // Capacity at level 'level'
    int capacity_at(int level) const;

    private:
    // Is the current PMA too full, with n_elems elements?
    bool is_too_full(int n_elems) const;
    // Is the 'level' level out of balance with n_elems elems?
    bool is_out_of_balance(int n_elems, int level) const;
    // Create a new PMA of size 'n_elems' 
    void new_PMA(int n_elems);
    // Rebalance the n-th node at level 'level'
    void rebalance(int node_n, int level);
    // Return the threshold at 'level'
    double upper_threshold_at(int level) const;
};

template <class E>
double PackedMemoryArray<E>::upper_threshold_at(int level) const {
    assert(level <= l);
    return T_0 - ((T_0 - T_l) * 1.0 * level) / l; 
}

template <class E>
bool PackedMemoryArray<E>::elem_exists_at(int index) const {
    assert(index < (sizeof(int)*exists_bitmask.size()));
    return (exists_bitmask[index/sizeof(int)] & (1<<(index % sizeof(int))));
}

template <class E>
bool PackedMemoryArray<E>::is_too_full(int n_elems) const {
    // TODO Will change when we get lower thresholds
    return is_out_of_balance(n_elems, l);
}

template <class E>
bool PackedMemoryArray<E>::is_out_of_balance(int n_elems, int level) const {
   // TODO Will change when we get lower thresholds
   return ((int)floor(upper_threshold_at(level) * capacity_at(level)) > n_elems);
}

template <class E>
E PackedMemoryArray<E>::elem_at(int index) const {
    assert(elem_exists_at(index));
    return store[index];
}

template <class E>
PackedMemoryArray<E>::PackedMemoryArray(E e) {
    
}

int main() {
}
