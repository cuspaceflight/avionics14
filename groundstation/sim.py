import struct
import time
import sys

def getData(f, s = 16):
    ### Read s bytes from file f
    while True:
        d = f.read(s)
        if not d:
            break
        yield d

def sleepRun(infile):
    timestamp_old = 429467295
    for packet in getData(infile):
        timestamp = struct.unpack("i", packet[:4])
        if (timestamp[0] < timestamp_old):
            timestamp_old = timestamp[0]
        dt = timestamp[0] - timestamp_old
        timestamp_old = timestamp[0]
        time.sleep(float(dt)/1000000.0)
        print packet 
        sys.stdout.flush()



def run(infile):
    timestamp_old = 429467295
    for packet in getData(infile):
        print packet
        timestamp = struct.unpack("i", packet[:4])
        if (timestamp[0] < timestamp_old):
            timestamp_old = timestamp[0]
        metadata, channel, cs1, cs2 = struct.unpack("BBBB", packet[4:8])
        origin = metadata >> 4
        mode = metadata & 15
        dt = timestamp[0] - timestamp_old

        
        timestamp_old = timestamp[0]
        time.sleep(float(dt)/1000000.0)


        #if a != 0 or b != 0:
            #print "a, b != 0", timestamp, a, b, mode, channel 

        if mode == 0:
            data = struct.unpack("8s", packet[8:])
        elif mode == 1:
            data = struct.unpack("q", packet[8:])
        elif mode == 2:
            data = struct.unpack("Q", packet[8:])
        elif mode == 3:
            data = struct.unpack("ii", packet[8:])
        elif mode == 4:
            data = struct.unpack("II", packet[8:])
        elif mode == 5:
            data = struct.unpack("hhhh", packet[8:])
        elif mode == 6:
            data = struct.unpack("HHHH", packet[8:])
        elif mode == 7:
            data = struct.unpack("bbbbbbbb", packet[8:])
        elif mode == 8:
            data = struct.unpack("BBBBBBBB", packet[8:])
        elif mode == 9:
            data = struct.unpack("ff", packet[8:])
        elif mode == 10:
            data = struct.unpack("d", packet[8:])

        if channel == 0x00:
            print "Initialisation message", data[0]
        elif channel == 0x10:
            print "Calibration - timestamp frequency", data[0]
        elif channel == 0x11:
            print "Calibration - lg_accel: axis, gravMag", data[0], data[1]
        elif channel == 0x12:
            print "Calibration - hg_accel: axis, gravMag", data[0], data[1]
        elif channel == 0x13: 
            print "Calibration - barometer: d0, c1, c2, c3", data[0], data[1], data[2], data[3]
        elif channel == 0x14:
            print "Calibration - barometer: c4, c5, c6, c7", data[0], data[1], data[2], data[3]
        elif channel == 0x20:
            print "IM - lg_accel: x, y, z", data[0], data[1], data[2]
        elif channel == 0x21:
            print "IM - hg_accel: x, y, z", data[0], data[1], data[2]
        elif channel == 0x22:
            print "IM - barom: pressure, temperature", data[0], data[1]
        elif channel == 0x23:
            print "IM - rate gyroscope", data
        elif channel == 0x24:
            print "Im - magnetometer", data
        elif channel == 0x30:
            print "External sensors - battery voltage", data[0]
        elif channel == 0x31:
            print "External sensors - strain gauges : ch1, ch2, ch3", data[0], data[1], data[2]
        elif channel == 0x32:
            print "External sensors - thermocouples: ch1, ch2, ch3", data[0], data[1], data[2]
        elif channel == 0x40:
            print "M2FC Mission Control: old_state, new_state", data[0], data[1]
        elif channel == 0x50:
            print "State estimation: pred output I: dt, height", data[0], data[1]
        elif channel == 0x51:
            print "State estimation: pred output II: vel, acc", data[0], data[1]
        elif channel == 0x52: 
            print "State estimation: pressure measurement: sensor, estimate", data[0], data[1]
        elif channel == 0x53:
            print "State estimation: acceleration measurement: sensor, estimate", data[0], data[1]
        elif channel == 0x60:
            print "Pyrotechnics: continuity results: ch1, ch2, ch3", data[0], data[1], data[2]
        elif channel == 0x61:
            print "Pyrotechnics: fired: ch1, ch2, ch3", data[0], data[1], data[2]

        



datafile = "log_00021.bin"
debug = False 
infile = open(datafile, "rb")
if (not debug):
    sleepRun(infile)
else:
    run(infile)
#if __name__=="__main__":
#    infile = open(datafile, "rb")
#
#    if (debug):
#        run(infile)
#    else:
#        sleepRun(infile)
