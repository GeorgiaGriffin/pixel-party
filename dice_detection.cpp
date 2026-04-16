#include <opencv2/opencv.hpp>
#include <chrono>
#include <thread>
#include <iostream>
#include <cstdlib>

#include <vector>
#include <cmath>

#include <lgpio.h>
#include <csignal>

const bool TESTING = true;

const int LED_PIN = 17;
int gpio_handle;

void initLED() {
    gpio_handle = lgGpiochipOpen(0);
    int PWM_FREQUENCY = 1000;  // Hz
    float BRIGHTNESS = 20.0;   // 0.0 to 100.0 (percent duty cycle)
    lgGpioClaimOutput(gpio_handle, 0, LED_PIN, 0);
    lgTxPwm(gpio_handle, LED_PIN, PWM_FREQUENCY, BRIGHTNESS, 0, 0);
}

void cleanupLED() {
    lgTxPwm(gpio_handle, LED_PIN, 0, 0, 0, 0);  // stop PWM
    lgGpioWrite(gpio_handle, LED_PIN, 0);
    lgGpiochipClose(gpio_handle);
}

void signalHandler(int signal) {
    cleanupLED();
    exit(0);
}

int detectDiceVal() {
    // ---------- LOAD IMAGE ----------
    cv::Mat thresh = cv::imread("dice_original.jpg", cv::IMREAD_GRAYSCALE);

    if (thresh.empty()) {
        std::cerr << "Error: Failed to load captured image" << std::endl;
        return -1;
    }

    // ---------- EDIT IMAGE ----------
    // Apply Gaussian blur to reduce noise
    cv::GaussianBlur(thresh, thresh, cv::Size(3, 3), 0);

    // Apply threshold 
    int thresh_val = 30; // adjust as needed, lower for more black
    cv::threshold(thresh, thresh, thresh_val, 255, cv::THRESH_BINARY_INV);

    if (TESTING) {
        cv::imwrite("dice_threshold.jpg", thresh);
    }

    // ---------- PROCESS IMAGE ----------
    // Set up SimpleBlobDetector parameters
    cv::SimpleBlobDetector::Params params;

    // Filter by area
    params.filterByArea = true;
    params.minArea = 100;
    params.maxArea = 10000;
    // Filter by circularity
    params.filterByCircularity = true;
    params.minCircularity = 0.5;
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
        cv::Mat orig_image = cv::imread("dice_original.jpg");
        cv::Mat frameWithDots = orig_image.clone();
        // Draw circles around detected dots
        for (const auto& kp : keypoints) {
            int x = static_cast<int>(kp.pt.x);
            int y = static_cast<int>(kp.pt.y);
            int radius = static_cast<int>(kp.size / 2);
            cv::circle(frameWithDots, cv::Point(x, y), radius, cv::Scalar(0, 255, 0), 5);
        }
        cv::imwrite("dice_detected.jpg", frameWithDots);
        std::cout << "\n  Dots detected: " << numDots << std::endl;
        if (numDots < 1 || numDots > 6) {
            std::cout << "  ERROR: detected invalid dice value" << std::endl;
        }
    }
    std::cout << "  Dice value detected: " << diceVal << std::endl;

    return diceVal;
}

class DiceRollDetector {

private:
    const int MOTION_THRESHOLD = 20;          // Pixel difference threshold
    const double MIN_MOTION_PERCENT = 0.3;    // Minimum % of frame that needs motion
    const int STABILITY_FRAMES = 5;           // Frames of stability required
    const int MAX_WAIT_FRAMES = 50;          // Maximum frames to wait before timeout. Short for debuggin ease rn
    const int MIN_ROLL_FRAMES = 1; 

    // Capture a frame using rpicam-still
    bool captureFrame(cv::Mat& frame) {
        // Set lower resolution so it goes faster
        system("rpicam-still -o /tmp/frame.jpg --width 800 --height 800 --mode 2304:1296 --timeout 100 --nopreview 2>/dev/null");
        frame = cv::imread("/tmp/frame.jpg");
        return !frame.empty();
    }

    // Calculate motion between two frames
    double calculateMotion(const cv::Mat& frame1, const cv::Mat& frame2) {
        cv::Mat gray1, gray2, diff, thresh;
        // Convert to grayscale
        cv::cvtColor(frame1, gray1, cv::COLOR_BGR2GRAY);
        cv::cvtColor(frame2, gray2, cv::COLOR_BGR2GRAY);
        // Blur to reduce noise
        cv::GaussianBlur(gray1, gray1, cv::Size(11, 11), 0);
        cv::GaussianBlur(gray2, gray2, cv::Size(11, 11), 0);
        // Find difference
        cv::absdiff(gray1, gray2, diff);
        // Threshold the difference
        cv::threshold(diff, thresh, MOTION_THRESHOLD, 255, cv::THRESH_BINARY);
        // Calculate percentage of pixels with motion
        int nonZero = cv::countNonZero(thresh);
        double motionPercent = (nonZero * 100.0) / (thresh.rows * thresh.cols);
        return motionPercent;
    }

public:
    DiceRollDetector() {
        cv::Mat testFrame;
        if (!captureFrame(testFrame)) {
            std::cerr << "ERROR: Cannot capture frames!" << std::endl;
            throw std::runtime_error("Camera failed");
        }
    }

    /**
     * Detects when a dice has been rolled and has settled.
     * 
     * State machine:
     * 1. WAITING_FOR_PICKUP: Waiting for dice to be picked up (motion starts)
     * 2. DICE_IN_MOTION: Dice is being moved/rolled (sustained motion)
     * 3. DICE_SETTLING: Dice has landed, checking for stability
     * 4. DICE_STABLE: Dice is stable and ready for detection
     * 
     * @return dice roll 1-6 if roll detected, -1 if timeout or error
     */
    int detectDiceRoll() {
        // Define states
        enum State { 
            WAITING_FOR_PICKUP,  // State 0: Waiting for initial motion
            DICE_IN_MOTION,      // State 1: Dice is moving/rolling
            DICE_SETTLING,       // State 2: Motion stopped, checking stability
            DICE_STABLE          // State 3: Stable and ready
        };
        
        State currentState = WAITING_FOR_PICKUP;
        
        cv::Mat previousFrame, currentFrame;
        int stableFrameCount = 0;
        int motionFrameCount = 0;
        int totalFrameCount = 0;

        // Capture initial frame
        std::cout << "\n=== Starting Dice Roll Detection ===" << std::endl;
        if (!captureFrame(previousFrame)) {
            std::cerr << "ERROR: Failed to capture initial frame!" << std::endl;
            return -1;
        }

        std::cout << "  Pick up the dice" << std::endl;

        // Main state machine loop
        while (totalFrameCount < MAX_WAIT_FRAMES) {
            // Capture current frame
            if (!captureFrame(currentFrame)) {
                std::cerr << "Failed to capture frame " << totalFrameCount << std::endl;
                return -1;
            }
            
            // Calculate motion
            double motionPercent = calculateMotion(previousFrame, currentFrame);
            totalFrameCount++;

            // STATE MACHINE
            switch (currentState) {
                case WAITING_FOR_PICKUP:
                    if (motionPercent > MIN_MOTION_PERCENT) {
                        motionFrameCount++;
                        if (motionFrameCount >= 3) {  // Need 3 consecutive frames of motion
                            std::cout << "  Dice picked up! Waiting for roll..." << std::endl;
                            currentState = DICE_IN_MOTION;
                            motionFrameCount = 0;
                        }
                    } else {
                        motionFrameCount = 0;  // Reset if motion stops
                    }
                    break;

                case DICE_IN_MOTION:
                    // Counting motion frames to detect a roll
                    if (motionPercent > MIN_MOTION_PERCENT) {
                        motionFrameCount++;
                        stableFrameCount = 0;  // Reset stability counter
                    } else {
                        // Motion stopped - was it a roll?
                        if (motionFrameCount >= MIN_ROLL_FRAMES) {
                            std::cout << "  Dice rolled! " << motionFrameCount << " frames of motion" << std::endl;
                            currentState = DICE_SETTLING;
                            stableFrameCount = 1;
                        } else {
                            // Not enough motion, probably just a bump
                            std::cout << "  Motion too brief (" << motionFrameCount << " frames)" << std::endl;
                            currentState = WAITING_FOR_PICKUP;
                        }
                        motionFrameCount = 0;
                    }
                    break;
                    
                case DICE_SETTLING:
                    // Checking for stability after roll
                    if (motionPercent < MIN_MOTION_PERCENT) {
                        stableFrameCount++;                        
                        
                        // Check if we have enough stable frames
                        if (stableFrameCount >= STABILITY_FRAMES) {
                            std::cout << "  Dice is stable! Ready for detection." << std::endl;
                            currentState = DICE_STABLE;
                        }
                    } else {
                        // Dice moved again!
                        std::cout << "  Dice moved again! Back to rolling state..." << std::endl;
                        currentState = DICE_IN_MOTION;
                        stableFrameCount = 0;
                        motionFrameCount = 1;
                    }
                    break;
                    
                case DICE_STABLE:
                    // Call detect dice value
                    cv::imwrite("dice_original.jpg", currentFrame);
                    int dice_val = detectDiceVal();
                    return dice_val;
            }

            // Update previous frame for next iteration
            previousFrame = currentFrame.clone();
            
            // Small delay between captures
            // If I start getting random errors it might mean that the timeout needs to be higher
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }

        // Timeout
        std::cerr << "\nTimeout: No dice roll detected within time limit" << std::endl;
        return -1;
    }
};


int runDiceDetection() {
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    initLED();

    int return_code = 0;

    try {
        DiceRollDetector detector;
        int dice_val = detector.detectDiceRoll();
        if (dice_val == -1) {
            std::cerr << "Failed to detect dice roll" << std::endl;
            return_code = 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return_code = 1;
    }

    cleanupLED();
    std::cerr << "=== End Dice Roll Detection ===\n" << std::endl;
    return return_code;
}

#ifdef DICE_TEST
int main() {
    return runDiceDetection();
}
#endif
