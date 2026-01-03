# Take one picture and output the dice value

import cv2
import numpy as np
from picamera2 import Picamera2
import time


# Testing variable to perform extra operations for debugging
# Set to False if in production for speed
TESTING = True


""" Do this when enter dice detection"""
def init_camera():
    # Initialize camera
    camera = Picamera2()
    config = camera.create_preview_configuration(main={"size": (1920, 1080)})
    camera.configure(config)
    camera.start()
    # Let camera warm up
    time.sleep(2)

    if TESTING: print("Camera initialized")

    return camera


def capture_image(camera):
    image = camera.capture_array()
    # Stop camera
    camera.stop()
    # Convert to BGR for OpenCV (picamera2 gives RGB)
    image = cv2.cvtColor(image, cv2.COLOR_RGB2BGR)

    if TESTING:
        # Save the original image
        cv2.imwrite("dice_original.jpg", image)
        print("Saved original capture: dice_original.jpg")

    return image


def process_image(image):
    # Convert to grayscale
    gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    # Apply Gaussian blur to reduce noise
    blurred = cv2.GaussianBlur(gray, (3, 3), 0)
    # Apply adaptive threshold to get binary image. Edit the first number to adjust, higher=more white
    _, thresh = cv2.threshold(blurred, 80, 255, cv2.THRESH_BINARY_INV)

    if TESTING:
        # Save threshold image for debugging
        cv2.imwrite("dice_threshold.jpg", thresh)
        print("Saved processed image: dice_threshold.jpg")

    return thresh


def detect_dots(thresh, original_image):
    # Set up blob detector to find circular dots
    params = cv2.SimpleBlobDetector_Params()

    # Filters
    params.filterByArea = True  # Area of dot
    params.minArea = 100
    params.maxArea = 10000

    params.filterByCircularity = True
    params.minCircularity = 0.6  # reduce for blurry/ warped dots

    params.filterByColor = True
    params.blobColor = 255  # white blobs

    # Create detector
    detector = cv2.SimpleBlobDetector_create(params)

    # Detect blobs (dots on dice)
    keypoints = detector.detect(thresh)

    # Count the number of dots detected
    num_dots = len(keypoints)
    # Convert to |7-val|
    dice_val = abs(7-num_dots)

    if TESTING:
        frame_with_dots = original_image.copy()
        # Draw circles around detected dots
        for kp in keypoints:
            x, y = int(kp.pt[0]), int(kp.pt[1])
            radius = int(kp.size / 2)
            print(f"circle at ({x}, {y}) with radius {radius}")
            cv2.circle(frame_with_dots, (x, y), radius, (0, 255, 0), 5)
        cv2.putText(frame_with_dots, f"Dots Detected: {num_dots}", (10, 50), cv2.FONT_HERSHEY_SIMPLEX, 2, (0, 255, 0), 5)
        # Save result image
        cv2.imwrite("dice_detected.jpg", frame_with_dots)
        print("Saved detection image: dice_detected.jpg")
        print(f"\nDice value detected: {dice_val}")

        if num_dots < 1 or num_dots > 6:
            print("ERROR: detected invalid dice value")

    # handle errors with closest guess
    if num_dots < 0:
        return 1
    elif num_dots > 6:
        return 6

    return dice_val


def detect_dice_val():
    # Dice value detection sub-state
    camera = init_camera()

    orig_image = capture_image(camera)
    processed_image = process_image(orig_image)
    num_dots = detect_dots(processed_image, orig_image)


if __name__=="__main__":
    detect_dice_val()
