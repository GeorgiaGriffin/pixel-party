#include <opencv2/opencv.hpp>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <cmath>

// Self-contained version — no dependency on dice_detection.cpp
int detectDiceVal(int thresh_val) {
    cv::Mat thresh = cv::imread("dice_original.jpg", cv::IMREAD_GRAYSCALE);
    if (thresh.empty()) {
        std::cerr << "Error: Failed to load image\n";
        return -1;
    }

    cv::GaussianBlur(thresh, thresh, cv::Size(3, 3), 0);
    cv::threshold(thresh, thresh, thresh_val, 255, cv::THRESH_BINARY_INV);

    cv::SimpleBlobDetector::Params params;
    params.filterByArea = true;
    params.minArea = 100;
    params.maxArea = 10000;
    params.filterByCircularity = true;
    params.minCircularity = 0.5;
    params.filterByColor = true;
    params.blobColor = 255;
    params.filterByConvexity = false;
    params.filterByInertia = false;

    auto detector = cv::SimpleBlobDetector::create(params);
    std::vector<cv::KeyPoint> keypoints;
    detector->detect(thresh, keypoints);

    int numDots = keypoints.size();
    std::cout << "  Dots detected: " << numDots << std::endl;
    return numDots;
}

void saveImages(int thresh_val) {
    cv::Mat gray = cv::imread("dice_original.jpg", cv::IMREAD_GRAYSCALE);
    cv::Mat thresh;
    cv::GaussianBlur(gray, gray, cv::Size(3, 3), 0);
    cv::threshold(gray, thresh, thresh_val, 255, cv::THRESH_BINARY_INV);
    cv::imwrite("dice_threshold.jpg", thresh);

    cv::Mat orig = cv::imread("dice_original.jpg");
    cv::Mat frameWithDots = orig.clone();

    cv::SimpleBlobDetector::Params params;
    params.filterByArea = true;
    params.minArea = 100;
    params.maxArea = 10000;
    params.filterByCircularity = true;
    params.minCircularity = 0.5;
    params.filterByColor = true;
    params.blobColor = 255;
    params.filterByConvexity = false;
    params.filterByInertia = false;

    auto detector = cv::SimpleBlobDetector::create(params);
    std::vector<cv::KeyPoint> keypoints;
    detector->detect(thresh, keypoints);

    for (const auto& kp : keypoints) {
        cv::circle(frameWithDots, cv::Point(kp.pt.x, kp.pt.y),
                   static_cast<int>(kp.size / 2), cv::Scalar(0, 255, 0), 5);
    }
    cv::imwrite("dice_detected.jpg", frameWithDots);
}


int main() {
    std::cout << "=== Dice Threshold Calibration ===\n";
    std::cout << "Place dice with 6 facing camera, then press Enter.\n";
    std::cin.get();

    system("rpicam-still -o dice_original.jpg --width 800 --height 800 --timeout 100 --nopreview 2>/dev/null");
    std::cout << "Image captured.\n\n";

    int thresh_val;

    for (thresh_val = 15; thresh_val <= 90; thresh_val += 3) {
        std::cout << "Testing threshold " << thresh_val << ": ";
        int numDots = detectDiceVal(thresh_val);

        if (numDots == 6) {
            saveImages(thresh_val);  // save only the successful threshold
            std::cout << "\nCalibrated threshold: " << thresh_val << "\n";
            std::cout << "Set thresh_val = " << thresh_val << " in dice_detection.cpp\n";
            return 0;
        }
    }

    saveImages(thresh_val);

    std::cout << "\nFailed to calibrate — no threshold produced 6 dots.\n";
    std::cout << "Check lighting or blob detector parameters.\n";
    return 1;
}