## Tests

### std::vector

#### Hammer Inserts / Insertion at Head
Time to insert 10<sup>5</sup> elements: 10.68 secs
Time to insert 10<sup>6</sup> elements: 15 min+ (didn't finish within 15mins)

### std::set

The comparison is not fair because set maintains a tree structure, and to retrieve the entire sequence, 
you will spend O(n log n) time, whereas, both PMA and std::vector can retrieve the sequence in O(n) time.

#### Hammer Inserts / Insertion at Head
Time to insert 10<sup>7</sup> elements: 9.44 secs

### std::deque

#### Insertion at Head
Time to insert 10<sup>7</sup> elements: 2.56 secs
(Praying that it doesn't do as well for randomized input)

