//#include <opencv2/opencv.hpp>
//#include <iostream>
//
//using std::cout;
//using std::endl;
//using std::vector;
//
//using namespace cv;
//
//int main() {
//
//    setlocale(LC_ALL, "RU");
//
//    // Размер холста: 800x600
//    int width = 800;
//    int height = 600;
//
//    //Холст с линией
//     Mat canvas_line(height, width, CV_8UC3,  Scalar(0, 0, 0));
//     line(canvas_line,  Point(0, 0),  Point(799, 599),  Scalar(255, 0, 0), 3);
//     putText(canvas_line, "Trifonov I.V. Group 22-PM",  Point(20, height - 20),
//         FONT_HERSHEY_SIMPLEX, 1.0,  Scalar(255, 255, 255), 2);
//     imshow("Line", canvas_line);
//     imwrite("canvas_line.jpg", canvas_line);
//
//    // Холст с прямоугольником 
//     Mat canvas_rect(height, width, CV_8UC3,  Scalar(0, 0, 0));
//     rectangle(canvas_rect,  Point(100, 100),  Point(300, 250),  Scalar(0, 255, 0), 2);
//     putText(canvas_rect, "Trifonov I.V. Group 22-PM",  Point(20, height - 20),
//         FONT_HERSHEY_SIMPLEX, 1.0,  Scalar(255, 255, 255), 2);
//     imshow("Rectangle", canvas_rect);
//     imwrite("canvas_rect.jpg", canvas_rect);
//
//    //Холст с кругом 
//     Mat canvas_circle(height, width, CV_8UC3,  Scalar(0, 0, 0));
//     circle(canvas_circle,  Point(400, 300), 100,  Scalar(0, 0, 255), -1);
//     putText(canvas_circle, "Trifonov I.V. Group 22-PM",  Point(20, height - 20),
//         FONT_HERSHEY_SIMPLEX, 1.0, Scalar(255, 255, 255), 2);
//    imshow("Circle", canvas_circle);
//    imwrite("canvas_circle.jpg", canvas_circle);
//
//    //  Холст с эллипсом
//    Mat canvas_ellipse(height, width, CV_8UC3, Scalar(0, 0, 0));
//    ellipse(canvas_ellipse, Point(400, 300), Size(150, 70), 30, 0, 360, Scalar(0, 255, 255), 2);
//    putText(canvas_ellipse, "Trifonov I.V. Group 22-PM", Point(20, height - 20),
//        FONT_HERSHEY_SIMPLEX, 1.0, Scalar(255, 255, 255), 2);
//    imshow("Ellipse", canvas_ellipse);
//    imwrite("canvas_ellipse.jpg", canvas_ellipse);
//
//    //Холст с многоугольником 5 вершин
//    Mat canvas_polygon(height, width, CV_8UC3, Scalar(0, 0, 0));
//    vector<Point> pts = {
//        Point(400, 150),
//        Point(550, 250),
//        Point(500, 400),
//        Point(300, 400),
//        Point(250, 250)
//    };
//    polylines(canvas_polygon, pts, true, Scalar(255, 255, 0), 2);
//    putText(canvas_polygon, "Trifonov I.V. Group 22-PM", Point(20, height - 20),
//        FONT_HERSHEY_SIMPLEX, 1.0, Scalar(255, 255, 255), 2);
//    imshow("Polygon", canvas_polygon);
//    imwrite("canvas_polygon.jpg", canvas_polygon);
//
//    cout << "Все холсты сохранены в файлы canvas_*.jpg" << endl;
//    cout << "Нажмите любую клавишу в любом окне для выхода..." << endl;
//    waitKey(0);
//    destroyAllWindows();
//
//    return 0;
//}