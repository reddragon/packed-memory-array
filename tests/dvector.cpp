#include "dvector.hpp"

int
main() {
    deamortized_vector<int> dvi;
    // vector<int> dvi;
    for (int i = 0; i < 50000000; ++i) {
        dvi.push_back(i);
    }
    for (int i = 0; i < 100; ++i) {
        // printf("%3d, ", dvi[i]);
        assert(dvi[i] == i);
    }
}
