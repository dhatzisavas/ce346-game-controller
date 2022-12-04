import serial
import time
import threading
from enum import Enum

class DEVS(Enum):
    JOYSTICK_X = 0
    JOYSTICK_Y = 1
    BUTTON_BLUE = 2
    BUTTON_GREEN = 3
    BUTTON_RED = 4
    BUTTON_YELLOW = 5

# most significant byte = index 0
def getByte(byteStrInt: int, byteNum: int, byteLen: int) -> int:
    shift_size = 8 * (byteLen - byteNum - 1)
    mask = 0xFF
    # print("mask: ", hex(mask));
    mask_placed = mask << shift_size
    # print("mask placed: ", hex(mask_placed))
    val = byteStrInt & mask_placed
    # print("val before shift right: ", hex(val))
    # print("after shift: ", hex(val >> shift_size))
    return (val >> shift_size) & mask

# array will be in same byte order as bytes were inserted. Big-endian byte ordering.
# e.g. byte b'\x01\x02' => [0x01, 0x02]


def byteToIntArr(byte: bytes) -> list:
    int_arr = []
    i = 0
    byteInt = int.from_bytes(byte, 'big')
    bytelen = len(byte)
    # print(bytelen)
    while i < bytelen:
        int_arr.append(getByte(byteInt, i, bytelen))
        i += 1
    return int_arr

# ensures input is valid as much as possible
# button values should either be 0 (unpressed) or 1 (pressed)


def validateInputArr(vals: list) -> bool:
    # no checks for first 2 values - anything in range is fine
    for i in range(2, len(vals)):
        if vals[i] != 0 or vals[i] != 1:
            return False
    return True


def findStart(val, old=0):
    map = []
    for i in range(len(val)):
        if val[i] > 1:
            map.append(i)
    # if there are not exactly 2 elements identified as joystick, returns last known start val
    if len(map) != 2:
        return old
    # are the sequential?
    if map[1] - map[0] == 1:
        return map[0]
    else:
        return map[1]


def accessArray(vals: list, start: int, ind: int):
    realInd = (ind + start) % len(vals)
    return vals[realInd]

# #test
# string = b'\x00\x01\x02\x03\x04\x05'
# print(string)
# vals = byteToIntArr(string)
# for i in vals:
#     print(i)


def microbitTest():
    microbit = serial.Serial('/dev/ttyACM0', 38400)
    print(microbit.name)
    start = 0
    while (True):
        raw = microbit.read(6)
        print("message start")
        rawToInts = byteToIntArr(raw)
        start = findStart(rawToInts, start)
        for i in range(6):
            print('Ordered: {0:3d}\t Original:{1:3d}'.format(
                accessArray(rawToInts, start, i), rawToInts[i]))
        print("message end")
        # time.sleep(.005) #this takes too long
        # maybe insert a delay here
        # or a way to trigger a software interrupt if the byte value is valid/makes sense

# meant to run in a parallel context
class MicrobitPolling:

    def __init__(self, message_size, print_time=0.25):
        self.size = message_size
        self.poll = True
        self.print = True
        self.time = print_time
        self.microbit = serial.Serial('/dev/ttyACM0', 38400)
        self.microbitval = self.__readVal()
        self.start = 0

    def __readVal(self):
        return byteToIntArr(self.microbit.read(self.size))

    def getSortedVals(self):
        start = self.start
        sortedLst = []
        i = self.start
        while i < start + self.size:
            sortedLst.append(self.microbitval[i % self.size])
            i+=1
        return sortedLst

    def readingValues(self):
        while (self.poll):
            self.microbitval = self.__readVal()
            self.start = findStart(self.microbitval, self.start)
        self.poll = True

    def printingValues(self):
        while (self.print):
            time.sleep(self.time)
            print("start of values")
            for i in range(self.size):
                print(accessArray(self.microbitval, self.start, i)) 
            print("end of values")
        self.print = True

    def stopPolling(self):
        self.poll = False

    def stopPrinting(self):
        self.print = False


# global microbitval


# def readingValues():
#     microbit = serial.Serial('/dev/ttyACM0', 38400)
#     while (True):
#         global microbitval
#         microbitval = byteToIntArr(microbit.read(6))


# def printingValues():
#     while (True):
#         time.sleep(.25)
#         print("start of values")
#         for i in microbitval:
#             print(i)
#         print("end of values")

# constantly polls, periodically prints. Parallelism helps manage acquiring and interpreting inputs and using them at once
# def parallelTesting():
#     MB = MicrobitPolling(6)
#     print("making threads")
#     read_mb_thread = threading.Thread(target=MB.readingValues)
#     print_mb_thread = threading.Thread(target=MB.printingValues)
#     print("starting")
#     read_mb_thread.start()
#     print_mb_thread.start()
#     read_mb_thread.join()
#     print_mb_thread.join()
#     print("You shouldn't have reached this point, but the functions have both ended")

def printv(microbit):
    while(True):
        lst = microbit.getSortedVals()
        print("start")
        for i in lst:
            print(i)
        print("end")
        time.sleep(.05)

def parallelTesting():
    MB = MicrobitPolling(6)
    print("making threads")
    read_mb_thread = threading.Thread(target=MB.readingValues)
    print_mb_thread = threading.Thread(target=printv, args=(MB,))
    print("starting")
    read_mb_thread.start()
    print_mb_thread.start()
    read_mb_thread.join()
    print_mb_thread.join()
    print("You shouldn't have reached this point, but the functions have both ended")

parallelTesting()
