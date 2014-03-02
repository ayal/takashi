import time
import serial
import threading
import json
sp = serial.Serial()
sp.port = 'COM3'
sp.baudrate = 115200
sp.parity = serial.PARITY_NONE
sp.bytesize = serial.EIGHTBITS
sp.stopbits = serial.STOPBITS_ONE
sp.timeout = 3
sp.xonxoff = False
sp.rtscts = False
sp.dsrdtr = False

pedic = {}

def stop():
    sp.write('s')

def start():
    sp.write('.')


def fadeone(c,ps,pe,d):

    ps = pedic[c] if pedic.has_key(c) else 0
    pedic[c] = pe

    msg = '!' + str(c).zfill(2) + str(ps).zfill(3) + str(pe).zfill(3) + str(d).zfill(2)  + '.'

    if ps == pe:
#        print 'buff'
        return

    print 'msg >>>> ' + msg
    sp.write(msg)

leds = [[2,1,0], [3,4,5], [6,7,8], [11,10,9], [13,14,15], [16,17,18], [21,20,19], [22,23,24], [25, 26, 27], [28,29,31], [34,33,32], [35,36,37], [40,39,38], [41,42,43], [47,45,44]]

def fadeleds(ledsn, color1, color2, time):
    for led in ledsn:
        led_c = leds[led]
        for i in [0,1,2]:
            fadeone(led_c[i], color1[i], color2[i], time)

def fadeall(who,ps=0,pe=255,d=2):
    for w in who:
        fadeone(w, ps, pe, d)

def fadegreen(ps=0,pe=255,d=2):
    fadeall([1,4,12],ps,pe,d)

def fadered(ps=0,pe=255,d=2):
    fadeall([0,3,14],ps,pe,d)

def fadeblue(ps=0,pe=255,d=2):
    fadeall([2,5,13],ps,pe,d)

def fadeviolet(d=2):
    fadered(d=d)
    fadeblue(d=d)

def fadeaqua(ps=0,pe=255,d=2):
    fadeblue(ps,pe,d)
    fadegreen(ps,pe,d)

def fadeyellow():
    fadegreen()
    fadered(pe=150)

def fadeaaalll(ps=0,pe=255,d=2):
    fadered(ps,pe,d)
    fadegreen(ps,pe,d)
    fadeblue(ps,pe,d)

def fadewhite():
    fadeaaalll(0,255,2)

def fadeblack():
    fadeaaalll(255,0,2)

def sea():
    while True:
        fadeaqua()
        time.sleep(2)
        fadegreen(255,50)
        time.sleep(2)
        fadeblue(255,50)
        time.sleep(2)

def haze():
    while True:
        fadeviolet(d=1)
        time.sleep(1)
        fadered(255,50,1)
        time.sleep(1)
        fadeblue(255,50,1)
        time.sleep(1)

sp.open()

value = sp.readline()

from BaseHTTPServer import BaseHTTPRequestHandler,HTTPServer


PORT_NUMBER = 8082

lock = threading.Lock()




#This class will handles any incoming request from
#the browser

class sThread(threading.Thread):
    """Thread class with a stop() method. The thread itself has to check
    regularly for the stopped() condition."""

    def __init__(self,animations):
        threading.Thread.__init__(self)
        self._stop = threading.Event()
        self.animations = animations

    def stop(self):
        self._stop.set()

    def stopped(self):
        return self._stop.isSet()

    def run(self):
        print 'animate'
        print self.animations[0]
#        while True:
#            if self.stopped():
#                break
        for frame in self.animations:
            for cmd in frame:
                eval(cmd['cmd'])
                print '>> ' + cmd['cmd']


class myHandler(BaseHTTPRequestHandler):
	#Handler for the GET requests
        t = None

	def do_POST(self):
            self.data_string = self.rfile.read(int(self.headers['Content-Length']))
            self.send_response(200)
            self.send_header('Access-Control-Allow-Origin', '*')
            self.end_headers()
            data = json.loads(self.data_string)

            if myHandler.t:
                myHandler.t.stop()
                myHandler.t.join()

            myHandler.t = sThread(data)
            myHandler.t.daemon = True
            myHandler.t.start()

            return

try:
	#Create a web server and define the handler to manage the
	#incoming request
	server = HTTPServer(('', PORT_NUMBER), myHandler)
	print 'Started httpserver on port ' , PORT_NUMBER

	#Wait forever for incoming htto requests
	server.serve_forever()

except KeyboardInterrupt:
	print '^C received, shutting down the web server'
	server.socket.close()
