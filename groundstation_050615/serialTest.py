import serial
import binascii

ser = serial.Serial('/dev/ttyACM2',115200)
while True:
    try:
        data = ser.readline()
        print data
    except:
        continue

ser.close()
