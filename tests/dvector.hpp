#include <vector>
#include <iostream>
#include <stdio.h>
#include <malloc.h>
#include <assert.h>

using namespace std;


template <typename T>
struct deamortized_vector {
    T* impl;
    T* prev;
    size_t implsz, prevcap, implcap, prevsz;
    // previ is the index of the last unprocessed element
    size_t previ;

    deamortized_vector() {
        previ = 0;
        prevcap = implcap = implsz = prevsz = 0;
        impl = prev = NULL;
    }

    void
    push_back(T const &x) {
        if (implcap > 0 && implsz == implcap - 1) {
            // We will resize on this insertion.
            // Allocate space for new elements, but don't copy them.
            prevcap = implcap;
            implcap = implcap * 2;
            prevsz = implsz;
            T *tmp = (T*)malloc(sizeof(T) * implcap);
            previ = 0;
            std::swap(prev, impl);
            std::swap(impl, tmp);
            free(tmp);
            // implsz remains unchanged
        } else if (implcap == 0) {
            implcap = 2;
            implsz = 0;
            impl = (T*)malloc(sizeof(T) * implcap);
        }

        // Copy at most 2 elements to the new array.
        int c = 0;
        // printf("previ: %d, prevsz: %d\n", previ, prevsz);
        while (c++ < 2 && previ < prevsz) {
            impl[previ] = prev[previ];
            ++previ;
        }

        impl[implsz++] = x;
    }

    size_t
    size() const {
        return implsz;
    }

    T&
    operator[](size_t i) {
        // printf("i: %d, impl.size(): %d, previ: %d\n", i, impl.size(), previ);
        assert(i < implsz);
        assert(previ <= prevsz);
        if (i >= previ && i < prevsz) {
            return prev[i];
        }
        return impl[i];
    }

};


