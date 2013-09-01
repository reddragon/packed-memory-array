## Benchmarks

### Implementation-1

Time to insert 10<sup>7</sup> elements: 0m30.180s<sup>*</sup>

### Implementation-2

Time to insert 10<sup>7</sup> elements: 0m25.048s<sup>*</sup>

<small>* (When compiled with -O2 using g++, on a 2.3 GHz Intel Core i7 machine with 8G RAM)</small>

### Analysis

* Complexity of an insert: O(log<sup>2</sup>n) (amortized)

* Complexity of find (binary search): O(log<sup>2</sup>n) (worst-case)
