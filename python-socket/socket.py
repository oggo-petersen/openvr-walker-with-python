# import serial
import time
import socket

from keypress import PressKey, ReleaseKey, W_KEY, R_KEY, S_KEY, A_KEY, D_KEY
from threading import Thread

# Is moving consts
STOPPED = 0
MOVING = 1

# Walk speed - for now we always run.
WALK_SPEED = 0
RUN_SPEED = 1

# Direction of the movement - for now, only forward.
MOV_CENTER = 0
MOV_BACKWARD = 2
MOV_FORWARD = 1

# If we are moving sideways - not used for now.
CENTER = 0
LEFT = 1
RIGHT = 2

def check_direction(direction):
    if direction == MOV_FORWARD:
        PressKey(W_KEY)
        PressKey(R_KEY)
    elif direction == MOV_BACKWARD:
        PressKey(S_KEY)
        PressKey(R_KEY)
    else:
        ReleaseKey(W_KEY)
        ReleaseKey(S_KEY)
        ReleaseKey(R_KEY)


def check_tilt(tilt):
    if tilt != CENTER:
        if tilt == LEFT:
            PressKey(A_KEY)
        elif tilt == RIGHT:
            PressKey(D_KEY)
    else:
        ReleaseKey(A_KEY)
        ReleaseKey(D_KEY)


def should_run(speed):
    if speed == RUN_SPEED:
        PressKey(R_KEY)
    else:
        ReleaseKey(R_KEY)


def stop_movement():
    ReleaseKey(A_KEY)
    ReleaseKey(D_KEY)
    ReleaseKey(W_KEY)
    ReleaseKey(S_KEY)


if __name__ == "__main__":
    print('Creating socket...')
    s = socket.socket()
    port = 8080

    s.bind(('0.0.0.0', port))
    s.listen(0)
    print(f"Socket is listening in {port}")

    while True:
        print('waiting for data...')
        c, add = s.accept()
        while True:
            content = c.recv(32)
            content = content.decode('Ascii')
            moving = int(content[0])
            speed = int(content[1])
            direction = int(content[2])
            tilt = int(content[3])
            print(content)

            if moving == MOVING:
                should_run(speed)
                check_tilt(tilt)
                check_direction(direction)
            else:
                stop_movement()

            c.close()
            break

    # For serial port communication.
    # serialPort = serial.Serial(port = "COM3", baudrate=9600,
    #                        bytesize=8, timeout=2, stopbits=serial.STOPBITS_ONE)
    # serialString = ""
    # 
    # while(True):
    #     serialString = serialPort.read(1)
    #     if serialString.decode('Ascii') == 'F':
    #         ReleaseKey(A_KEY)
    #     else:
    #         PressKey(A_KEY)