#include <iostream>
using namespace std;

int main() {
    int a = 0x12345678;
    if ((char)a == 0x12)
        cout << "big" << endl;
    else
        cout << "little" << endl;
    union {
        int b;
        char c;
    } temp;
    temp.b = 0x12345678;
    if (temp.c == 0x12)
        cout << "big" << endl;
    else
        cout << "little" << endl;
    return 0;
}