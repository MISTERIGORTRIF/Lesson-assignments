#include <iostream>
#include <thread>
#include <chrono>

using namespace std;

void printer(int n) {
    for (int i = 0; i < 5; ++i) {
        cout << i + 1 << endl;
        this_thread::sleep_for(chrono::milliseconds(n));
    }
}

int main() {
    
    setlocale(LC_ALL, "RU");

    thread t1(printer, 200);
    thread t2(printer, 800);
    for (int i = 0; i < 5; ++i) {
        cout << "Main Thread running: " << i + 1 << endl;
        this_thread::sleep_for(chrono::milliseconds(150));
    }

    t1.join();
    t2.detach();
    this_thread::sleep_for(chrono::seconds(3));
    return 0;
}