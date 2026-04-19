#include <iostream>
#include <vector>
#include <complex>
#include <chrono>
#include <sstream>
#include <opencv2/opencv.hpp>
#include <omp.h>

const int WIDTH = 1000;
const int HEIGHT = 1000;
const int MAX_ITER = 256;
const double X_MIN = -2.5;
const double X_MAX = 1.0;
const double Y_MIN = -1.5;
const double Y_MAX = 1.5;

int mandelbrot_iterations(double cx, double cy) {
    std::complex<double> z(0.0, 0.0);
    std::complex<double> c(cx, cy);
    int iter = 0;
    while (std::norm(z) <= 4.0 && iter < MAX_ITER) {
        z = z * z + c;
        ++iter;
    }
    return iter;
}

cv::Vec3b get_color(int iter) {
    if (iter == MAX_ITER) return cv::Vec3b(0, 0, 0);
    double hue = 360.0 * iter / MAX_ITER;
    double h = hue / 60.0;
    int sector = static_cast<int>(h);
    double f = h - sector;
    double p = 0.0;
    double q = 1.0 - f;
    double t = f;

    double r, g, b;
    switch (sector) {
    case 0: r = 1.0; g = t;   b = p; break;
    case 1: r = q;   g = 1.0; b = p; break;
    case 2: r = p;   g = 1.0; b = t; break;
    case 3: r = p;   g = q;   b = 1.0; break;
    case 4: r = t;   g = p;   b = 1.0; break;
    default: r = 1.0; g = p;   b = q; break;
    }
    return cv::Vec3b(static_cast<uchar>(b * 255),
        static_cast<uchar>(g * 255),
        static_cast<uchar>(r * 255));
}

double compute_mandelbrot(int num_threads, cv::Mat* output_img = nullptr) {

    omp_set_num_threads(num_threads);

    #pragma omp parallel
    {
    #pragma omp single
        std::cout << "Используется потоков: " << omp_get_num_threads() << std::endl;
    }

    cv::Mat img(HEIGHT, WIDTH, CV_8UC3);
    double step_x = (X_MAX - X_MIN) / WIDTH;
    double step_y = (Y_MAX - Y_MIN) / HEIGHT;

    auto start = std::chrono::high_resolution_clock::now();

#pragma omp parallel for collapse(2) schedule(dynamic)
    for (int y = 0; y < HEIGHT; ++y) {
        double cy = Y_MAX - y * step_y;
        for (int x = 0; x < WIDTH; ++x) {
            double cx = X_MIN + x * step_x;
            int iter = mandelbrot_iterations(cx, cy);
            img.at<cv::Vec3b>(y, x) = get_color(iter);
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    if (output_img) {
        *output_img = std::move(img);
    }

    return elapsed.count();
}

int main() {
    setlocale(LC_ALL, "RU");

    std::cout << "Введите количество потоков для тестирования: ";
    std::string line;
    std::getline(std::cin, line);
    std::istringstream iss(line);
    std::vector<int> threads_list;
    int n;
    while (iss >> n) {
        threads_list.push_back(n);
    }

    if (threads_list.empty()) {
        std::cerr << "Не задано ни одного значения потоков. Завершение.\n";
        return 1;
    }


    std::cout << "\nПоследовательное вычисление (1 поток)...\n";
    double seq_time = compute_mandelbrot(1);
    std::cout << "Время: " << seq_time << " сек\n";

    std::vector<double> times;
    cv::Mat final_image;

    for (size_t i = 0; i < threads_list.size(); ++i) {
        int threads = threads_list[i];
        std::cout << "\nВычисление с " << threads << " потоками...\n";
        double t;
        if (i == threads_list.size() - 1) {
            t = compute_mandelbrot(threads, &final_image);
        }
        else {
            t = compute_mandelbrot(threads);
        }
        times.push_back(t);
        std::cout << "Время: " << t << " сек\n";
    }

    std::cout << "\n=== Сравнительный анализ ===\n";
    std::cout << "Потоков\tВремя (сек)\tУскорение\n";
    for (size_t i = 0; i < threads_list.size(); ++i) {
        double speedup = seq_time / times[i];
        std::cout << threads_list[i] << "\t" << times[i] << "\t\t" << speedup << "\n";
    }

    if (!final_image.empty()) {
        cv::imshow("Mandelbrot Set", final_image);
        cv::waitKey(0);
    }

    return 0;
}