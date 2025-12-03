# Take one picture and output the dice value

import cv2
import numpy as np
from picamera2 import Picamera2
import time


def init_camera():
    # Initialize camera
    camera = Picamera2()
    config = camera.create_preview_configuration(main={"size": (640, 480)})
    camera.configure(config)
    camera.start()
    # Let camera warm up
    time.sleep(2)
    print("Camera initialized")

    return camera


def capture_image(camera):
    print("Capturing image...")
    image = camera.capture_array()
    # Stop camera
    camera.stop()
    # Convert to BGR for OpenCV (picamera2 gives RGB)
    image = cv2.cvtColor(image, cv2.COLOR_RGB2BGR)
    # Save the original image
    cv2.imwrite("dice_original.jpg", image)
    print("Saved original capture: dice_original.jpg")
    
    return image


def blur_image(image):
    # Convert to grayscale
    gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    # Apply Gaussian blur to reduce noise
    blurred = cv2.GaussianBlur(gray, (1, 1), 0)
    # Apply adaptive threshold to get binary image
    thresh = cv2.adaptiveThreshold(blurred, 255, cv2.ADAPTIVE_THRESH_GAUSSIAN_C, cv2.THRESH_BINARY_INV,51, 10)
    # Save threshold image for debugging
    cv2.imwrite("dice_threshold.jpg", thresh)
    print("Saved processed image: dice_threshold.jpg")

    return thresh


def detect_dice_val(thresh):
    # Set up blob detector to find circular dots
    params = cv2.SimpleBlobDetector_Params()

    # Filter by area (size of dots)
    params.filterByArea = True
    params.minArea = 20
    params.maxArea = 500

    # Filter by circularity
    params.filterByCircularity = True
    params.minCircularity = 0.7

    # Filter by convexity
    params.filterByConvexity = True
    params.minConvexity = 0.8

    # Filter by inertia (roundness)
    params.filterByInertia = True
    params.minInertiaRatio = 0.6

    # Create detector
    detector = cv2.SimpleBlobDetector_create(params)

    # Detect blobs (dots on dice)
    keypoints = detector.detect(thresh)

    # Count the number of dots detected
    num_dots = len(keypoints)

    # Draw circles around detected dots
    frame_with_dots = frame.copy()
    for kp in keypoints:
        x, y = int(kp.pt[0]), int(kp.pt[1])
        radius = int(kp.size / 2)
        cv2.circle(frame_with_dots, (x, y), radius, (0, 255, 0), 2)

    # Draw the dice value on the image
    cv2.putText(frame_with_dots, f"Dice Value: {num_dots}", 
                (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 255, 0), 2)

    # Save result image
    cv2.imwrite("dice_detected.jpg", frame_with_dots)
    print("Saved detection image: dice_detected.jpg")

    return num_dots


if __name__=="__main__":
    camera = init_camera()
    image = capture_image(camera)
    thresh = blur_image(image)
    num_dots = detect_dice_val(thresh)
    print(f"\nDice value detected: {num_dots}")
