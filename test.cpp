#if 0
#include <unistd.h>
#include <iostream>
#include <string>
using namespace std;

int main() {
    const char* s = "abcde";
    s += 2;
    printf("%d", s);
    return 0;
}
#endif

#if 0
void decode1(int* xp, int* yp, int* zp) {
    int x = *xp, y = *yp, z = *zp;
    *yp = x, *zp = y, *xp = z;
}
#endif

#if 0
int shift_left2_rightn(unsigned int x, int n) {
    x <<= 2;
    x >>= n;
    return x;
}
#endif

#if 0
int switch_eg(int x) {
    int ret = x;
    switch (x) {
        case 100:
            ret *= 13;
            break;

        case 102:
            ret += 10;

        case 103:
            ret += 11;
            break;

        case 104:
        case 106:
            ret *= ret;
            break;

        default:
            ret = 0;
            break;
    }
    return ret;
}
#endif

#if 1
int loop_plus(int x) {
    int ret = 0;
    while (x < 10) {
        ret += x;
        ++x;
    }
    return ret;
}
#endif

#if 0
int loop_plus(int x) {
    int ret = 0;
    do {
        ret += x;
        ++x;
    } while (x < 10);
    return ret;
}
#endif