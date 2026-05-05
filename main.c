#include "slate_runtime.h"

extern void main(void);

int main_caller(int argc, char** argv) {
    main();
    return 0;
}