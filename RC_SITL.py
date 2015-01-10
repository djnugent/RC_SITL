import serial
from numpy import ushort
import time

"""
Daniel Nugent
with DroneApi snippets from Randy Mackay


Program that allows the use of an RC Transmitter with ArduPilot SITL
"""


class RCControl(object):

    def __init__(self):
        self.api = None
        self.vehicle = None

        #Used to send RC signals at 50hz
        self.last_send = 0

        #Open connection to arduin
        #Try ttyACM0 or ttyACM1
        self.ser = serial.Serial()
        self.ser.port = '/dev/ttyACM0'
        self.ser.baudrate = 115200
        try:
            self.ser.open()
        except:
            print 'Serial port not found'


    # connect - connects to droneAPI.
    #    because of scope issues the local_connect() must be called from the top level class
    def connect(self, api):
        # First get an instance of the API endpoint (the connect via web case will be similar)
        self.api = api

        # if we succesfully connect
        if not self.api is None:
            # get our vehicle (we assume the user is trying to control the virst vehicle attached to the GCS)
            self.vehicle = self.api.get_vehicles()[0]
            return

    #is_connected - are we connected to a DroneApi
    def is_connected(self):
        if (self.api is None) or (self.vehicle is None):
            return False
        return (not self.api.exit)

    # read_serial_PPM - read 8 channel serial stream from arduino
    def read_serial_PPM(self):
        line = self.ser.readline().strip()
        raw_values = line.split(',')
        self.values = []
        try:
            for i in range(0,len(raw_values)):
                self.values.append(ushort(raw_values[i]))
        except:
            pass
        
    # set_RC - send RC commands to ArduPilot
    def set_RC(self):
        if(time.time() - self.last_send > 0.02):
            self.last_send = time.time()
            for i in range(0,len(self.values)):
                self.vehicle.channel_override = { str(i+1) : self.values[i] }
            self.vehicle.flush()

    # run - runs the main program
    def run(self):
        if(self.ser.isOpen()):
            print 'RC control started'
            while self.is_connected() and self.ser.isOpen():
                self.read_serial_PPM()
                self.set_RC()
            self.ser.close()
        else:
            print 'unable to start RC control'

        print 'RC control terminated'


# if this is the parent class connect and run test
if __name__ == "__builtin__":
    veh_control = RCControl()
    veh_control.connect(local_connect())
    veh_control.run()
