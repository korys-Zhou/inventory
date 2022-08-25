#include <vector>

using namespace std;

void quickSort(vector<int>& arr, int l, int r) {
    if (l + 1 >= r)
        return;
    int t = arr[l];
    int ll = l, rr = r - 1;
    while (ll < rr) {
        while (ll < rr && arr[rr] >= t)
            --rr;
        arr[ll++] = arr[rr];
        while (ll < rr && arr[ll] < t)
            ++ll;
        arr[rr--] = arr[ll];
    }
    arr[ll] = t;
    quickSort(arr, l, t);
    quickSort(arr, t + 1, r);
}