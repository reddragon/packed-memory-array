#include <iostream>
#include <vector>
#include <cstdio>
#include "../include/timer.hpp"

using namespace std;
int main()
{
	Timer t;
	t.start();    
    
    vector<int> m;
    int upto = 100000;
    for(int i = 0; i < upto; i++)
        m.insert(m.begin(),upto-i);
        
	double time_taken = (t.stop()) / (1000000.0);
    printf("Time taken for hammer inserts of %d elements: %lf seconds\n", upto, time_taken);
}
