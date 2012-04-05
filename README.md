## Benchmarks

### Implementation-1

Time to insert 10<sup>7</sup> elements: 3min 8sec<sup>*</sup>

### Implementation-2

Time to insert 10<sup>7</sup> elements: 2min 40sec<sup>*</sup>

<small>* (When compiled with -O2 using g++, on a 1.73 GHz Pentium M machine)</small>

### Analysis

* Complexity of an insert: O(log<sup>2</sup>n) (amortized)

* Complexity of find (binary search): O(log<sup>2</sup>n) (worst-case)
