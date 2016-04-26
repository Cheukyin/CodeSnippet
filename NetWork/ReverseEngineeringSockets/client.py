'''
In order to reverse engineering a socket program,
you must keep watching the port and the data of each frame captured by WireShark

WireShark filter:
ip.addr == 129.170.213.101
'''

'''
The sensor network protocol:
Each datagram use '\n' for break
Each time the client wants to send datum to the server, it must build a new connection

1. client send "AUTH secretpasswod" to the server
2. server send back a port, and close the connection
3. client use that port to get the temperature info or the power level
'''

import socket
import struct
import time

class SensorReader:
    infoMap = {
        '1': "WATER TEMPERATURE",
        '2': "REACTOR TEMPERATURE",
        '3': "POWER LEVEL",
    }

    def __init__(self):
        self.host = '129.170.213.101'
        self.authPort = 47789
        self.dataBreak = '\n'

    def recv(self):
        buf = ""
        while True:
            character = self.sock.recv(1)
            if character == self.dataBreak:
                break
            buf += character

        msg, = struct.unpack(">%ds" % len(buf), buf)
        return msg

    def send(self, msg):
        data = struct.pack(">%ds" % (len(msg) + 1), msg + self.dataBreak)
        self.sock.sendall(data)

    def connect(self, addr):
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.connect(addr)

    def getInfoPort(self):
        self.connect( (self.host, self.authPort) )
        self.send("AUTH secretpassword")
        msg = self.recv()
        self.sock.close()

        port = msg.split(' ')[-2]
        return int(port)

    def readInfo(self, infoType, port):
        self.connect( (self.host, port) )
        self.send("AUTH networks")
        resp = self.recv()
        if resp != "SUCCESS": return

        self.send(infoType)
        resp = self.recv()
        info = resp.split(" ")

        self.send("CLOSE")
        self.sock.close()

        info[0] = time.asctime( time.localtime( int(info[0]) ) )
        return info

    def prompt(self):
        print("Which sensor would you like to read:\n")
        print("	(1) Water temperature")
        print("	(2) Reactor temperature")
        print("	(3) Power level\n")

        try:
            return raw_input("Selection: ")
        except KeyboardInterrupt:
            exit(0)

    def run(self):
        selection = self.prompt()
        try:
            infoType = self.infoMap[selection]
        except KeyError:
            print("	*** Invalid selection.\n")
            return

        port = self.getInfoPort()

        info = self.readInfo(infoType, port)
        if info == None: return
        print("	The last " + infoType +
              " was taken %s and was %s %s\n" % tuple(info))


if __name__ == "__main__":
    client = SensorReader()
    while True: client.run()
