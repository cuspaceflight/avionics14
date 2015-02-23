import sys
import struct
import random

##Generates random data in telemetry format

f = open("testData.txt", "wb")

data = []

for i in xrange(5000):
    time = "abcd"

    a = struct.pack("B",0)
    b = struct.pack("B",0)
    mode = struct.pack("B",9)
    channel = struct.pack("B", 0x50)
    dt = struct.pack("f", 1.23456)
    height = struct.pack("f", random.random() * 1000)
    s = time + a + b + mode + channel + dt + height
    data.append(s)

for i in xrange(5000):
    time = "abcd"
    a = struct.pack("B",0)
    b = struct.pack("B", 0)
    mode = struct.pack("B",9)
    channel = struct.pack("B", 0x51)
    vel = struct.pack("f", random.random() * 100)
    acc = struct.pack("f", float(i)/100.0)
    s = time + a + b + mode + channel + vel + acc 
    data.append(s)

random.shuffle(data)
for i in data:
    f.write(i)

f.close()
#f = open("testData.txt", "rb")
#packet = f.read(16)


#print struct.unpack('f', packet[12:])
def parse(s):
    timestamp = struct.unpack("i", packet[:4])
    a, b, mode, channel = struct.unpack("BBBB", packet[4:8])

    print a, b, mode, channel
    if mode == 9:
        data = struct.unpack("ff", packet[8:])


    if channel == 0x50: ##dt, height
        print data[0]
        print data[1]
    elif channel == 0x51: ##velocity, acceleration
        print data[0]
        print data[1]

#parse(packet)
#f.close()


