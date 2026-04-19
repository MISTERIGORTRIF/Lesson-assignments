#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <string>

std::string recognizeFigure(const std::vector<cv::Point>& border) {
    double length = cv::arcLength(border, true);

    std::vector<cv::Point> polygon;
    cv::approxPolyDP(border, polygon, 0.02 * length, true);

    int corners = static_cast<int>(polygon.size());

    cv::Rect bounds = cv::boundingRect(border);
    double ratio = static_cast<double>(bounds.width) / bounds.height;

    double shapeArea = cv::contourArea(border);
    double boxArea = static_cast<double>(bounds.width * bounds.height);
    double density = shapeArea / boxArea;

    if (corners == 4) {
        if (ratio >= 0.9 && ratio <= 1.1 && density > 0.75) {
            return "Square";
        }
    }

    if (corners == 3 || corners == 4) {
        if (density < 0.6) {
            return "Triangle";
        }
    }

    if (corners > 7) {
        double roundness = 4.0 * 3.1415 * shapeArea / (length * length);
        if (roundness > 0.85) {
            return "Circle";
        }
    }

    return "Unknown";
}

int main() {
    cv::VideoCapture video("video.mkv");
    if (!video.isOpened()) {
        std::cout << "Не удалось открыть видеофайл\n";
        return -1;
    }

    double frameRate = video.get(cv::CAP_PROP_FPS);
    int frameWidth = static_cast<int>(video.get(cv::CAP_PROP_FRAME_WIDTH));
    int frameHeight = static_cast<int>(video.get(cv::CAP_PROP_FRAME_HEIGHT));

    cv::VideoWriter output(
        "output_video.avi",
        cv::VideoWriter::fourcc('M', 'J', 'P', 'G'),
        frameRate,
        cv::Size(frameWidth, frameHeight)
    );

    if (!output.isOpened()) {
        std::cout << "Ошибка создания видео для записи\n";
        return -1;
    }

    cv::Mat currentFrame;
    cv::Mat grayFrame;
    cv::Mat edgeMask;
    cv::Mat contourLayer;

    while (video.read(currentFrame)) {
        cv::cvtColor(currentFrame, grayFrame, cv::COLOR_BGR2GRAY);
        cv::GaussianBlur(grayFrame, grayFrame, cv::Size(5, 5), 0);
        cv::Canny(grayFrame, edgeMask, 30, 100);
        cv::dilate(edgeMask, edgeMask, cv::Mat(), cv::Point(-1, -1), 2);

        std::vector<std::vector<cv::Point>> foundBorders;
        cv::findContours(edgeMask, foundBorders, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

        contourLayer = cv::Mat::zeros(currentFrame.size(), CV_8UC3);
        cv::drawContours(contourLayer, foundBorders, -1, cv::Scalar(0, 255, 255), 2);

        for (const auto& border : foundBorders) {
            if (cv::contourArea(border) < 500) {
                continue;
            }

            std::string figureName = recognizeFigure(border);
            if (figureName == "Unknown") {
                continue;
            }

            cv::Rect region = cv::boundingRect(border);
            cv::rectangle(currentFrame, region, cv::Scalar(0, 255, 0), 2);

            cv::Point labelPoint(
                region.x + region.width / 2 - 40,
                region.y + region.height / 2
            );

            cv::putText(
                currentFrame,
                figureName,
                labelPoint,
                cv::FONT_HERSHEY_SIMPLEX,
                0.6,
                cv::Scalar(0, 0, 255),
                2
            );
        }

        output.write(currentFrame);

        cv::imshow("Result", currentFrame);
        cv::imshow("Contours after Canny", contourLayer);

        if (cv::waitKey(30) == 27) {
            break;
        }
    }

    video.release();
    output.release();
    cv::destroyAllWindows();

    return 0;
}