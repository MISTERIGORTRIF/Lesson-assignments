#include <iostream>
#include <vector>
#include <omp.h>
#include <chrono>

double calculateSequential(const std::vector<double>& data) {
    double total = 0.0;
    for (size_t i = 0; i < data.size(); ++i) {
        total += data[i];
    }
    return total;
}

double calculateParallel(const std::vector<double>& data, int threadCount) {
    double total = 0.0;
    omp_set_num_threads(threadCount);

#pragma omp parallel
    {
#pragma omp single
        {
            std::cout << "Threads in use: " << omp_get_num_threads() << std::endl;
        }

        int threadId = omp_get_thread_num();
#pragma omp critical
        {
            std::cout << "Worker " << threadId << " started" << std::endl;
        }

#pragma omp for reduction(+:total)
        for (int i = 0; i < static_cast<int>(data.size()); ++i) {
            total += data[i];
        }
    }

    return total;
}

int main() {
    const int size = 10000000;
    const int threads = 4;

    std::vector<double> values(size);
    for (int i = 0; i < size; ++i) {
        values[i] = static_cast<double>(i);
    }

    auto seqBegin = std::chrono::high_resolution_clock::now();
    double seqSum = calculateSequential(values);
    auto seqEnd = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> seqTime = seqEnd - seqBegin;
    std::cout << "Sequential version: " << seqTime.count() << " seconds" << std::endl;
    std::cout << "Sum = " << seqSum << std::endl;

    auto parBegin = std::chrono::high_resolution_clock::now();
    double parSum = calculateParallel(values, threads);
    auto parEnd = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> parTime = parEnd - parBegin;
    std::cout << "Parallel version: " << parTime.count() << " seconds" << std::endl;
    std::cout << "Sum = " << parSum << std::endl;
    std::cout << "Speedup: " << seqTime.count() / parTime.count() << std::endl;

    return 0;
}