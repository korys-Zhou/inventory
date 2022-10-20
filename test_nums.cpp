#include <assert.h>
#include <climits>
#include <iostream>
using namespace std;

#define FLOAT_NUMS

#ifdef INT_NUMS
int main() {
    int x1 = INT_MIN;
    cout << "x1 = " << x1 << " , x1 * 2 = " << x1 * 2
         << " , x1 << 1 = " << (x1 << 1) << endl;

    int x2 = INT_MAX;
    cout << "x2 = " << x2 << " , x2 * 2 = " << x2 * 2
         << " , x2 << 1 = " << (x2 << 1) << endl;

    unsigned int x3 = 5;
    cout << "usigned 5 > -1 ? answer is : " << (x3 > -1) << endl;
    cout << "usigned 5 > 1 ? answer is : " << (x3 > 1) << endl;

    cout << "int -1 = " << -1 << ", unsigned -1 = " << (unsigned int)-1 << endl;

    int x4 = INT_MAX, x5 = INT_MIN;
    cout << "-INT_MAX = " << -x4 << ", -INT_MIN = " << -x5 << endl;
    cout << "INT_MAX + 1 = " << x4 + 1 << ", INT_MIN - 1 = " << x5 - 1 << endl;

    int x6 = 50000;
    cout << "50000 * 50000 = " << x6 * x6 << endl;

    cout << "-1 >> 31 = " << (-1 >> 31) << endl;
    cout << "1 << 31 = " << (1 << 31) << endl;

    return 0;
}
#endif

#ifdef FLOAT_NUMS
int main() {
    assert(+0. == -0.);
    assert(1.0 / +0. == 1.0 / -0.);
    return 0;
}
#endif