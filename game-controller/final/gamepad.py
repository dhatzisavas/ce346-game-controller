import vgamepad as vg
import time
from microbitinput import *
import threading
import signal
import sys

# values for scaling down 8-bit joystick values to floats in [-1,1]
# unfortunately each direction has a different midpoint
H_MID_LOW = 124
h_top_scale = 255.0 - H_MID_LOW
h_bottom_scale = float(H_MID_LOW)
V_MID_LOW = 129
v_top_scale = 255.0 - V_MID_LOW
v_bottom_scale = float(V_MID_LOW)


# converts 8-bit int to float for horizontal values
def joystickToFloatX(val):
    val -= H_MID_LOW
    if val >= 0:
        return - val / h_top_scale
    else:
        return - val / h_bottom_scale


# converts 8-bit int to float for vertical values
def joystickToFloatY(val):
    val -= V_MID_LOW
    if val >= 0:
        return val / v_top_scale
    else:
        return val / v_bottom_scale


keep_working = True

gamepad = vg.VX360Gamepad()


def work(microbit):
    while keep_working:
        vals = microbit.getSortedVals()
        gamepad.left_joystick_float(x_value_float=joystickToFloatX(vals[0]), y_value_float=joystickToFloatY(vals[1]))
        # TODO: finish this
        if (vals[2] == 1):
            gamepad.press_button(button=vg.XUSB_BUTTON.XUSB_GAMEPAD_A)
        else:
            gamepad.release_button(button=vg.XUSB_BUTTON.XUSB_GAMEPAD_A)
        if(vals[3] == 1):
            gamepad.press_button(button=vg.XUSB_BUTTON.XUSB_GAMEPAD_B)
        else:
            gamepad.release_button(button=vg.XUSB_BUTTON.XUSB_GAMEPAD_B)
        if(vals[4] == 1):
            gamepad.press_button(button=vg.XUSB_BUTTON.XUSB_GAMEPAD_X)
        else:
            gamepad.release_button(button=vg.XUSB_BUTTON.XUSB_GAMEPAD_X)
        if(vals[5] == 1):
            gamepad.press_button(button=vg.XUSB_BUTTON.XUSB_GAMEPAD_Y)
        else:
            gamepad.release_button(button=vg.XUSB_BUTTON.XUSB_GAMEPAD_Y)
        gamepad.update()
        



# allow threads to easily quit when SIGINT is received (ctrl + c)
def sigintHandler(sig, frame):
    MB.stopPolling()
    global keep_working
    keep_working = False
    read_input_thread.join()
    work_thread.join()
    sys.exit(0)


# 2 threads allows one to constantly read and process values from the microbit
# the other actually takes them and uses them with the virtual gamepad
MB = MicrobitPolling(6)
read_input_thread = threading.Thread(target=MB.readingValues)
work_thread = threading.Thread(target=work, args=(MB,))
read_input_thread.start()
work_thread.start()
signal.signal(signal.SIGINT, sigintHandler)
