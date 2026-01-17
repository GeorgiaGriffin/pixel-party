#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>

const bool TESTING = true;

cv::Mat captureImage() {
    // Use rpicam-still to capture image
    // -t 0 for immediate capture, or use small value like -t 100 for minimal auto-adjustment
    int result = system("rpicam-still -o dice_original.jpg -t 100 --width 1920 --height 1080 --immediate");

    if (result != 0) {
        std::cerr << "Error: Failed to capture image with rpicam-still" << std::endl;
        return cv::Mat();
    }

    // Load the captured image
    cv::Mat image = cv::imread("dice_original.jpg");

    if (image.empty()) {
        std::cerr << "Error: Failed to load captured image" << std::endl;
        return image;
    }

    if (TESTING) {
        std::cout << "Saved original capture: dice_original.jpg" << std::endl;
    }

    return image;
}

cv::Mat processImage(const cv::Mat& image) {
    cv::Mat gray, blurred, thresh;
    
    // Convert to grayscale
    cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
    
    // Apply Gaussian blur to reduce noise
    cv::GaussianBlur(gray, blurred, cv::Size(3, 3), 0);
    
    // Apply threshold (80 threshold value, adjust as needed)
    cv::threshold(blurred, thresh, 25, 255, cv::THRESH_BINARY_INV);
    
    if (TESTING) {
        cv::imwrite("dice_threshold.jpg", thresh);
        std::cout << "Saved processed image: dice_threshold.jpg" << std::endl;
    }
    
    return thresh;
}

int detectDots(const cv::Mat& thresh, const cv::Mat& originalImage) {
    // Set up SimpleBlobDetector parameters
    cv::SimpleBlobDetector::Params params;
    
    // Filter by area
    params.filterByArea = true;
    params.minArea = 100;
    params.maxArea = 10000;
    
    // Filter by circularity
    params.filterByCircularity = true;
    params.minCircularity = 0.2f;
    
    // Filter by color
    params.filterByColor = true;
    params.blobColor = 255; // white blobs
    
    // Create detector
    cv::Ptr<cv::SimpleBlobDetector> detector = cv::SimpleBlobDetector::create(params);
    
    // Detect blobs
    std::vector<cv::KeyPoint> keypoints;
    detector->detect(thresh, keypoints);
    
    int numDots = keypoints.size();
    int diceVal = std::abs(7 - numDots);
    
    if (TESTING) {
        cv::Mat frameWithDots = originalImage.clone();
        
        // Draw circles around detected dots
        for (const auto& kp : keypoints) {
            int x = static_cast<int>(kp.pt.x);
            int y = static_cast<int>(kp.pt.y);
            int radius = static_cast<int>(kp.size / 2);
            
            std::cout << "Circle at (" << x << ", " << y << ") with radius " << radius << std::endl;
            cv::circle(frameWithDots, cv::Point(x, y), radius, cv::Scalar(0, 255, 0), 5);
        }
        
        cv::putText(frameWithDots, "Dots Detected: " + std::to_string(numDots),
                    cv::Point(10, 50), cv::FONT_HERSHEY_SIMPLEX, 2, cv::Scalar(0, 255, 0), 5);
        
        cv::imwrite("dice_detected.jpg", frameWithDots);
        std::cout << "Saved detection image: dice_detected.jpg" << std::endl;
        std::cout << "\nDice value detected: " << diceVal << std::endl;
        
        if (numDots < 1 || numDots > 6) {
            std::cout << "ERROR: detected invalid dice value" << std::endl;
        }
    }
    
    // Handle errors with closest guess
    if (numDots < 1) return 1;
    if (numDots > 6) return 6;
    
    return diceVal;
}

int detectDiceVal() {
    cv::Mat origImage = captureImage();

    if (origImage.empty()) {
        return -1;
    }

    cv::Mat processedImage = processImage(origImage);
    int diceValue = detectDots(processedImage, origImage);

    return diceValue;
}

int main() {
    int result = detectDiceVal();

    if (result == -1) {
        std::cerr << "Failed to detect dice value" << std::endl;
        return 1;
    }

    return 0;
}