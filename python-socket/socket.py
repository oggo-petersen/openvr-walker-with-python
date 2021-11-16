# import serial
import time
import socket

from keypress import PressKey, ReleaseKey, A_KEY

if __name__ == "__main__":
    print('Creating socket...')
    s = socket.socket()
    port = 8080

    s.bind(('0.0.0.0', port))
    s.listen(0)
    print("Socket is listening in {port}")

    while True:
        c, add = s.accept()
        while True:
            content = c.recv(32)
            print(content)
            content = content.decode('Ascii')
            if len(content) == 0:
                break
            elif content == 'F':
                print("Not walking...")
                ReleaseKey(A_KEY)
            else:
                print("Walking...")
                PressKey(A_KEY)
            c.close()
            break

    # For serial port communication.
    # serialPort = serial.Serial(port = "COM3", baudrate=9600,
    #                        bytesize=8, timeout=2, stopbits=serial.STOPBITS_ONE)
    # serialString = ""
    # while(True):
    #     serialString = serialPort.read(1)
    #     if serialString.decode('Ascii') == 'F':
    #         ReleaseKey(A_KEY)
    #     else:
    #         PressKey(A_KEY)