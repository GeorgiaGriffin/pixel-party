import json
from evdev import InputDevice, ecodes

DEVICE_PATH = "/dev/input/event7"   # replace with your actual event path
JSON_FILE = "player_state.json"
DEADZONE = 8000

dev = InputDevice(DEVICE_PATH)

state = {
    "playerFour": 0,
    "playerFourMove": 0,
    "playerOne": 1,
    "playerOneMove": 0,
    "playerThree": 1,
    "playerThreeMove": 0,
    "playerTwo": 1,
    "playerTwoMove": 0
}

def axis_to_move(value):
    if value < -DEADZONE:
        return -1
    elif value > DEADZONE:
        return 1
    else:
        return 0

for event in dev.read_loop():
    if event.type == ecodes.EV_ABS:
        if event.code == ecodes.ABS_X:     # first joystick horizontal
            state["playerOneMove"] = axis_to_move(event.value)

            with open(JSON_FILE, "w") as f:
                json.dump(state, f, indent=4)
