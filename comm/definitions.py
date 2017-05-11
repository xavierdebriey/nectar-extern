"""
beefi:
---------
Define the constants for the programs
---------
Design 4, Université Laval, Aut. 2016
---------
"""

"""
Database
"""

SIZEOFDATA = 32

"""
Communication
"""

TCP_IP = '192.168.1.10'
TCP_PORT = 9750
BUFFER_SIZE = 4096

"""
Commands
"""

COMMANDS = {
    "toggle green": "\001\000\000\000\000",
    "toggle red": "\002\000\000\000\000",
    "change measure 1": "\003\001\000\000\000",
    "change measure 2": "\003\002\000\000\000",
    "change measure 3": "\003\003\000\000\000",
    "change measure 4": "\003\004\000\000\000",
    "change measure 5": "\003\005\000\000\000",
    "change sending 1": "\004\001\000\000\000",
    "change sending 3": "\004\003\000\000\000",
}