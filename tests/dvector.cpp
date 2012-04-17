#include "dvector.hpp"
#include <ctime>

int
main() {
    deamortized_vector<int> dvi;
    vector<int> vi;
    for (int i = 0; i < 40000; ++i) {
		clock_t start = clock();
		for (int j = 0; j < 128; ++j) {
			dvi.push_back(j);
			//vi.push_back(j);
		}
		clock_t end = clock();
		if (end - start + 1 > 1)
			printf("%d\t%ld\n", i, end - start + 1);
    }
    /*
    for (int i = 0; i < 100; ++i) {
        // printf("%3d, ", dvi[i]);
        assert(dvi[i] == i);
    }*/
}
