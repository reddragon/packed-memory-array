#include <cstdio>
#include <vector>
#include <cassert>

#define DEFAULT_C 2
#define DEFAULT_T_O 0.5
#define DEFAULT_T_L 1.0

template <class E> 
class PackedMemoryArray {
    // The actual array
    std::vector<E> store;
    // A bitmask to check if an element exists or not
    std::vector<int> exists_bitmask;
    // Upper thresholds for the level 0, and level l
    double T_0, T_l;
    // The space requirement for n elements would be cn
    double c;
    // Number of levels
    int total_levels;

    public:
    PackedMemoryArray();
    PackedMemoryArray(E e);
    PackedMemoryArray(std::vector<E> v);
    ~PackedMemoryArray();

    // Insert after the element elem
    void insert_elem(E elem);
    // Delete the element at index 'index'
    bool delete_elem(int index);
    // Return the element at index 'index'
    E elem_at(int index);
    // Does an element exist at position index?
    bool elem_exists_at(int index);
    
    private:
    // Is the current PMA too full?
    bool is_too_full();
    // Create a new PMA of size 'n_elems' 
    void new_PMA(int n_elems);
    // Rebalance the n-th node at level 'level'
    void rebalance(int n, int level);
    // Return the threshold at 'level'
    double threshold_at_level(int level);
};

template <class E>
double PackedMemoryArray<E>::threshold_at_level(int level) {
    assert(level <= total_levels);
    return T_0 - ((T_0 - T_l) * 1.0 * level) / total_levels; 
}

template <class E>
bool PackedMemory<E>::elem_exists_at(int index) {
    assert(index < (sizeof(int)*exists_bitmask.size()));
    return exists_bitmask[index/sizeof(int)];
}



int main() {
}
