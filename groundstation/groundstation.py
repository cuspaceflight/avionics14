
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

    def parse(self,packet):
        timestamp = struct.unpack("i", packet[:4])
        a, b, mode, channel = struct.unpack("BBBB", packet[4:8])

        #print a, b, mode, channel
        if mode == 9:
            data = struct.unpack("ff", packet[8:])


        if channel == 0x50: ##dt, height
            #print data[0]
            self.altitude_1 = data[1]
            self.datascreen.ids.bar1.setCurVal(data[1])#update acceleration bar as desired
            return str(timestamp[0]) + " " + str(channel) + " alt: " + str(data[1]) + "\n"
        elif channel == 0x51: ##velocity, acceleration
            self.velocity_1 = data[0]
            self.acceleration_1 = data[1]
            self.datascreen.ids.bar2.setCurVal(data[0])
            self.datascreen.ids.bar3.setCurVal(data[1])
            return str(timestamp[0]) + " " + str(channel) + " vel: " + str(data[0]) + " acc: " + str(data[1]) + "\n"


    def update(self, datascreen):
        self.datascreen = datascreen

        if not self.hasStarted: return
        m, s = divmod(self.time.seconds, 60)
        milliseconds = self.time.microseconds/1000.0
        xVal = m*60.0 + s + milliseconds/1000.0

        s = getData(inFile).next()
        parsed = self.parse(s)
        datascreen.ids.commandoutput.text = (parsed + datascreen.ids.commandoutput.text)[:500]
        

#        self.altitude_1 = 150 * math.sqrt(xVal) 
        self.altitude_2 = 500 - (xVal - 20)**2 + 75
        if (self.altitude_1 > 500):
            self.parachute_1 = True


class MainScreen(Screen):
    pass

class Screen2(Screen):
    pass

class DataScreen(Screen): 

        
    def update(self):
        #self.ids.commandoutput.text = str(flightData.time) + "\n"
        self.ids.timer.update()

        #self.ids.bar1.update()
        #self.ids.bar2.update()
        #self.ids.bar3.update()
        self.ids.bar4.update()

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
        m, s = divmod(flightData.time.seconds, 60)
        milliseconds = flightData.time.microseconds/1000.0
        self.xVal = m*60.0 + s + milliseconds/1000.0
  
        self.yVal_1 = flightData.altitude_1

        self.plot_1.points.append((self.xVal, self.yVal_1))

        self.xMax = max(self.xMax, self.xVal + 5)
        self.yMax = max(self.yMax, self.yVal_1 + 30, self.yVal_2 + 30)
        #self.add_plot(self.plot_1)
           
        if(flightData.parachute_1):
            self.yVal_2 = flightData.altitude_2
            self.plot_2.points.append((self.xVal, self.yVal_2))

 


if __name__=="__main__":
    inFile = open("testData.txt", "rb")
    flightData = FlightData()
    app = GroundstationApp()

    app.run()

