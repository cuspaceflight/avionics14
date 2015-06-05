
class FlightData():
    def __init__(self):
        
        self.radiocontact = False
        self.goforllaunch = False
        
        self.started = False
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
        self.text = "CUSF"
    """
    TODO: add getters and setters
    """
    def getText(self):
        return self.text

    def addText(self,text):
        self.text = str(text) + "\n" + self.text
        self.text = (self.text)[:1024]
        #self.text = self.text[:1024]

    def setText(self, text):
        """
        TODO: cut off string after certain point?
        """
        self.text = str(text)[:1024]

    def hasStarted(self):
        return self.started

    def setStarted(self,started):
        self.started = started

    def setTime(self, time):
        self.time = time

    def getTime(self):
        return self.time

    def setVel1(self,vel):
        self.velocity_1 = vel

    def getVel1(self):
        return self.velocity_1

    def setAcc1(self,acc):
        self.acceleration_1 = acc
    
    def getAcc1(self):
        return self.acceleration_1

    def setIgnited1(self,ignited):
        self.ignited_1 = ignited

    def getAlt1(self):
        return self.altitude_1
    
    def setAlt1(self, alt):
        self.altitude_1 = alt

    def getAlt2(self):
        return self.altitude_2

    def setAlt2(self,alt):
        self.altitude_2 = alt

    def par1(self):
        return self.parachute_1

    
    
