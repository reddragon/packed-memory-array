#include <iostream>
#include <algorithm>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <// assert.h>
#include "../include/timer.hpp"

using namespace std;

// #define dprintf(args...) printf(args)
#define dprintf(args...)

typedef vector<int> vi_t;

int log2(int n) {
    int lg2 = 0;
    while (n > 1) {
        n /= 2;
        ++lg2;
    }
    return lg2;
}

unsigned int nmoves = 0;

struct PMA {
    vi_t impl;
    int nelems;
    vector<bool> present;
    int chunk_size;
    int nchunks;
    int nlevels;
    int lgn;
    vi_t tmp;

    struct PMAIterator {
        PMA *pma;
        int i;

        PMAIterator(PMA *p, int _i)
            : pma(p), i(_i)
        { }

        PMAIterator(const PMAIterator &rhs) {
            this->pma = rhs.pma;
            this->i   = rhs.i;
        }

        PMAIterator&
        operator=(PMAIterator &rhs) {
            this->pma = rhs.pma;
            this->i   = rhs.i;
            return *this;
        }

        PMAIterator&
        operator++() {
            if (i < (int)pma->impl.size()) ++i;
            while (i < (int)pma->impl.size() && !pma->present[i]) {
                ++i;
            }
            return *this;
        }

        PMAIterator
        operator++(int) {
            PMAIterator tmp = *this;
            ++(*this);
            return tmp;
        }

        bool
        operator==(PMAIterator rhs) {
            return this->pma == rhs.pma && this->i == rhs.i;
        }

        bool
        operator!=(PMAIterator rhs) {
            return !(*this == rhs);
        }

        int&
        operator*() {
            // assert(pma->present[this->i]);
            return pma->impl[this->i];
        }

        int*
        operator->() {
            // assert(pma->present[this->i]);
            return &(pma->impl[this->i]);
        }
    };

    typedef PMAIterator iterator;

    PMA(int capacity = 2)
        : nelems(0) {
        // assert(capacity > 1);
        // assert(1 << log2(capacity) == capacity);

        this->init_vars(capacity);
        this->impl.resize(capacity);
        this->present.resize(capacity);
    }


    double
    upper_threshold_at(int level) const {
        // assert(level <= this->nlevels);
        double threshold = 1.0 - ((1.0 - 0.5) * level) / (double)this->lgn;
        return threshold;
    }

    void
    init_vars(int capacity) {
        this->chunk_size = 1 << log2(log2(capacity) * 2);
        // assert(this->chunk_size == (1 << log2(this->chunk_size)));
        this->nchunks = capacity / this->chunk_size;
        this->nlevels = log2(this->nchunks);
        this->lgn = log2(capacity);
        dprintf("init_vars::capacity: %d, nelems: %d, chunk_size: %d, nchunks: %d\n", capacity, nelems, chunk_size, nchunks);
    }

    int
    left_interval_boundary(int i, int interval_size) {
        // assert(interval_size == (1 << log2(interval_size)));
        // assert(i < (int)this->impl.size());

        int q = i / interval_size;
        int boundary = q * interval_size;
        dprintf("left_interval_boundary(%d, %d) = %d\n", i, interval_size, boundary);
        return boundary;
    }

    void
    resize(int capacity) {
        // assert(capacity > this->impl.size());
        // assert(1 << log2(capacity) == capacity);

        vi_t tmpi(capacity);
        vector<bool> tmpp(capacity);
        double d = (double)capacity / this->nelems;
        int ctr = 0;
        for (int i = 0; i < (int)this->impl.size(); ++i) {
            if (this->present[i]) {
                int idx = d*(ctr++);
                tmpp[idx] = true;
                tmpi[idx] = this->impl[i];
            }
        }
        this->impl.swap(tmpi);
        this->present.swap(tmpp);
        this->init_vars(capacity);
        nmoves += this->impl.size();
        // dprintf("After resize: ");
        // this->print();
    }

    void
    get_interval_stats(int left, int level, bool &in_limit, int &sz) {
        double t = upper_threshold_at(level);
        int w = (1 << level) * this->chunk_size;
        sz = 0;
        for (int i = left; i < left + w; ++i) {
            sz += this->present[i] ? 1 : 0;
        }
        double q = (double)sz / double(w);
        dprintf("q: %f, t: %f\n", q, t);
        in_limit = q < t;
    }

    int
    lb_in_chunk(int l, int v) {
        int i;
        for (i = l; i < l + chunk_size; ++i) {
            if (this->present[i]) {
                if (this->impl[i] >= v) {
                    return i;
                }
            }
        }
        return i;
    }

    int
    lower_bound(int v) {
        int i;
        if (this->nelems == 0) {
            i = this->impl.size();
        } else {
#if 0
            for (i = 0; i < this->impl.size(); ++i) {
                if (this->present[i] && !(this->impl[i] < v)) {
                    break;
                }
            }
#else
            int l = 0, r = this->nchunks;
            int m;
            while (l != r) {
                m = l + (r-l)/2;
                int sz;
                int left = left_interval_boundary(m * chunk_size, chunk_size);
                int pos = lb_in_chunk(left, v);

                // Why does this work? We assume that every chunk of
                // size this->chunk_size contains at least 1
                // element. Hence, if we reach the end of an interval
                // without finding a lower bound, we conclude that all
                // the elements in this chunk are < 'v'. Because every
                // chunk contains at least 1 element, we will never
                // reach the end of an interval because the interval
                // is empty.
                //
                // Note: This is why we need lower density thresholds!
                // 
                if (pos == left + chunk_size) {
                    // Move to right half
                    l = m + 1;
                } else {
                    r = m;
                }
            }
            i = l * chunk_size;
#endif
        }
        dprintf("lower_bound(%d) == %d\n", v, i);
        return i;
    }

    void
    insert_merge(int l, int v) {
        dprintf("insert_merge(%d, %d)\n", l, v);
        // Insert by merging elements in a window of size 'chunk_size'
        tmp.clear();
        tmp.reserve(this->chunk_size);
        for (int i = l; i < l + this->chunk_size; ++i) {
            if (this->present[i]) {
                this->present[i] = false;
                tmp.push_back(this->impl[i]);
            }
        }
        vi_t::iterator iter = std::lower_bound(tmp.begin(), tmp.end(), v);
        tmp.insert(iter, v);

        dprintf("insert_merge::tmp.size(): %d\n", tmp.size());
        for (int i = 0; i < tmp.size(); ++i) {
            this->present[l + i] = true;
            this->impl[l + i] = tmp[i];
        }
        ++this->nelems;
        nmoves += chunk_size;
    }

    void
    rebalance_interval(int left, int level) {
        dprintf("rebalance_interval(%d, %d)\n", left, level);
        int w = (1 << level) * this->chunk_size;
        tmp.clear();
        tmp.reserve(w);
        for (int i = left; i < left + w; ++i) {
            if (this->present[i]) {
                tmp.push_back(this->impl[i]);
                this->present[i] = false;
            }
        }
        double m = (double)(1<<level)*chunk_size / (double)tmp.size();
        dprintf("m: %f, tmp.size(): %d\n", m, tmp.size());
        // assert(m >= 1.0);
        for (int i = 0; i < tmp.size(); ++i) {
            int k = i * m + left;
            if (k >= left + w) {
                dprintf("k: %d, left+w: %d\n", k, left + w);
            }
            // assert(k < left + w);
            this->present[k] = true;
            this->impl[k] = tmp[i];
        }
        nmoves += w;
    }

    void
    insert(int v) {
        /*
        if ((this->nelems + 2) * 2 > this->impl.size()) {
            // resize array
            this->resize(2 * this->impl.size());
        }
        */

        int i = lower_bound(v);
        if (i == this->impl.size()) {
            --i;
        }
        // assert(i > -1);
        // assert(i < this->impl.size());

        // Check in a window of size 'w'
        int w = chunk_size;
        int level = 0;
        int l = this->left_interval_boundary(i, w);

        // Number of elements in current window. We just need sz to be
        // less than w -- we don't need the exact value of 'sz' here.
        int sz = w - 1;

        bool in_limit = false;

        // If the current chunk has space, then the last element will
        // be unused (with significant probability). First check that
        // as a quick check.
        if (this->present[l + this->chunk_size - 1]) {
            get_interval_stats(l, level, in_limit, sz);
        }

        if (sz < w) {
            // There is some space in this interval. We can just
            // shuffle elements and insert.
            this->insert_merge(l, v);
        } else {
            // No space in this interval. Find an interval above this
            // interval that is within limits, re-balance, and
            // re-start insertion.
            in_limit = false;
            while (!in_limit) {
                w *= 2;
                level += 1;
                // // assert(level <= this->nlevels);
                if (level > this->nlevels) {
                    // Root node is out of balance. Resize array.
                    this->resize(2 * this->impl.size());
                    this->insert(v);
                    return;
                }

                l = this->left_interval_boundary(i, w);
                get_interval_stats(l, level, in_limit, sz);
                dprintf("level: %d, this->nlevels: %d, in_limit: %d, sz: %d\n", level, this->nlevels, in_limit, sz);
            }
            this->rebalance_interval(l, level);
            this->insert(v);
        }

    } // insert(int v)

    int
    size() const {
        return this->nelems;
    }

    iterator
    begin() {
        return iterator(this, 0);
    }

    iterator
    end() {
        return iterator(this, this->impl.size());
    }

    void
    print() {
        for (int i = 0; i < (int)this->impl.size(); ++i) {
            printf("%3d ", this->present[i] ? this->impl[i] : -1);
        }
        printf("\n");
    }

};
n
template <typename Iter>
bool
is_sorted(Iter f, Iter l) {
    Iter next = f;
    while (f != l) {
        ++next;
        if (next != l && *f > *next) {
            return false;
        }
        f = next;
    }
    return true;
}

int
main() {
    PMA p1;
    for (int i = 0; i < 100; ++i) {
        p1.insert(10000000 - i);
        // v.insert(v.begin(), 100000 - i);
    }
    //printf("Time taken: %lf\n", ms/1000000.0);
    printf("%d moves to insert %d elements\n", nmoves, p1.size());
}
