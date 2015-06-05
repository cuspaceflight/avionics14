


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
from functools import partial
import multiprocessing
from multiprocessing import Process, Queue
from flightdata import FlightData
from datetime import datetime
import random
import time
import struct
import serial
import Queue
import binascii



    
class GroundstationApp(App):    
    debug = False
    """
    args[0] is message queue
    """
    def __init__(self, *args, **kwargs):
        super(GroundstationApp,self).__init__(**kwargs)
        self.queue = args[0]
        self.fd = FlightData()
        
    
    def build(self):
        self.gui = Gui(self.fd)
        if self.debug:
            Clock.schedule_interval(self.scheduledUpdatesTest, 10.0/60.0)     
        else:
            Clock.schedule_interval(self.scheduledUpdates,10.0/60.0)
        return self.gui

    def scheduledUpdates(self,dt):
        qsize = self.queue.qsize()
        #self.fd.addText(qsize)
        

        while (not self.queue.empty() and qsize > 0):
            try:
                qsize -= 1
                nextTask = self.queue.get()
                self.fd.addText(nextTask)
                #self.gui.update(dt)
                parse(nextTask)
            except Queue.Empty:

                break
            except ValueError:
                self.fd.setText("Should have code! But anyways... " + nextTask)
                self.gui.update(dt)
                continue 
            except: ###TODO - bad
                continue
            

        try:    
            self.gui.update(dt)
        except:
            return

        
    def parse(packet):
            #packet = packet.strip()
            #packet = binascii.unhexlify(packet)
            if (len(packet)!=16):
                #TODO: handle error
                return packet 
            timestamp = struct.unpack("i", packet[:4])
            metadata, channel, cs1, cs2 = struct.unpack("BBBB", packet[4:8])
            origin = metadata >> 4
            mode = metadata & 15

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
                self.fd.addText("Initialisation message: " + data[0])
                return "Initialisation message", data[0]
            elif channel == 0x10:
                self.fd.addText("Calibration - timestamp frequency " + data[0])
                return "Calibration - timestamp frequency ", data[0]
            elif channel == 0x11:
                self.fd.addText("Calibration = lg_accel: axis, gravMag " + data[0] + " " + data[1])
                return "Calibration - lg_accel: axis, gravMag", data[0], data[1]
            elif channel == 0x12:
                self.fd.addText("Calibration - hg_accel: axis, gravMag " + data[0] + " " + data[1])
                return "Calibration - hg_accel: axis, gravMag", data[0], data[1]
            elif channel == 0x13: 
                self.fd.addText("Calibration - barometer: " + data[0] + " " + data[1] + " " + data[2] + " " + data[3])
                return "Calibration - barometer: d0, c1, c2, c3", data[0], data[1], data[2], data[3]
            elif channel == 0x14:
                self.fd.addText("Calibration - barometer: " + data[0] + " " + data[1] + " " + data[2] + " " + data[3]) 
                return "Calibration - barometer: c4, c5, c6, c7", data[0], data[1], data[2], data[3]
            elif channel == 0x20:
                self.fd.addText("IM - lg_accel: x, y, z " + data[0] + " " + data[1] + " " + data[2])
                return "IM - lg_accel: x, y, z", data[0], data[1], data[2]
            elif channel == 0x21:
                self.fd.addText("IM - hg_accel: x y z " + data[0] + " " + data[1] + " " + data[2])
                return "IM - hg_accel: x, y, z", data[0], data[1], data[2]
            elif channel == 0x22:
                self.fd.addText("IM - barom: pressure, temperature " + data[0] + " " + data[1])
                return "IM - barom: pressure, temperature", data[0], data[1]
            elif channel == 0x23:
                ###TODO
                return "IM - rate gyroscope", data
            elif channel == 0x24:
                ###TODO
                return "Im - magnetometer", data
            elif channel == 0x30:
                ###TODO
                return "External sensors - battery voltage", data[0]
            elif channel == 0x31:
                ###TODO
                return "External sensors - strain gauges : ch1, ch2, ch3", data[0], data[1], data[2]
            elif channel == 0x32:
                ###TODO
                return "External sensors - thermocouples: ch1, ch2, ch3", data[0], data[1], data[2]
            elif channel == 0x40:
                self.fd,addText("Mission control: old_state, new_state " + data[0] + " " + data[1])
                return "M2FC Mission Control: old_state, new_state", data[0], data[1]
            elif channel == 0x50:
    #            self.altitude_1 = data[1]
    #            self.datascreen.ids.bar1.setCurVal(data[1])#update acceleration bar as desired
                self.fd.addText("State est: I: dt, height " + data[0] + " " + data[1])
                return "State estimation: pred output I: dt, height", data[0], data[1]
            elif channel == 0x51:
                self.fd.addText("State est II: vel, acc " + data[0] + " " + data[1])
                return "State estimation: pred output II: vel, acc", data[0], data[1]
            elif channel == 0x52: 
                self.fd.addText("State est: pressure: sensor,estimate " + data[0] + " " + data[1])
                return "State estimation: pressure measurement: sensor, estimate", data[0], data[1]
            elif channel == 0x53:
                self.fd.addText("State est: acc " + data[0] + " " + data[1])
                return "State estimation: acceleration measurement: sensor, estimate", data[0], data[1]
            elif channel == 0x60:
                self.fd.addText("Pyrotechnics: continuity: ch1,ch2,ch3 " + data[0] + " " + data[1] + " " + data[2])
                return "Pyrotechnics: continuity results: ch1, ch2, ch3", data[0], data[1], data[2]
            elif channel == 0x61:
                self.fd.addtext("Pyro fired: ch1, ch2, ch3 " + data[0] + " "  +data[1] + " " + data[2])
                return "Pyrotechnics: fired: ch1, ch2, ch3", data[0], data[1], data[2]

            return packet 

    """
    List of all updates
    """
    def scheduledUpdatesTest(self,dt):
       
        if (not self.queue.empty()):
            nextTask = self.queue.get()
            try:
                code, msg = nextTask.strip().split()
            except ValueError:
                self.fd.setText("Should have code! But anyways... " + nextTask)
                self.gui.update(dt)
                return
 
            """
            TODO: update altitudes/velocity/etc based on different codes
            Update string of form "x y" w/0 the quotes where x is code and y is msg
            """
            if (code == "0"):
                self.fd.setText(msg)
                #self.fd.setText(self.fd.getAcc1())
            elif (code == "1"):
                self.fd.setAlt1(float(msg))
            elif (code=="2"):
                self.fd.setVel1(float(msg))
            elif (code == "3"):
                self.fd.setAcc1(float(msg))
            elif (code == "4"):
                self.fd.setAlt2(float(msg))
            else:
                self.fd.setText("Code does not exist, but here's your msg " + msg)
        try:    
            self.gui.update(dt)
        except:
            return
   

class Gui(ScreenManager):
    dataScreen = ObjectProperty(None)
    
    def __init__(self, *args, **kwargs):
        super(Gui,self).__init__(**kwargs)
        self.fd = args[0]
        self.dataScreen.fd = self.fd
        self.dataScreen.setfd()
    def update(self, dt):
        
        self.dataScreen.update() 

class MainScreen(Screen):
    pass

class Screen2(Screen):
    pass

class DataScreen(Screen): 

    def setfd(self):
        self.ids.timer.fd = self.fd
        ###TODO: set default and maximum values
        self.ids.bar1.fd = self.fd
        self.ids.bar1.setMaxVal(3500.0)

        self.ids.bar2.fd = self.fd
        self.ids.bar2.setMaxVal(50.0)

        self.ids.bar3.fd = self.fd
        self.ids.bar3.setMaxVal(330.0)

        self.ids.bar4.fd = self.fd
        self.ids.bar4.setMaxVal(3500.0)
        self.ids.status1.fd = self.fd
        self.ids.status2.fd = self.fd
        self.ids.status2.fd = self.fd
        self.ids.status3.fd = self.fd
        self.ids.status4.fd = self.fd
        self.ids.status5.fd = self.fd
#        self.ids.graph.fd = self.fd
        self.ids.commandoutput.fd = self.fd
    
    def update(self):
    #self.ids.commandoutput.text = str(flightData.time) + "\n"
        self.ids.commandoutput.text = self.fd.text + "\n"
        self.ids.timer.update()
        """python strip
        Poor substitute for bars updating, but to demonstrate functionality...

        TODO: update bars based on fd
        TODO: set max and min vals of bars based on what they represent
        TODO: decide what bars represent
        """
        #self.ids.bar1.update()

        ##Altitude1
        self.ids.bar1.setCurVal(self.fd.getAlt1())


        ##Velocity1
        self.ids.bar2.setCurVal(self.fd.getVel1())
        #self.ids.bar2.update()
        
        ##Acceleration1
        self.ids.bar3.setCurVal(self.fd.getAcc1())
        #self.ids.bar3.update()

        ###Altitude2
        self.ids.bar4.setCurVal(self.fd.getAlt2())
        #self.ids.bar4.update()


        self.ids.status1.update()
        self.ids.status2.update()
        self.ids.status3.update()
        self.ids.status4.update()
        self.ids.status5.update()

#        self.ids.graph.update()


class changescreen(BoxLayout):
    pass

class StatusBox(Button):
    background_color = ListProperty((100, 100, 0, 1))
    altitude = NumericProperty(0.0)
    color = ListProperty((0,0,1,1))
    font_size = NumericProperty(15)
    status = BooleanProperty(False)

    def setStatus(self, stat):
        self.status = BooleanProperty(stat)

    def update(self):
        if(self.status):
            self.background_color = (0, 1, 0 ,1)
            self.color = (255,255,255,1)

class Timer(Widget):
    st = datetime.now()
    

    def button_clicked(self):
        if not self.fd.hasStarted():
            self.startTime = datetime.now()
            self.fd.setStarted(True)
                        
        else:
            self.fd.setStarted(False)
        
    def getTime(self):
        if self.fd.hasStarted():
            return self.mTime
        else:
            return 0.0
    def hasStarted(self):
        return self.started
    def update(self):
        if self.fd.hasStarted():
            self.mTime = (datetime.now() - self.startTime)
            m, s = divmod(self.mTime.seconds, 60)
            milliseconds = self.mTime.microseconds/1000.0
            self.ids.timerButton.text = "%0.2d:%0.2d:%0.2d" %(m,s,milliseconds/10)
            self.fd.setTime(self.mTime)



class Bar(BoxLayout):
    maxVal = NumericProperty(100.0)
    curVal = NumericProperty(10.0)

    def setMaxVal(self, num):
        self.maxVal = num
    def setCurVal(self, num):
    
        self.curVal = num

    def update(self):
        self.curVal = random.random() * self.maxVal

"""

class LineGraph(Graph):
    xMax = NumericProperty(60.0)
    yMax = NumericProperty(1000.0)
    yMin = NumericProperty(0.0)
    xMin = NumericProperty(0.0)
    xLabel = Property("Time (s)")
    yLabel = Property("Altitude (m)")
    plot_1 = MeshLinePlot(color=[100,0,0,1])
    plot_2 = MeshLinePlot(color = [0, 1, 0, 1])
    yVal_1 = NumericProperty(0.0)
    yVal_2 = NumericProperty(0.0)
    xVal = NumericProperty(0.0)   
    
    def __init__(self, *args, **kwargs):
        super(LineGraph,self).__init__(**kwargs)
        self.add_plot(self.plot_1)
        self.add_plot(self.plot_2)

    def update(self):
        if not self.fd.hasStarted(): return


        m, s = divmod(self.fd.getTime().seconds, 60)
        milliseconds = self.fd.getTime().microseconds/1000.0
        self.xVal = m*60.0 + s + milliseconds/1000.0
        
        self.yVal_1 = self.fd.getAlt1()
        self.plot_1.points.append((self.xVal, self.yVal_1))
        self.plot_2.points = [(1,50),(30,100)]
        self.xMax = max(self.xMax, self.xVal + 5)
        self.yMax = max(self.yMax, self.yVal_1 + 30, self.yVal_2 + 30)




        """
"""           
        if(self.fd.par1()):
            self.yVal_2 = self.fd.getAlt2()
            self.plot_2.points.append((self.xVal, self.yVal_2))
"""
"""

"""


if __name__=="__main__":
    q = multiprocessing.Queue()

    app = GroundstationApp(q).run()
    p = multiprocessing.Process(target = app.run)
    p.start()
    time.sleep(1)

    
    """
    while(1):
        inputText = raw_input("Command")
        q.put(inputText)

    
    """
    ser = serial.Serial('/dev/ttyACM3',115200,timeout=1)
    while True:
        try:
            data = ser.readline().strip()
            print data
            q.put(data)
        except:
            continue
    ser.close()
    

