#include <deque>
#include <iterator>
#include <cstdio>
#include <algorithm>
#include "../include/timer.hpp"

using namespace std;

int main() {
    deque<int> d;
    /*
    d.push_back(2);
    d.push_back(5);
    d.push_back(11);
    d.push_back(20);
    deque<int>::iterator it = lower_bound(d.begin(), d.end(), 7);
    printf("%d\n", *it);
    d.insert(it, 7);
    for (int i = 0; i < d.size(); i++)
        printf("%d\n", d[i]);
    */
    Timer t;
    t.start();

    int upto = 10000000;
    for (int i = 0; i < upto; i++) {
        deque<int>::iterator it = lower_bound(d.begin(), d.end(), upto-i);
        d.insert(it, upto-i);
    }
    double time_taken = t.stop() / 1000000.0;
    printf("Time taken for hammer inserts of %d elements: %lf seconds\n", upto, time_taken);
}
