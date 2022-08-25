#include <unistd.h>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

using namespace std;

#define MAX_SIZE 100
#define MAX_NUM 1000

int id = 0;
queue<int> resource;
mutex mtx_p, mtx_c;
condition_variable cond_p, cond_c;

void run_producer() {
    while (true) {
        unique_lock<mutex> lk(mtx_p);
        while (resource.size() >= MAX_SIZE) {
            cond_p.wait(lk);
        }
        if (id >= MAX_NUM)
            break;
        int cur = id++;
        resource.push(cur);
        lk.unlock();
        cond_c.notify_all();
        printf("id = %d produced.\n", cur);
    }
}

void run_consumer() {
    while (true) {
        unique_lock<mutex> lk(mtx_c);
        while (resource.empty()) {
            cond_c.wait(lk);
        }
        int cur = resource.front();
        resource.pop();
        lk.unlock();
        cond_p.notify_all();
        printf("id = %d consumed.\n", cur);
    }
}

int main() {
    vector<thread> pros, cons;
    for (int i = 0; i < 4; ++i) {
        pros.emplace_back(run_producer);
    }
    for (int i = 0; i < 4; ++i) {
        cons.emplace_back(run_consumer);
    }
    for (auto& th : pros)
        th.join();
    for (auto& th : cons)
        th.join();
    return 0;
}