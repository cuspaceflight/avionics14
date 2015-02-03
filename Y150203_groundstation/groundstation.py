
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

from kivy.properties import NumericProperty, ReferenceListProperty, ObjectProperty, Property

from kivy.clock import Clock
from kivy.garden.graph import Graph, MeshLinePlot

from random import randint
import random
import string
import math

import time
from datetime import datetime


#Layout classes:

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
    yMax = NumericProperty(50.0)
    yMin = NumericProperty(-50.0)
    xMin = NumericProperty(0.0)
    xLabel = Property("Time (s)")
    yLabel = Property("SOMETHING")
    plot = MeshLinePlot(color=[1,0,0,1])

    def update(self, hasStarted, timePassed):
        if not hasStarted: return
        m, s = divmod(timePassed.seconds, 60)
        milliseconds = timePassed.microseconds/1000.0
        xVal = m*60.0 + s + milliseconds/1000.0
        self.plot.points.append((m*60.0 + s + milliseconds/1000.0,self.yMax*math.sin(xVal)))
        self.xMax = max(self.xMax, xVal + 5)
        self.add_plot(self.plot)

	

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

        self.ids.commandoutput.text =  random.choice(string.letters + "\n") + self.ids.commandoutput.text
        self.ids.timer.update(dt)
        self.ids.bar1.update(dt)
        self.ids.bar2.update(dt)
        self.ids.bar3.update(dt)
        self.ids.bar4.update(dt)
        self.ids.graph.update(self.ids.timer.hasStarted(), self.ids.timer.getTime())
	
#properties classes




class Timer(Widget):
    
    startTime = datetime.now() 
    started = False
	
    def button_clicked(self):
        if not self.started:
            self.startTime = datetime.now()
            self.started = True
        else:
            self.started = False
    def getTime(self):
        if self.started:
            return self.mTime
        else:
            return 0.0
    def hasStarted(self):
        return self.started

    def update(self, dt):
        if self.started:
            self.mTime = (datetime.now() - self.startTime)
            m, s = divmod(self.mTime.seconds, 60)
            milliseconds = self.mTime.microseconds/1000

            self.ids.timerButton.text = "%0.2d:%0.2d:%0.2d" %(m,s,milliseconds/10)
    



class Bar(BoxLayout):
    maxValue = NumericProperty(100.0)
    curValue = NumericProperty(20.0)
    def update(self, dt):
        self.curValue = random.random() * self.maxValue 
    
	
	








if __name__=="__main__":
	GroundstationApp().run()

