//#include <opencv2/opencv.hpp>
//#include <iostream>
//
//
//using std::string;
//
//
//using namespace cv;
//
//int main(int argc, char** argv) {
//
//    setlocale(LC_ALL, "RU");
//
//    string imagePath = "C:/Users/MISTERIGORTRIF/Downloads/example.jpg";
//    Mat img = imread(imagePath);
//    
//
//    //Переводим в цветовое пространство HSV
//    Mat hsv;
//    cvtColor(img, hsv, COLOR_BGR2HSV);
//
//    Mat mask1, mask2, mask;
//
//    
//    //Красный
//
//    //inRange(hsv, Scalar(0, 100, 100), Scalar(10, 255, 255), mask1);
//    //inRange(hsv, Scalar(170, 100, 100), Scalar(180, 255, 255), mask2);
//
//    // Зеленый
//
//    inRange(hsv, Scalar(40, 100, 100), Scalar(80, 255, 255), mask);
//
//    // Объединяем маски (Для красного маркера)
//    //mask = mask1 | mask2;
//
//
//    // Отображаем
//    imshow("Original", img);
//    imshow("Red Marker Mask", mask);
//
//    //Сохраняем бинарную маску
//    imwrite("mask_red.png", mask);
//
//    waitKey(0);
//    destroyAllWindows();
//
//    return 0;
//}