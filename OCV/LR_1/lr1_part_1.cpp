#include <iostream>
#include <thread>
#include <chrono>
using namespace std;
void printnum(int n) {
    for (size_t i = 0; i < 6; i++) {
        cout << "Thread: " << n << " " << i << "\n";
        this_thread::sleep_for(std::chrono::milliseconds(300));
    }
}

int main() {
    setlocale(LC_ALL, "ru");
    thread t1(printnum, 1);
    thread t2(printnum, 2);
    for (size_t i = 0; i < 6; i++) {
        cout << " main" << " " << i << "\n";
        this_thread::sleep_for(std::chrono::milliseconds(400));
    }
    t1.join();
    t2.join();
    return 0;
}