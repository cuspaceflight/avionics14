
import kivy

from kivy.app import App
from kivy.uix.screenmanager import ScreenManager, Screen
from kivy.uix.label import Label
from kivy.uix.button import Button
from kivy.uix.boxlayout import BoxLayout
from kivy.uix.scrollview import ScrollView
from kivy.uix.gridlayout import GridLayout
from kivy.uix.relativelayout import RelativeLayout
from kivy.uix.image import Image
from kivy.uix.widget import Widget
from kivy.properties import NumericProperty, ReferenceListProperty, ObjectProperty, Property, BooleanProperty, ListProperty
from kivy.clock import Clock
from kivy.garden.graph import Graph, MeshLinePlot

from random import randint
import random
import string
import math

import time
from datetime import datetime
import struct
import sys
import numpy as np
import matplotlib.pyplot as plt


######################



mission_states_map = {
    0: "Pad",
    1: "Ignition",
    2: "Powered Ascent",
    3: "Free Ascent",
    4: "Apogee",
    5: "Drogue Descent",
    6: "Main Release",
    7: "Main Descent",
    8: "Land",
    9: "Landed"
}

Rs = 8.31432
g0 = 9.80665
M = 0.0289644
Lb = [-0.0065, 0.0, 0.001, 0.0028, 0.0, -0.0028, -0.002]
Pb = [101325.0, 22632.10, 5474.89, 868.02, 110.91, 66.94, 3.96]
Tb = [288.15, 216.65, 216.65, 228.65, 270.65, 270.65, 214.65]
Hb = [0.0, 11000.0, 20000.0, 32000.0, 47000.0, 51000.0, 71000.0]


se_t = []
se_dt = []
se_h = []
se_v = []
se_a = []
baro_t = []
baro_h = []
accel_t = []
accel_a = []
mission_t = []
mission_s = []
lga_y = []
hga_y = []
pyro_fire_t = []
pyro_fire_c = []
last_timestamp = 0
t_correction = 0

def p2a(p):
    for b in range(7):
        if p <= Pb[b] and p > Pb[b+1]:
            if Lb[b] == 0.0:
                return p2a_zl(p, b)
            else:
                return p2a_nzl(p, b)
    return -9999.0

def p2a_zl(p, b):
    hb = Hb[b]
    pb = Pb[b]
    tb = Tb[b]
    return hb + (Rs * tb)/(g0 * M) * (math.log(p) - math.log(pb))

def p2a_nzl(p, b):
    lb = Lb[b]
    hb = Hb[b]
    pb = Pb[b]
    tb = Tb[b]
    return hb + tb/lb * (math.pow(p/pb, (-Rs*lb)/(g0*M)) - 1)







def getData(fileObject, chunk_size = 16):
    while True:
        data = fileObject.read(chunk_size)
        if not data:
            break
        yield data

class GroundstationApp(App):
    def build(self):
        gui = Gui()
        Clock.schedule_interval(gui.update, 10.0/60.0)
        return gui

class Gui(ScreenManager):
    dataScreen = ObjectProperty(None)
    def update(self, dt):
        self.dataScreen.update()
   

class FlightData():
    def __init__(self):
        
		self.radiocontact = False
		self.goforllaunch = False
       
		self.hasStarted = False
		self.time = None
		self.timestamp = 0
                
		self.altitude_1 = 0.0
		self.velocity_1 = 0.0
		self.acceleration_1 = 0.0
		self.gpspos_1 = (0.0, 0.0)
		self.rotation_1 = [0.0, 0.0, 0.0, 0.0]
		self.ignited_1 = False
		self.parachute_1 = False

		self.altitude_2 = 0.0
		self.velocity_2 = 0.0
		self.acceleration_2 = 0.0
		self.gpspos_2 = (0.0, 0.0)
		self.rotation_2 = [0.0, 0.0, 0.0, 0.0]
		self.ignited_2 = False
		self.parachute_2 = False

        
		self.parachute_3 = False
		
		self.last_timestamp = 0
		self.t_correction = 0
    def parse(self,packet):
		
		
		####
        
		timestamp = struct.unpack("I", packet[:4])[0]
		mode, channel, _, _ = struct.unpack("BBBB", packet[4:8])
		if timestamp < self.last_timestamp:
			self.t_correction += 0xFFFFFFFF
		self.last_timestamp = timestamp
		timestamp += self.t_correction
		self.timestamp = float(timestamp/ 168E6) * 200
		

		if channel == 0x11:
			data = struct.unpack("hhhh", packet[8:16])
			axis, grav, _, _ = data
			print("LGA cal axis={} grav={}".format(axis, grav))
			return "Remove before flight"
		if channel == 0x12:
			data = struct.unpack("hhhh", packet[8:16])
			axis, grav, _, _ = data
			print("HGA cal axis={} grav={}".format(axis, grav))
			return "Remove before flight"
		if channel == 0x50: ##dt, height 
			data = struct.unpack("ff", packet[8:16])
			self.altitude_1 = data[1]
			self.datascreen.ids.bar1.setCurVal(data[1]) #update acceleration bar as desired
#			return (" " + str(channel) + " alt: " + str(data[1]) + "\n")
			return (str(timestamp) + " " + str(channel) + " alt: " + str(data[1]) + "\n")

#            se_t.append(timestamp / 168E6)
#            se_h.append(data[1])
#			return "Remove before flight"
		if channel == 0x51: ##velocity, acceleration
			data = struct.unpack("ff", packet[8:16])
			self.velocity_1 = data[0]
			self.acceleration_1 = data[1]
			self.datascreen.ids.bar2.setCurVal(data[0])
			self.datascreen.ids.bar3.setCurVal(data[1])
#			return (" " + str(channel) + " vel: " + str(data[0]) + " acc: " + str(data[1]) + "\n")	
			return str(timestamp) + " " + str(channel) + " vel: " + str(data[0]) + " acc: " + str(data[1]) + "\n"
#            se_v.append(data[0])
#            se_a.append(data[1])
			return "Remove before flight"
		if channel == 0x52:
			#I don't know what this channel sends, but for now it updates alt_2
			data = struct.unpack("ff", packet[8:16])
			self.datascreen.ids.bar4.setCurVal(p2a(data[0]))			
			baro_t.append(timestamp / 168E6)
			baro_h.append(p2a(data[0]))
			return str(timestamp) + " " + str(channel) + " baro: " + str(data[0]) + " a??: " + str(data[1]) + "\n"
		if channel == 0x53:
			data = struct.unpack("ff", packet[8:16])
			accel_t.append(timestamp / 168E6)
			accel_a.append(data[0])
			return "Remove before flight"
		if channel == 0x40:
			data = struct.unpack("ii", packet[8:16])
			mission_t.append(timestamp / 168E6)
			mission_s.append(data[1])
			return "Remove before flight"
		if channel == 0x20:
			x, y, z, _ = struct.unpack("hhhh", packet[8:16])
			lga_y.append(y)
			return "Remove before flight"
		if channel == 0x21:
			x, y, z, _ = struct.unpack("hhhh", packet[8:16])
			hga_y.append(y)
			return "Remove before flight"
		if channel == 0x61:
			ch1, ch2, ch3, _ = struct.unpack("hhhh", packet[8:16])
			pyro_fire_t.append(timestamp / 168E6)
			if ch1:
				pyro_fire_c.append(1)
			elif ch2:
				pyro_fire_c.append(2)
			elif ch3:
				pyro_fire_c.append(3)
			return "Remove before flight"
		#######
		else:
			return "Unknown channel"
		
		
		

        #print a, b, mode, channel
#        if mode == 9:
#            data = struct.unpack("ff", packet[8:])
#

#        if channel == 0x50: ##dt, height
#            #print data[0]
#            self.altitude_1 = data[1]
#            self.datascreen.ids.bar1.setCurVal(data[1])#update acceleration bar as desired
#            return str(timestamp[0]) + " " + str(channel) + " alt: " + str(data[1]) + "\n"
#        elif channel == 0x51: ##velocity, acceleration
#            self.velocity_1 = data[0]
#            self.acceleration_1 = data[1]
#            self.datascreen.ids.bar2.setCurVal(data[0])
#            self.datascreen.ids.bar3.setCurVal(data[1])
 #           return str(timestamp[0]) + " " + str(channel) + " vel: " + str(data[0]) + " acc: " + str(data[1]) + "\n"
#

    def update(self, datascreen):
        self.datascreen = datascreen

        if not self.hasStarted: return
#        m, s = divmod(self.time.seconds, 60)
#        milliseconds = self.time.microseconds/1000.0
#        xVal = m*60.0 + s + milliseconds/1000.0

        s = getData(inFile).next()
        parsed = self.parse(s)
        datascreen.ids.commandoutput.text = (parsed + "\n" + datascreen.ids.commandoutput.text)[:500]
        

#        self.altitude_1 = 150 * math.sqrt(xVal) 
#        self.altitude_2 = 500 - (xVal - 20)**2 + 75
#        if (self.altitude_1 > 500):
#            self.parachute_1 = True


class MainScreen(Screen):
    pass

class Screen2(Screen):
    pass

class DataScreen(Screen): 

        
    def update(self):
        self.ids.commandoutput.text = (str(flightData.timestamp) + "\n" + self.ids.commandoutput.text)[:500]
        self.ids.timer.update()

        #self.ids.bar1.update()
        #self.ids.bar2.update()
        #self.ids.bar3.update()
        #self.ids.bar4.update()

        self.ids.status1.update()
        self.ids.status2.update()
        self.ids.status3.update()
        self.ids.status4.update()
        self.ids.status5.update()

        self.ids.graph.update()
        flightData.update(self)

class changescreen(BoxLayout):
    pass

class StatusBox(Button):
    background_color = ListProperty((100, 100, 0, 1))
    altitude = NumericProperty(0.0)
    color = ListProperty((0,0,1,1))
    font_size = NumericProperty(15)
    status = BooleanProperty(False)
    def update(self):
        if not flightData.hasStarted: return
        if(self.status):
            self.background_color = (0, 1, 0 ,1)
            self.color = (255,255,255,1)

class Timer(Widget):
    st = datetime.now()
    started = False

    def button_clicked(self):
        if not self.started:
            self.startTime = datetime.now()
            self.started = True
            flightData.hasStarted = True
        else:
            self.started = False
            flightData.hasStarted = False
    def getTime(self):
        if self.started:
            return self.mTime
        else:
            return 0.0
    def hasStarted(self):
        return self.started
    def update(self):
        if self.started:
            self.mTime = (datetime.now() - self.startTime)
            m, s = divmod(self.mTime.seconds, 60)
            milliseconds = self.mTime.microseconds/1000.0
            self.ids.timerButton.text = "%0.2d:%0.2d:%0.2d" %(m,s,milliseconds/10)
            flightData.time = self.mTime

class Bar(BoxLayout):
    maxVal = NumericProperty(100.0)
    curVal = NumericProperty(20.0)

    def setMaxVal(self, num):
        self.maxVal = num
    def setCurVal(self, num):
        self.curVal = num
    def update(self):
        self.curVal = random.random() * self.maxVal


class LineGraph(Graph):
    xMax = NumericProperty(60.0)
    yMax = NumericProperty(1000.0)
    yMin = NumericProperty(0.0)
    xMin = NumericProperty(0.0)
    xLabel = Property("Time (s)")
    yLabel = Property("Altitude (m)")
    plot_1 = MeshLinePlot(color=[1,0,0,1])
    plot_2 = MeshLinePlot(color = [0, 1, 0, 1])
    yVal_1 = NumericProperty(0.0)
    yVal_2 = NumericProperty(0.0)
    xVal = NumericProperty(0.0)   
    
    def __init__(self, **kwargs):
        super(LineGraph, self).__init__(**kwargs)
        self.add_plot(self.plot_1)
        self.add_plot(self.plot_2)

    def update(self):
        if not flightData.hasStarted: return
        self.xVal = flightData.timestamp
#        m, s = divmod(flightData.time.seconds, 60)
#        milliseconds = flightData.time.microseconds/1000.0
#        self.xVal = m*60.0 + s + milliseconds/1000.0
  
        self.yVal_1 = flightData.altitude_1

        self.plot_1.points.append((self.xVal, self.yVal_1))

        self.xMax = max(self.xMax, self.xVal + 5)
        self.yMax = max(self.yMax, self.yVal_1 + 30, self.yVal_2 + 30)
        #self.add_plot(self.plot_1)
           
        if(flightData.parachute_1):
            self.yVal_2 = flightData.altitude_2
            self.plot_2.points.append((self.xVal, self.yVal_2))

 


if __name__=="__main__":
    inFile = open("log_00027.bin", "rb")
    flightData = FlightData()
    app = GroundstationApp()

    app.run()

