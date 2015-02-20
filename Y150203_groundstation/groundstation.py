
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


#Layout classes:

class FlightData():
    def __init__(self):
        
        #Rocketstatus
        self.radiocontact = False
        self.goforllaunch = False
        
        #General
        self.hasStarted = False
        self.TheTime = None
                
        #stage1
        self.altitude_1 = 0.0
        self.velocity_1 = 0.0
        self.acceleration_1 = 0.0
        self.gpspos_1 = (0.0, 0.0)
        self.rotation_1 = [0.0, 0.0, 0.0, 0.0]
        self.ignited_1 = False
        self.parachute_1 = False
        
        #stage2
        self.altitude_2 = 0.0
        self.velocity_2 = 0.0
        self.acceleration_2 = 0.0
        self.gpspos_2 = (0.0, 0.0)
        self.rotation_2 = [0.0, 0.0, 0.0, 0.0]
        self.ignited_2 = False
        self.parachute_2 = False
        
        #stage nosecone
        self.parachute_3 = False
        
    def update(self, dt):
        #I am not completely sure waht to do here. 
        #I was thinking we have several if tests checking what type of information has arrived and updates that. 
        #and then we 
#        radio_alt_1 = True 
#        if (radio_alt_1):
#            self.altitude_1 += 1

        if not myflightdata.hasStarted: return
        m, s = divmod(self.TheTime.seconds, 60)
        milliseconds = self.TheTime.microseconds/1000.0
        xVal = m*60.0 + s + milliseconds/1000.0
        self.altitude_1 = 150 * math.sqrt(xVal)    
        self.altitude_2 = 500 - (xVal - 20)**2 + 75
        if (self.altitude_1 > 500):
            self.parachute_1 = True
            
        
    
    def writetofile(self, dt):
        pass
        
class GroundstationApp(App):
    def build(self):
        mainScreen = MyScreenManager()
        Clock.schedule_interval(mainScreen.update, 10.0/60.0)
        return mainScreen

        
class MyScreenManager (ScreenManager):
    dataScreen = ObjectProperty(None) 
    def update(self,dt):

        self.dataScreen.update(dt) 
    
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


    def update(self, myflightdata):
        if not myflightdata.hasStarted: return
        m, s = divmod(myflightdata.TheTime.seconds, 60)
        milliseconds = myflightdata.TheTime.microseconds/1000.0
        self.xVal = m*60.0 + s + milliseconds/1000.0
        self.yVal_1 = myflightdata.altitude_1

        self.plot_1.points.append((self.xVal, self.yVal_1))
        self.xMax = max(self.xMax, self.xVal + 5)
        self.yMax = max(self.yMax, self.yVal_1 + 30, self.yVal_2 + 30)
        self.add_plot(self.plot_1)
           
        if(myflightdata.parachute_1):

            self.yVal_2 = myflightdata.altitude_2
            self.plot_2.points.append((self.xVal, self.yVal_2))
            self.add_plot(self.plot_2)
        

class StatusBox(Button):
    background_color = ListProperty((100, 100, 0, 1))
    altitude = NumericProperty(0.0)
    color = ListProperty((0,0,1,1))
    font_size = NumericProperty(15)
    status = BooleanProperty(False)
    def update(self, myflightdata, dt):
        if not myflightdata.hasStarted: return
        if(self.status):
            self.background_color = (0, 1, 0 ,1)
            self.color = (255,255,255,1)


class Screen1(Screen):
    pass
        
class changescreen(BoxLayout):
    pass

class Screen2(Screen):
    pass

class Screen3(Screen):
#    def __init__(self, **kwargs):
#        super(Screen3, self).__init__(**kwargs)
#        self.add_widget(Graph(
#            y_ticks_major = 10,
#            y_grid=True,
#            xlabel='X',
#            ylabel='Y',
#        ))


    def update(self, dt):

        
        #update timer and commandline (I think this takes up a lot of memory so i commented out the saving thing)
        self.ids.commandoutput.text =  str(myflightdata.TheTime) + "\n" # + self.ids.commandoutput.text
        self.ids.timer.update(myflightdata, dt) 
        
        #update bars
        self.ids.bar1.update(myflightdata, dt)
        self.ids.bar2.update(myflightdata, dt)
        self.ids.bar3.update(myflightdata, dt)
        self.ids.bar4.update(myflightdata, dt)

        #update status
        self.ids.status1.update(myflightdata, dt)
        self.ids.status3.update(myflightdata, dt)
        self.ids.status3.update(myflightdata, dt)
        self.ids.status4.update(myflightdata, dt)
        self.ids.status5.update(myflightdata, dt)  
        
        #update graph
        self.ids.graph.update(myflightdata)
        
        #update flight data
        myflightdata.update(dt)
    
#properties classes




class Timer(Widget):
    
    startTime = datetime.now() 
    started = False
    
    def button_clicked(self):
        if not self.started:
            self.startTime = datetime.now()
            self.started = True
            myflightdata.hasStarted = True
        else:
            self.started = False
            myflightdata.hasStarted = True
    def getTime(self):
        if self.started:
            return self.mTime
        else:
            return 0.0
    def hasStarted(self):
        return self.started

    def update(self, myflightdata, dt):
        if self.started:
            self.mTime = (datetime.now() - self.startTime)
            myflightdata.TheTime = self.mTime
            m, s = divmod(self.mTime.seconds, 60)
            milliseconds = self.mTime.microseconds/1000
            self.ids.timerButton.text = "%0.2d:%0.2d:%0.2d" %(m,s,milliseconds/10)
    



class Bar(BoxLayout):
    maxValue = NumericProperty(100.0)
    curValue = NumericProperty(20.0)
    def update(self, myflightdata, dt):
        self.curValue = random.random() * self.maxValue 
    
    
    




if __name__=="__main__":
    myflightdata = FlightData()
    GroundstationApp().run()

