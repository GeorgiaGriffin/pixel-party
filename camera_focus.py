import cv2
from picamera2 import Picamera2
import time

# Initialize camera
picam2 = Picamera2()

# Create configuration
config = picam2.create_preview_configuration(main={"size": (640, 480)})
picam2.configure(config)

# Set autofocus mode to manual so we can control it
# Note: Camera Module 3 supports autofocus
picam2.set_controls({
    "AfMode": 0,  # 0 = Manual focus, 2 = Continuous autofocus
    "LensPosition": 5.0  # Starting focus position (0.0 to 12.0, lower = closer)
})

picam2.start()
time.sleep(2)

print("\n=== Camera Setup Tool ===")
print("\nControls:")
print("  'f' / 'F' - Focus closer / further")
print("  'a' - Toggle Autofocus on/off")
print("  's' - Save current settings and image")
print("  'q' - Quit")
print("\nAdjust until dice is clear, then press 's' to save settings.\n")

# Starting values
focus_position = 5.0
autofocus_enabled = False

try:
    while True:
        # Capture frame
        frame = picam2.capture_array()
        
        # Convert RGB to BGR for OpenCV display
        frame_bgr = cv2.cvtColor(frame, cv2.COLOR_RGB2BGR)
        
        # Display current settings on frame
        text = f"Focus: {focus_position:.1f} | AutoFocus: {'ON' if autofocus_enabled else 'OFF'}"
        cv2.putText(frame_bgr, text, (10, 30), 
                    cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 255, 0), 2)
        
        cv2.imshow("Camera Setup - Adjust Focus", frame_bgr)
        
        # Handle key presses
        key = cv2.waitKey(100) & 0xFF
        
        if key == ord('q'):
            print("\nExiting without saving...")
            break
            
        elif key == ord('f'):
            # Focus closer (decrease value)
            focus_position = max(0.0, focus_position - 0.5)
            picam2.set_controls({"LensPosition": focus_position})
            print(f"Focus: {focus_position:.1f} (closer)")
            
        elif key == ord('F'):
            # Focus further (increase value)
            focus_position = min(12.0, focus_position + 0.5)
            picam2.set_controls({"LensPosition": focus_position})
            print(f"Focus: {focus_position:.1f} (further)")
            
        elif key == ord('a'):
            # Toggle autofocus
            autofocus_enabled = not autofocus_enabled
            if autofocus_enabled:
                picam2.set_controls({"AfMode": 2})  # Continuous autofocus
                print("Autofocus: ON")
            else:
                picam2.set_controls({"AfMode": 0})  # Manual
                print("Autofocus: OFF")
                
        elif key == ord('s'):
            # Save settings and test image
            print("\n=== Saving Settings ===")
            print(f"Focus Position: {focus_position}")
            print(f"Autofocus: {'Enabled' if autofocus_enabled else 'Disabled'}")
            q
            # Save test image
            test_frame = cv2.cvtColor(frame, cv2.COLOR_RGB2BGR)
            cv2.imwrite("camera_test.jpg", test_frame)
            print("Test image saved as: camera_test.jpg")
            
            # Save settings to file
            with open("camera_settings.txt", "w") as f:
                f.write(f"focus_position={focus_position}\n")
                f.write(f"autofocus_enabled={autofocus_enabled}\n")
            print("Settings saved to: camera_settings.txt")
            
            print("\nYou can now run read_dice.py")
            print("Press 'q' to quit or continue adjusting...")

finally:
    picam2.stop()
    cv2.destroyAllWindows()
    print("\nCamera stopped.")