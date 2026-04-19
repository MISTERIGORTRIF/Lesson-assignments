#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <cmath>

struct FigureInfo {
    std::string name;
    double perimeter;
    cv::Point2f center;
    int corners;
};

double getAngleBetween(const cv::Point& a, const cv::Point& b, const cv::Point& c) {
    cv::Point2f ab = b - a;
    cv::Point2f bc = c - b;

    double angle1 = std::atan2(ab.y, ab.x);
    double angle2 = std::atan2(bc.y, bc.x);

    double diff = std::fabs(angle1 - angle2) * 180.0 / CV_PI;
    if (diff > 180.0) {
        diff = 360.0 - diff;
    }

    return diff;
}

std::string detectFigureType(const std::vector<cv::Point>& polygon, double areaValue, double length) {
    int pointCount = static_cast<int>(polygon.size());
    double roundness = 4.0 * CV_PI * areaValue / (length * length);

    switch (pointCount) {
    case 3:
        return "Triangle";
    case 4: {
        int nearRightAngles = 0;

        for (int i = 0; i < 4; ++i) {
            double currentAngle = getAngleBetween(
                polygon[i],
                polygon[(i + 1) % 4],
                polygon[(i + 2) % 4]
            );

            if (std::fabs(currentAngle - 90.0) < 15.0) {
                nearRightAngles++;
            }
        }

        if (nearRightAngles >= 2) {
            cv::Rect box = cv::boundingRect(polygon);
            double ratio = static_cast<double>(box.width) / box.height;

            if (std::fabs(ratio - 1.0) < 0.15) {
                return "Square";
            }
            return "Rectangle";
        }

        return "Rhombus";
    }
    case 5:
        return "Pentagon";
    case 6:
        return "Hexagon";
    default:
        if (pointCount >= 8) {
            return (roundness > 0.89) ? "Circle" : "Oval";
        }
        return "Shape";
    }
}

FigureInfo analyzeContour(const std::vector<cv::Point>& contour) {
    FigureInfo info{};

    double contourPerimeter = cv::arcLength(contour, true);
    double contourArea = cv::contourArea(contour);

    std::vector<cv::Point> simplified;
    cv::approxPolyDP(contour, simplified, 0.02 * contourPerimeter, true);

    cv::Moments moments = cv::moments(contour);
    cv::Point2f massCenter(
        static_cast<float>(moments.m10 / moments.m00),
        static_cast<float>(moments.m01 / moments.m00)
    );

    info.name = detectFigureType(simplified, contourArea, contourPerimeter);
    info.perimeter = contourPerimeter;
    info.center = massCenter;
    info.corners = static_cast<int>(simplified.size());

    return info;
}

int main() {
    setlocale(LC_ALL, "rus");

    cv::Mat source = cv::imread("shapes.jpg");
    if (source.empty()) {
        std::cout << "Не удалось загрузить изображение!" << std::endl;
        return -1;
    }

    cv::Mat output = source.clone();
    cv::Mat grayscale;
    cv::Mat borderMap;

    cv::cvtColor(source, grayscale, cv::COLOR_BGR2GRAY);
    cv::GaussianBlur(grayscale, grayscale, cv::Size(5, 5), 0);
    cv::Canny(grayscale, borderMap, 50, 150);

    std::vector<std::vector<cv::Point>> foundContours;
    cv::findContours(borderMap, foundContours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    std::cout << "\nРезультаты распознавания:\n" << std::endl;

    int index = 0;

    for (const auto& contour : foundContours) {
        if (cv::contourArea(contour) < 500) {
            continue;
        }

        index++;
        FigureInfo data = analyzeContour(contour);

        cv::drawContours(output, std::vector<std::vector<cv::Point>>{contour}, -1, cv::Scalar(0, 255, 0), 2);
        cv::circle(output, data.center, 5, cv::Scalar(0, 0, 255), -1);

        std::string text = data.name + " P: " + std::to_string(static_cast<int>(data.perimeter));
        cv::putText(
            output,
            text,
            cv::Point(static_cast<int>(data.center.x) - 30, static_cast<int>(data.center.y) - 10),
            cv::FONT_HERSHEY_SIMPLEX,
            0.4,
            cv::Scalar(0, 0, 0),
            1
        );

        std::cout << "Фигура " << index << ":" << std::endl;
        std::cout << "  Название: " << data.name << std::endl;
        std::cout << "  Периметр: " << static_cast<int>(data.perimeter) << std::endl;
        std::cout << "  Центр масс: (" << data.center.x << ", " << data.center.y << ")" << std::endl;
        std::cout << "  Количество углов: " << data.corners << std::endl;
        std::cout << "------------------------" << std::endl;
    }

    cv::imshow("Canny", borderMap);
    cv::imshow("Detected Shapes", output);

    cv::imwrite("result_with_labels.jpg", output);
    std::cout << "\nРезультат сохранен в 'result_with_labels.jpg'" << std::endl;

    cv::waitKey(0);
    return 0;
}