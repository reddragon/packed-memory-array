#include <cstdio>
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
    void insert_elem(E elem);
    
    // TODO Delete the element at index 'index'
    //      Support this later.
    //      bool delete_elem(int index);
    
    // Return the element at index 'index'
    E elem_at(int index) const;
    // Does an element exist at position index?
    bool elem_exists_at(int index) const;
    // Capacity at level 'level'
    uint32 capacity_at(int level) const;
    // Size of the PMA
    uint32 size() const;
    // Print the PMA
    void print() const;

    private:
    // Is the current PMA too full?
    bool is_too_full() const;
    // Is the 'level' level out of balance with n_elems elems?
    bool is_out_of_balance(int n_elems, int level) const;
    // Create a new PMA of size 'n_elems' 
    void new_PMA(int n_elems);
    // Rebalance the n-th node at level 'level'
    void rebalance(int node_n, int level);
    // Return the threshold at 'level'
    double upper_threshold_at(int level) const;
    // Put element e at index 'index'
    void put_element_at(E e, int index);
};

template <class E>
double PackedMemoryArray<E>::upper_threshold_at(int level) const {
    assert(level <= l);
    return t_0 - ((t_0 - t_l) * 1.0 * level) / l; 
}

template <class E>
bool PackedMemoryArray<E>::elem_exists_at(int index) const {
    assert(index < (sizeof(int)*exists_bitmask.size()));
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
   return ((int)floor(upper_threshold_at(level) * capacity_at(level)) > n_elems);
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
uint32 PackedMemoryArray<E>::capacity_at(int level) const {
    return 1<<level;
}

template <class E>
PackedMemoryArray<E>::PackedMemoryArray(E e) : c(VAL_C), t_0(VAL_T_0), t_l(VAL_T_L) {
    // Assert that c is a power of 2 and > 1
    assert(c > 1 && !(c & (c-1)));
    // Get the new store
    store.resize(c*1);
    // Resize the bitmask as well
    exists_bitmask.resize((size_t)ceil((c*1)*1.0/(8*sizeof(int))));
    put_element_at(e, 0);
    // One liner log2 since c is a power of 2 :-P
    l = __builtin_popcount(c-1);
    s = 1;
    // Now assert that the upper thresholds are sane, and you do not go out of balance the very first time.
    assert(!is_too_full());
    // And we have set this thing in motion. Pray!
}

template <class E>
PackedMemoryArray<E>::~PackedMemoryArray() {
}

template <class E>
void PackedMemoryArray<E>::print() const {
}

template <class E>
void PackedMemoryArray<E>::put_element_at(E e, int index) {
    // There is no element at index 'index'
    assert(!elem_exists_at(index));
    store[index] = e;
    exists_bitmask[index/(8*sizeof(int))] |= (1<<(index % (8*sizeof(int))));
    assert(elem_exists_at(index));
}

int main() {
    int e = 3;
    PackedMemoryArray<int> pma(e);
}
