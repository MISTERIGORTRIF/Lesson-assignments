#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;

int main() {
    
    std::string imagePath = "C:/Users/MISTERIGORTRIF/Downloads/test.jpg";

    setlocale(LC_ALL, "ru");

    // Загружаем изображение
    Mat src = cv::imread(imagePath);

    //исходное изображение
    imshow("Original", src);
    waitKey(300); // небольшая пауза, чтобы окно появилось

    //градации серого
    Mat gray;
    cvtColor(src, gray, cv::COLOR_BGR2GRAY);
    imshow("Grayscale", gray);
    waitKey(300);

    // уменьшаем в 2 раза
    Mat resized;
    resize(src, resized, cv::Size(), 0.5, 0.5);
    imshow("Resized", resized);
    waitKey(300);

    // Поворот на 90 
    Mat rotated;
    rotate(src, rotated, cv::ROTATE_90_CLOCKWISE);
    imshow("Rotated 90° clockwise", rotated);
    waitKey(300);

    //Размытие по Гауссу
    Mat blurred;
    GaussianBlur(src, blurred, cv::Size(5, 5), 1.5);
    imshow("Gaussian Blur", blurred);
    waitKey(300);

    //Выделение границ оператором Canny
    Mat edges;
    Canny(gray, edges, 50, 150);
    imshow("Canny Edges", edges);
    waitKey(300);

    // Сохраняем два обработанных изображения на диск
    imwrite("grayscale.jpg", gray);
    imwrite("canny_edges.jpg", edges);
    std::cout << "Изображения сохранены: grayscale.jpg, canny_edges.jpg" << std::endl;

    // Ожидаем нажатия любой клавиши для завершения
    std::cout << "..." << std::endl;
    waitKey(0);
    destroyAllWindows();

    return 0;
}