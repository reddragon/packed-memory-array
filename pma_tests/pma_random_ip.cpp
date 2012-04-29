#include <cstdio>
#include <cstdlib>

int main(int argc, char** argv) {
    if (argc < 3) {
        return 1;
    }

    freopen(argv[2], "w", stdout);
    srand(0);
    int upto = atoi(argv[1]);
    printf("%d\n", upto);
    for (int i = 0; i < upto; i++) {
        printf("%d\n", rand());
    }
}
