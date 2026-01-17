#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>

const bool TESTING = true;


int detectDiceVal() {
    // ---------- CAPTURE IMAGE ----------
    cv::Mat orig_image = cv::imread("dice_original.jpg");

    if (orig_image.empty()) {
        std::cerr << "Error: Failed to load captured image" << std::endl;
        return -1;
    }

    if (TESTING) {
        std::cout << "Saved original image: dice_original.jpg" << std::endl;
    }

    cv::Mat thresh;
    thresh = cv::imread("dice_original.jpg", cv::IMREAD_GRAYSCALE);

    // ---------- EDIT IMAGE ----------
    // Apply Gaussian blur to reduce noise
    cv::GaussianBlur(thresh, thresh, cv::Size(3, 3), 0);

    // Apply threshold (third is threshold value, adjust as needed, lower for more black)
    cv::threshold(thresh, thresh, 20, 255, cv::THRESH_BINARY_INV);

    if (TESTING) {
        cv::imwrite("dice_threshold.jpg", thresh);
        std::cout << "Saved processed image: dice_threshold.jpg" << std::endl;
    }

    // ---------- PROCESS IMAGE ----------
    // Set up SimpleBlobDetector parameters
    cv::SimpleBlobDetector::Params params;

    // Filter by area
    params.filterByArea = true;
    params.minArea = 100;
    params.maxArea = 100000;
    // Filter by circularity
    params.filterByCircularity = true;
    params.minCircularity = 0.6;
    // Filter by color
    params.filterByColor = true;
    params.blobColor = 255; // white blobs
    // Turn off convexity, intertia filters
    params.filterByConvexity = false;
    params.filterByInertia = false;

    // Create detector
    cv::Ptr<cv::SimpleBlobDetector> detector = cv::SimpleBlobDetector::create(params);

    // Detect blobs
    std::vector<cv::KeyPoint> keypoints;
    detector->detect(thresh, keypoints);

    int numDots = keypoints.size();
    int diceVal;

    // Handle errors with closest guess
    if (numDots < 1) {
        diceVal = 1;
    }
    else if (numDots > 6) {
        diceVal = 6;
    }
    else {
        diceVal = std::abs(7 - numDots);
    }

    if (TESTING) {
        cv::Mat frameWithDots = orig_image.clone();
        // Draw circles around detected dots
        for (const auto& kp : keypoints) {
            int x = static_cast<int>(kp.pt.x);
            int y = static_cast<int>(kp.pt.y);
            int radius = static_cast<int>(kp.size / 2);
            cv::circle(frameWithDots, cv::Point(x, y), radius, cv::Scalar(0, 255, 0), 5);
        }
        cv::imwrite("dice_detected.jpg", frameWithDots);
        std::cout << "Saved detection image: dice_detected.jpg" << std::endl;
        std::cout << "\nDots detected: " << numDots << std::endl;
        if (numDots < 1 || numDots > 6) {
            std::cout << "ERROR: detected invalid dice value" << std::endl;
        }
        std::cout << "\nDice value detected: " << diceVal << std::endl;
    }    

    return diceVal;
}

int main() {
    int result = detectDiceVal();

    if (result == -1) {
        std::cerr << "Failed to detect dice value" << std::endl;
        return 1;
    }

    return 0;
}