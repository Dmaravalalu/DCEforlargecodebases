#include <stdio.h>
int main() {
#ifdef HELLO
    printf("hello defined\n");
#else
    printf("hello not defined\n");
#endif
    return 0;
}
