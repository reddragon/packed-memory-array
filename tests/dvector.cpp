#include "dvector.hpp"
#include "../include/timer.hpp"

int
main() {
    deamortized_vector<int> dvi;
    vector<int> vi;
    Timer t;
    for (int i = 0; i < 4096; ++i) {
		t.start();
		for (int j = 0; j < 128; ++j) {
			dvi.push_back(j);
			//vi.push_back(j);
		}
		//if (end - start + 1 > 1)
		double d = t.stop();
		printf("%d\t%lf\n", i<<7, d);
    }
    /*
    for (int i = 0; i < 100; ++i) {
        // printf("%3d, ", dvi[i]);
        assert(dvi[i] == i);
    }*/
}
