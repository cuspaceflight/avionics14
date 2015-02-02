import kivy

from kivy.uix.gridlayout import GridLayout
from kivy.uix.relativelayout import RelativeLayout
from kivy.uix.boxlayout import BoxLayout
from kivy.properties import NumericProperty, ReferenceListProperty, ObjectProperty

from kivy.app import App
from kivy.uix.button import Button
from kivy.uix.widget import Widget
from kivy.clock import Clock
from random import randint
import time

class Dial(Widget):
   pass 

class Timer(Widget):
    mTime = NumericProperty(0) 
    startTime = time.time()

    def button_clicked(self):
        self.startTime = time.time()
    
    def update(self, dt):
        self.mTime = time.time() - self.startTime
class Display(Widget):
    
    def update(self, dt):
        
        self.ids.label2.text = "Acceleration :" + str(randint(0,1000))

class Layout(Widget):

    mVelocity = ObjectProperty(None)
    mAcceleration = ObjectProperty(None)
    mAltitude = ObjectProperty(None)
    mStage = ObjectProperty(None)


    pass

class MainScreen(BoxLayout):
    accelerationDisplay = ObjectProperty(None)
    timerDisplay = ObjectProperty(None)
    def update(self,dt):
        
        self.ids.accel1.text = "Acceleration :" + str(randint(0,1000))
        self.timerDisplay.update(dt)
        #self.accelerationDisplay.update(dt)
    def button_clicked(self):
        
        self.ids.home.counter +=1
        self.ids.button1.text = "You clicked " + str(self.ids.home.counter) + " times!"


class GroundStationApp(App):
    def build(self):
        mainScreen = MainScreen()
        Clock.schedule_interval(mainScreen.update, 10.0/60.0)
        return mainScreen 
    
if __name__ == '__main__':
    GroundStationApp().run()
