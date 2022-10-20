#include <assert.h>
#include <malloc.h>
#include <string.h>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

using namespace std;

void test_malloc_leak(int size) {
    malloc(1024);
}

void no_leak() {
    void* p = malloc(1024 * 1024 * 10);
    free(p);
}

int main() {
    no_leak();
    test_malloc_leak(1024);
    return 0;
}