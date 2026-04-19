#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <algorithm>
#include <clocale>

struct Detectors {
    cv::CascadeClassifier face;
    cv::CascadeClassifier eyes;
    cv::CascadeClassifier smile;
};

bool loadAll(Detectors& d) {
    std::string base = "C:\\Users\\MISTERIGORTRIF\\source\\repos\\OpenCV\\haarcascades\\";

    if (!d.face.load(base + "haarcascade_frontalface_default.xml")) {
        std::cout << "Ошибка загрузки лица\n";
        return false;
    }

    if (!d.eyes.load(base + "haarcascade_eye_tree_eyeglasses.xml")) {
        if (!d.eyes.load(base + "haarcascade_eye.xml")) {
            std::cout << "Ошибка загрузки глаз\n";
            return false;
        }
    }

    if (!d.smile.load(base + "haarcascade_smile.xml")) {
        std::cout << "Ошибка загрузки улыбки\n";
        return false;
    }

    return true;
}

std::vector<cv::Rect> pickEyes(const std::vector<cv::Rect>& all, const cv::Rect& face) {
    std::vector<cv::Rect> result;
    int midY = face.height / 2;
    int midX = face.width / 2;

    for (const auto& e : all) {
        if ((e.y + e.height < midY) &&
            (e.width < face.width / 2 && e.height < face.height / 3)) {
            result.push_back(e);
        }
    }

    if (result.size() > 2) {
        std::sort(result.begin(), result.end(),
            [midX](const cv::Rect& a, const cv::Rect& b) {
                int ca = a.x + a.width / 2;
                int cb = b.x + b.width / 2;
                return std::abs(ca - midX) < std::abs(cb - midX);
            });
        result.resize(2);
    }

    return result;
}

void drawEyes(cv::Mat& img, const std::vector<cv::Rect>& eyes, const cv::Rect& face) {
    for (const auto& e : eyes) {
        cv::Point c(face.x + e.x + e.width / 2,
            face.y + e.y + e.height / 2);

        int r = std::max(e.width, e.height) / 2;
        cv::circle(img, c, r, cv::Scalar(255, 0, 0), 2);
    }
}

void drawSmiles(cv::Mat& img, const std::vector<cv::Rect>& smiles, const cv::Rect& face) {
    for (const auto& s : smiles) {
        if (s.y + s.height > face.height / 2) {
            cv::rectangle(img,
                cv::Point(face.x + s.x, face.y + s.y),
                cv::Point(face.x + s.x + s.width, face.y + s.y + s.height),
                cv::Scalar(0, 0, 255), 2);
        }
    }
}

void handleFrame(cv::Mat& frame, Detectors& d) {
    cv::Mat gray;
    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
    cv::equalizeHist(gray, gray);

    std::vector<cv::Rect> faces;
    d.face.detectMultiScale(gray, faces, 1.1, 5, 0, cv::Size(30, 30));

    for (const auto& f : faces) {
        cv::rectangle(frame, f, cv::Scalar(0, 255, 0), 2);

        cv::Mat roi = gray(f);

        std::vector<cv::Rect> eyesRaw;
        d.eyes.detectMultiScale(roi, eyesRaw, 1.1, 3, 0, cv::Size(10, 10));

        auto eyes = pickEyes(eyesRaw, f);
        drawEyes(frame, eyes, f);

        std::vector<cv::Rect> smiles;
        d.smile.detectMultiScale(roi, smiles, 1.1, 25, 0, cv::Size(25, 25));

        drawSmiles(frame, smiles, f);
    }
}

int main() {
    setlocale(LC_ALL, "rus");

    Detectors detectors;
    if (!loadAll(detectors)) {
        return -1;
    }

    cv::VideoCapture cap("ZUA.mp4");
    if (!cap.isOpened()) {
        std::cout << "Не удалось открыть видео\n";
        return -1;
    }

    double fps = cap.get(cv::CAP_PROP_FPS);
    if (fps <= 0) fps = 25;

    int w = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_WIDTH));
    int h = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_HEIGHT));

    
    cv::VideoWriter writer(
        "..\\..\\output_video.avi",
        cv::VideoWriter::fourcc('M', 'J', 'P', 'G'),
        fps,
        cv::Size(w, h)
    );

    if (!writer.isOpened()) {
        std::cout << "Ошибка записи видео\n";
        return -1;
    }

    cv::Mat frame;

    while (cap.read(frame)) {
        handleFrame(frame, detectors);

        writer.write(frame);

        cv::imshow("Detection", frame);

        if (cv::waitKey(1) == 27) break;
    }

    cap.release();
    writer.release();
    cv::destroyAllWindows();

    std::cout << "Готово. Видео сохранено в корне проекта 👍\n";
    return 0;
}