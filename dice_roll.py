# detect when the dice has been rolled

import cv2
from picamera2 import Picamera2
import numpy as np
import time

class DiceRollDetector:
    def __init__(self, motion_threshold=500, stillness_frames=10):
        """
        motion_threshold: minimum pixel difference to consider motion
        stillness_frames: number of consecutive still frames before considering roll complete
        """
        self.picam2 = Picamera2()
        
        # Configure for faster processing
        config = self.picam2.create_preview_configuration(
            main={"size": (1920, 1080), "format": "RGB888"}
        )
        self.picam2.configure(config)
        self.picam2.start()
        
        self.motion_threshold = motion_threshold
        self.stillness_frames = stillness_frames
        self.prev_frame = None
        
        # Warm up camera
        time.sleep(2)
    
    def detect_motion(self, frame):
        """Calculate motion between current and previous frame"""
        gray = cv2.cvtColor(frame, cv2.COLOR_RGB2GRAY)
        gray = cv2.GaussianBlur(gray, (21, 21), 0)
        
        if self.prev_frame is None:
            self.prev_frame = gray
            return 0
        
        # Calculate difference
        frame_delta = cv2.absdiff(self.prev_frame, gray)
        thresh = cv2.threshold(frame_delta, 25, 255, cv2.THRESH_BINARY)[1]
        
        # Count changed pixels
        motion_pixels = np.sum(thresh) / 255
        
        self.prev_frame = gray
        return motion_pixels
    
    def wait_for_roll(self, callback=None):
        """
        Wait for a dice roll to complete
        Returns the frame when dice has settled
        """
        print("Waiting for dice roll...")
        rolling = False
        still_count = 0
        
        while True:
            frame = self.picam2.capture_array()
            motion = self.detect_motion(frame)
            
            if motion > self.motion_threshold:
                if not rolling:
                    print("Roll detected! Waiting for dice to settle...")
                    rolling = True
                still_count = 0
            elif rolling:
                still_count += 1
                if still_count >= self.stillness_frames:
                    print("Dice has settled!")
                    if callback:
                        callback(frame)
                    return frame
            
            time.sleep(0.05)  # ~20 FPS
    
    def cleanup(self):
        self.picam2.stop()

# Example usage
if __name__ == "__main__":
    detector = DiceRollDetector(motion_threshold=500, stillness_frames=15)
    
    try:
        while True:
            # This will block until a roll is detected and complete
            settled_frame = detector.wait_for_roll()
            
            # Save the image of the settled dice
            cv2.imwrite(f"dice_roll_{int(time.time())}.jpg", 
                       cv2.cvtColor(settled_frame, cv2.COLOR_RGB2BGR))
            print("Image saved! Ready for next roll...\n")
            
    except KeyboardInterrupt:
        print("\nStopping...")
    finally:
        detector.cleanup()
