"""
beefi : Communication module to communicate with beeOS.
-------

Louis-Émile Robitaille
Johnny Chea
Jean-Sebastien Pelletier-Rioux
Dominic Aucoin

Université Laval Aut. 2016
"""

import time
import queue as q
import socket
import threading

from definitions import TCP_IP, TCP_PORT, BUFFER_SIZE
from database import getPackagesFromBytes

class Connect2BeeOS:

    def __init__(self, ip, port, buffer_size):
        # Arguments
        self.ip = ip
        self.port = port
        self.buffer_size = buffer_size

        # Flag
        self.enable_ = False
        self.connected_ = False
        
        # Queues
        self.data_queue = q.Queue()
        self.response_queue = q.Queue()
        self.ack_queue = q.Queue()

        # Thread Safe Lock
        self.lock = threading.Lock()

    def send(self, to_send):
        self.lock.acquire()
        ##
        if self.enable_:
            if type(to_send) == str:
                self.s.send(to_send.encode('ascii'))
            else:
                self.s.send(to_send)
        else:
            print(" >> The socket is closed")
        ##
        self.lock.release()

    def recvData_(self):
        while 1:
            raw = self.s.recv(self.buffer_size)
            if not self.enable_:
                break
            if len(raw) == 0:
                self.reconnect()
            else:
                recvs = getPackagesFromBytes(raw)
                for recv in recvs:
                    if recv['type_'] == 'data':
                        self.data_queue.put(recv['content'])
                    elif recv['type_'] == 'res':
                        self.response_queue.put(recv['content'])
                    elif recv['type_'] == 'ack':
                        self.ack_queue.put(recv['content'])

    def recv_(self):
        if self.enable_:
            self.t = threading.Thread(target=self.recvData_)
            self.t.setDaemon(True)
            self.t.start()
        else:
            print(" >> The socket is closed")
        return self.data_queue, self.response_queue

    def checkBeeOS_(self):
        self.connected_ = False
        self.send(bytearray([0,0,0,0,0]))
        _ = self.ack_queue.get()
        self.connected_ = True

    def byeBeeOS_(self):
        self.send(bytearray([7,0,0,0,0]))

    def connect_(self):
        self.lock.acquire()
        ##
        self.s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.s.connect((self.ip, self.port))
        self.enable_ = True
        ##
        self.lock.release()

    def connect(self):
        self.connect_()
        self.recv_()
        self.checkBeeOS_()
        return self.data_queue, self.response_queue

    def reconnect(self):
        self.enable_ = False
        self.connect_()

    def isEnable(self):
        return self.enable_

    def isConnected(self):
        return self.connected_

    def deInit(self):
        self.byeBeeOS_()
        self.lock.acquire()
        ##
        if self.enable_:
            self.enable_ = False
            self.connected_ = False
            self.s.shutdown(socket.SHUT_RDWR)
        ##
        self.lock.release()


