#include <opencv2/opencv.hpp>
#include <chrono>
#include <thread>
#include <iostream>
#include <cstdlib>

#include <vector>
#include <cmath>

#include <lgpio.h>

const bool TESTING = true;

const int LED_PIN = 17;
int gpio_handle;

void initLED() {
    gpio_handle = lgGpiochipOpen(0);
    lgGpioClaimOutput(gpio_handle, 0, LED_PIN, 0);
    lgGpioWrite(gpio_handle, LED_PIN, 1);
}

void cleanupLED() {
    lgGpioWrite(gpio_handle, LED_PIN, 0);
    lgGpiochipClose(gpio_handle);
    exit(0);
}

int detectDiceVal() {
    // ---------- CAPTURE IMAGE ----------
    // Use rpicam-still to capture image
    // -t 0 for immediate capture, or use small value like -t 100 for minimal auto-adjustment
    int result = system("rpicam-still -o dice_original.jpg -t 100 --width 1920 --height 1080 --immediate");
    
    if (result != 0) {
        std::cerr << "Error: Failed to capture image with rpicam-still" << std::endl;
        return -1;
    }

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
    cv::threshold(thresh, thresh, 30, 255, cv::THRESH_BINARY_INV);

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
        system("rpicam-still -o /tmp/frame.jpg --width 640 --height 480 --timeout 100 --nopreview 2>/dev/null");
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
        std::cout << "Camera works!" << std::endl;
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
     * @param capturedFrame Output parameter - the stable frame to analyze
     * @return true if roll detected and dice is stable, false if timeout or error
     */
    bool detectDiceRoll(cv::Mat& capturedFrame) {
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
            return false;
        }

        std::cout << "\n[STATE: WAITING_FOR_PICKUP]" << std::endl;
        std::cout << "Pick up the dice\n" << std::endl;

        // Main state machine loop
        while (totalFrameCount < MAX_WAIT_FRAMES) {
            // Capture current frame
            if (!captureFrame(currentFrame)) {
                std::cerr << "Failed to capture frame " << totalFrameCount << std::endl;
                return false;
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
                            std::cout << "  Dice picked up! Waiting for roll...\n" << std::endl;
                            std::cout << "\n[STATE: DICE_IN_MOTION]" << std::endl;
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
                            std::cout << "\n[STATE: DICE_SETTLING]" << std::endl;
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
                            std::cout << "\n[STATE: DICE_STABLE]" << std::endl;
                            std::cout << "  Dice is stable! Ready for detection.\n" << std::endl;
                            currentState = DICE_STABLE;
                            capturedFrame = currentFrame.clone();
                            return true;
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
                    // Should not reach here (we return in DICE_SETTLING)
                    break;
            }

            // Update previous frame for next iteration
            previousFrame = currentFrame.clone();
            
            // Small delay between captures
            // If I start getting random errors it might mean that the timeout needs to be higher
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }

        // Timeout
        std::cerr << "\nTimeout: No dice roll detected within time limit" << std::endl;
        return false;
    }
};


int main() {
    // ------- Test DiceRollDetector -------
    initLED();

    try {
        DiceRollDetector detector;
        cv::Mat diceFrame;
        
        // Wait for dice roll with full state machine
        if (detector.detectDiceRoll(diceFrame)) {
            // Detect dice val now          
            std::cout << "\n=== Dice Roll Detection Complete ===" << std::endl;
            int dice_val = detectDiceVal();
            std::cout << "\nDice detected: " << dice_val << std::endl;
        } else {
            std::cerr << "Failed to detect dice roll" << std::endl;
            cleanupLED();
            return 1;
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        
        cleanupLED();
        return 1;
    }

    // ------- Test detectDiceVal -------
    // int result = detectDiceVal();
    // if (result == -1) {
    //     std::cerr << "Failed to detect dice value" << std::endl;
    //     return 1;
    // }

    cleanupLED();
    return 0;
}
