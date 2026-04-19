//#include <iostream>
//#include <thread>
//#include <future>
//#include <atomic>
//#include <vector>
//#include <chrono>
//#include <random>
//
//using namespace std;
//
////Атомарный счётчик выполненных задач
//atomic<int> tasks_done{ 0 };
//
//// Вычислительная задача: факториал числа n 
//unsigned long long factorial(int n) {
//    unsigned long long result = 1;
//    for (int i = 2; i <= n; ++i) {
//        result *= i;
//        this_thread::sleep_for(chrono::milliseconds(100));
//    }
//    return result;
//}
//
//// Функция для запуска задачи в потоке
//void run_task(packaged_task<unsigned long long()> task) {
//    task();
//    tasks_done.fetch_add(1, memory_order_release); //увеличить счётчик
//}
//
//// Поток-монитор
//void monitor(int total_tasks, promise<void>&& prom) {
//    while (tasks_done.load(memory_order_acquire) < total_tasks) {
//        this_thread::sleep_for(chrono::milliseconds(10));
//    }
//    prom.set_value(); //уведомить основной поток
//}
//
//int main() {
//
//    setlocale(LC_ALL, "RU");
//
//    const int N = 100; 
//
//    vector<future<unsigned long long>> futures;
//    vector<thread> workers;
//
//    // Создаём promise и future для оповещения о завершении всех задач
//    promise<void> all_done_promise;
//    future<void> all_done_future = all_done_promise.get_future();
//
//    //Генератор случайных чисел для выбора аргументов от 1 до 20
//    random_device rd;
//    mt19937 gen(rd());
//    uniform_int_distribution<> dist(1, 20);
//
//    // Запускаем N задач
//    for (int i = 0; i < N; ++i) {
//        int arg = dist(gen); 
//        //Создаём packaged_task, который вычислит факториал
//        packaged_task<unsigned long long()> task([arg]() { return factorial(arg); });
//        // Получаем future для результата
//        futures.push_back(task.get_future());
//        //Запускаем поток с этой задачей
//        workers.emplace_back(run_task, move(task));
//    }
//
//    // Запускаем поток-монитор
//    thread monitor_thread(monitor, N, move(all_done_promise));
//
//    // Основной поток ждёт сигнала от монитора
//    cout << "Основной поток ожидает завершения всех " << N << " задач...\n";
//    all_done_future.wait(); // блокировка до сигнала
//    cout << "Все задачи выполнены! Получаем результаты:\n";
//
//    // Собираем результаты
//    for (size_t i = 0; i < futures.size(); ++i) {
//        unsigned long long res = futures[i].get();
//        cout << "Задача " << i + 1 << ": факториал = " << res << endl;
//    }
//
//    //Ждём завершения всех потоков
//    for (auto& t : workers) {
//        t.join();
//    }
//    monitor_thread.join();
//
//    return 0;
//}