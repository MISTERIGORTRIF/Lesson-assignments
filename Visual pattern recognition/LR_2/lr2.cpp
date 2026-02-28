#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <chrono>
#include <vector>

// Общая очередь задач
std::queue<int> tasks;
// Мьютекс для защиты очереди
std::mutex queue_mutex;
// Условная переменная для уведомлений
std::condition_variable cv;
// Флаг завершения работы (после добавления всех задач)
bool finished = false;

// thread_local переменная для подсчёта обработанных задач каждым потребителем
thread_local int tasks_processed = 0;

// Производитель: генерирует числа от 1 до 20 и помещает в очередь
void producer(int total_tasks) {
    for (int i = 1; i <= total_tasks; ++i) {
        // Имитация времени генерации задачи
        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        // Блокируем мьютекс для добавления задачи
        {
            std::lock_guard<std::mutex> lock(queue_mutex);
            tasks.push(i);
            std::cout << "Производитель добавил задачу: " << i << std::endl;
        }
        // Уведомляем один из ожидающих потоков-потребителей
        cv.notify_one();
    }

    // Устанавливаем флаг завершения и уведомляем всех
    {
        std::lock_guard<std::mutex> lock(queue_mutex);
        finished = true;
    }
    cv.notify_all();
}

// Потребитель: извлекает задачи из очереди и обрабатывает их
void consumer(int id) {
    while (true) {
        std::unique_lock<std::mutex> lock(queue_mutex);
        // Ждём, пока очередь не станет непустой или не завершится работа
        cv.wait(lock, [] { return !tasks.empty() || finished; });

        if (!tasks.empty()) {
            // Извлекаем задачу
            int task = tasks.front();
            tasks.pop();
            // Разблокируем мьютекс на время обработки (демонстрация unique_lock::unlock)
            lock.unlock();

            // Обработка задачи (вычисляем квадрат числа)
            int result = task * task;
            // Имитация длительности обработки
            std::this_thread::sleep_for(std::chrono::milliseconds(100));

            // Увеличиваем счётчик обработанных задач данного потока (thread_local)
            ++tasks_processed;

            // Вывод информации с идентификатором потока
            std::cout << "Потребитель " << id << " (ID: " << std::this_thread::get_id()
                << ") обработал задачу " << task << ", квадрат = " << result << std::endl;
        }
        else if (finished) {
            // Очередь пуста и производство завершено – выходим из цикла
            break;
        }
    }

    // Вывод статистики по текущему потоку-потребителю
    std::cout << "Потребитель " << id << " (ID: " << std::this_thread::get_id()
        << ") обработал всего задач: " << tasks_processed << std::endl;
}

// Демонстрация recursive_mutex и проблемы deadlock с обычным mutex
std::recursive_mutex rec_mtx;   // рекурсивный мьютекс
std::mutex plain_mtx;           // обычный мьютекс

void log_details_recursive() {
    std::lock_guard<std::recursive_mutex> lock(rec_mtx);
    std::cout << "  [recursive] log_details: захватила мьютекс" << std::endl;
}

void log_message_recursive() {
    std::lock_guard<std::recursive_mutex> lock(rec_mtx);
    std::cout << "[recursive] log_message: захватила мьютекс" << std::endl;
    log_details_recursive();    // рекурсивный захват – безопасно
}

// Аналогичные функции с обычным mutex – при вызове приведут к deadlock
/*
void log_details_plain() {
    std::lock_guard<std::mutex> lock(plain_mtx);
    std::cout << "  [plain] log_details: захватила мьютекс" << std::endl;
}

void log_message_plain() {
    std::lock_guard<std::mutex> lock(plain_mtx);
    std::cout << "[plain] log_message: захватила мьютекс" << std::endl;
    log_details_plain();    // здесь поток попытается захватить уже захваченный мьютекс -> deadlock
}
*/

// Функция для демонстрации работы recursive_mutex (безопасный вариант)
void demonstrate_recursive_mutex() {
    std::cout << "\n=== Демонстрация recursive_mutex ===" << std::endl;
    log_message_recursive();
    std::cout << "recursive_mutex отработал без проблем." << std::endl;

    // Если раскомментировать вызов log_message_plain(), программа зависнет.
    // std::cout << "\nПопытка вызова с обычным mutex (приведёт к deadlock) – закомментировано." << std::endl;
    // log_message_plain();
}

int main() {
    setlocale(LC_ALL, "RU");

    const int total_tasks = 20;
    const int num_consumers = 3;

    // Запускаем производителя
    std::thread prod(producer, total_tasks);

    // Запускаем потребителей
    std::vector<std::thread> consumers;
    for (int i = 1; i <= num_consumers; ++i) {
        consumers.emplace_back(consumer, i);
    }

    // Ожидаем завершения всех потоков
    prod.join();
    for (auto& c : consumers) {
        c.join();
    }

    std::cout << "\nВсе задачи обработаны. Основной поток завершает работу." << std::endl;

    // Демонстрация recursive_mutex
    demonstrate_recursive_mutex();

    return 0;
}