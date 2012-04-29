#include <set>
#include <cstdio>
#include "../include/timer.hpp"

using namespace std;

int main() {
    set<int> s;
    Timer t;
    t.start();
    
    int upto = 10000000;
    for (int i = 0; i < upto; i++) {
        s.insert(i);
    }

    double time_taken = (t.stop() / 1000000.0);
    printf("Time taken for hammer inserts of %d elements: %lf seconds\n", upto, time_taken);
}
