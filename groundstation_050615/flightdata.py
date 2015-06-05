
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
        self.states = ["PAD", "IGNITION","POWERED_ASCENT","BURNOUT","FREE-ASCENT","IGNITE","WAIT_IGNITION","SEPARATE","SEPARATED_ASCENT","APOGEE","DROGUE_DEPLOY","DROGUE_DESCENT","MAIN_DEPLOY","MAIN_DESCENT","TOUCHDOWN","LANDED"]
        self.state = 0



        """State variables"""
        self.status_launched = False
        self.status_drogueDeployed = False
        self.status_ignited = False
        self.status_mainDeployed = False
        self.status_touchdown = False

    def launched(self):
        return self.status_launched
    def drogueDeployed(self):
        return self.status_drogueDeployed
    def ignited(self):
        return self.status_ignited
    def mainDeployed(self):
        return self.status_mainDeployed
    def touchdown(self):
        return self.status_touchdown
    def getNumState(self, state):
        return self.states[state]
    def getState(self):
        return self.state
    def setState(self,state):
        if (state >=1):
            self.status_launched = True
        if (state==10):
            self.status_drogueDeployed = True
        if (state == 5):
            self.status_ignited = True
        if (state == 12):
            self.status_mainDeployed = True
        if (state == 14 or state == 15):
            self.status_touchdown = True
        self.state = state
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

    def getVel2(self):
        return self.velocity_2

    def setVel2(self,vel):
        self.velocity_2 = vel

    def getAcc2(self):
        return self.acceleration_2

    def setAcc2(self,acc):
        self.acceleration_2 = acc

    def par1(self):
        return self.parachute_1

    
    
