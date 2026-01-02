# Take one picture and output the dice value

import cv2
import numpy as np
from picamera2 import Picamera2
import time


def init_camera():
    # Initialize camera
    camera = Picamera2()
    config = camera.create_preview_configuration(main={"size": (1920, 1080)})
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
    blurred = cv2.GaussianBlur(gray, (3, 3), 0)
    # Apply adaptive threshold to get binary image
    _, thresh = cv2.threshold(blurred, 55, 255, cv2.THRESH_BINARY_INV)
    # Save threshold image for debugging
    cv2.imwrite("dice_threshold.jpg", thresh)
    print("Saved processed image: dice_threshold.jpg")

    return thresh


def detect_dice_val(thresh, original_image):
    # Set up blob detector to find circular dots
    params = cv2.SimpleBlobDetector_Params()
    
    # Filter by area (MUCH LARGER for close-up dice)
    params.filterByArea = True
    params.minArea = 100      # Increased from 20
    params.maxArea = 10000    # Increased from 1000
    
    # Filter by circularity (RELAXED for blurry dots)
    params.filterByCircularity = True
    params.minCircularity = 0.3  # Reduced from 0.7
    
    # Filter by convexity (RELAXED)
    params.filterByConvexity = True
    params.minConvexity = 0.5    # Reduced from 0.8
    
    # Filter by inertia (RELAXED)
    params.filterByInertia = True
    params.minInertiaRatio = 0.3  # Reduced from 0.6

    # Filter by color
    params.filterByColor = True
    params.blobColor = 255  # white blobs
    
    # Create detector
    detector = cv2.SimpleBlobDetector_create(params)
    
    # Detect blobs (dots on dice)
    keypoints = detector.detect(thresh)
    
    # Count the number of dots detected
    num_dots = len(keypoints)
    
    # Draw on the ORIGINAL COLOR image instead
    frame_with_dots = original_image.copy()
    
    # Draw circles around detected dots
    for kp in keypoints:
        x, y = int(kp.pt[0]), int(kp.pt[1])
        radius = int(kp.size / 2)
        print(f"Drawing circle at ({x}, {y}) with radius {radius}")
        cv2.circle(frame_with_dots, (x, y), radius, (0, 255, 0), 5)
        # Draw center point
        cv2.circle(frame_with_dots, (x, y), 10, (0, 0, 255), -1)
    
    # Draw the dice value on the image
    cv2.putText(frame_with_dots, f"Dice Value: {num_dots}", 
                (10, 50), cv2.FONT_HERSHEY_SIMPLEX, 2, (0, 255, 0), 5)
    
    # Save result image
    cv2.imwrite("dice_detected.jpg", frame_with_dots)
    print("Saved detection image: dice_detected.jpg")
    
    return num_dots


if __name__=="__main__":
    camera = init_camera()
    image = capture_image(camera)
    thresh = blur_image(image)
    num_dots = detect_dice_val(thresh, image)  # Pass original image too
    print(f"\nDice value detected: {num_dots}")
