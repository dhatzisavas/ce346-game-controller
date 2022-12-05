import serial
import time
import threading
from enum import Enum
import platform

#may need to change port number for windows depending on system
PORT = 'COM6' if platform.system() == 'Windows' else '/dev/ttyACM0'


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
    mask_placed = mask << shift_size
    val = byteStrInt & mask_placed
    return (val >> shift_size) & mask


# array will be in same byte order as bytes were inserted. Big-endian byte ordering.
# e.g. byte b'\x01\x02' => [0x01, 0x02]
def byteToIntArr(byte: bytes) -> list:
    int_arr = []
    i = 0
    byteInt = int.from_bytes(byte, 'big')
    bytelen = len(byte)
    while i < bytelen:
        int_arr.append(getByte(byteInt, i, bytelen)) #could this be done more efficiently? yes
        i += 1
    return int_arr


# ensures input is valid as much as possible
# button values should either be 0 (unpressed) or 1 (pressed)
def validateInputArr(vals: list) -> bool:
    # no checks for first 2 values - anything in range is fine, and physically can't be out of range
    for i in range(2, len(vals)):
        if vals[i] != 0 or vals[i] != 1:
            return False
    return True


#iterates through list, returning the actual start of transmission for that set of data
def findStart(val, old=0):
    map = []
    for i in range(len(val)):
        if val[i] > 1:
            map.append(i)
    # if there are not exactly 2 elements identified as joystick, returns last known start val
    if len(map) != 2:
        return old
    # are they sequential?
    if map[1] - map[0] == 1:
        return map[0]
    else:
        return map[1]


#acceses array based on pre-determine start value - like circle queue (I think?)
def accessArray(vals: list, start: int, ind: int):
    realInd = (ind + start) % len(vals)
    return vals[realInd]


# #test
# string = b'\x00\x01\x02\x03\x04\x05'
# print(string)
# vals = byteToIntArr(string)
# for i in vals:
#     print(i)


#Tests receiving raw inputs and ordering them
#used to understand features only
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

#encapsulates all data and methods required to receive the desired data
#from the microbit
class MicrobitPolling:

    def __init__(self, message_size, print_time=0.25):
        self.size = message_size
        self.poll = True
        self.print = True
        self.time = print_time
        self.microbit = serial.Serial(PORT, 38400, timeout=.2)
        self.start = 0
        self.drive = False
        self.sleep = 0.05
        self.microbitval = self.__readVal()

    def __readVal(self):
        val = byteToIntArr(self.microbit.read(self.size))
        if len(val) != 6:
            return [0] * 6
        return val

    def getSortedVals(self):
        start = self.start
        sortedLst = []
        i = self.start
        while i < start + self.size:
            sortedLst.append(self.microbitval[i % self.size])
            i += 1
        return sortedLst

    #meant to be run in a parallel context
    #only reads microbit inputs and updates stored value (self.microbitval)
    def readingValues(self):
        while (self.poll):
            self.microbitval = self.__readVal()
            self.start = findStart(self.microbitval, self.start)
        self.poll = True

    #for testing - run in parallel to readingValues()
    def printingValues(self):
        while (self.print):
            time.sleep(self.time)
            print("start of values")
            for i in range(self.size):
                print(accessArray(self.microbitval, self.start, i))
            print("end of values")
        self.print = True

    #attempts to write char to microbit to start/not start haptic driver
    #time of vibration is predetermined at 500ms
    def driveHapticAutomatic(self):
        while (True):
            if self.drive:
                print("writing yes")
                self.microbit.write(int.to_bytes('a', 1, 'big'))
                self.drive = False
            else:
                print("writing no")
                self.microbit.write(int.to_bytes(0x00, 1, 'big'))

    #merges readingValues() and driveHapticAutomatic()
    #i thought this would resolve concurrency issues (both threads accessing Microbit at once),
    #assuming serial.read and serial.write are thread safe
    def readAndDrive(self):
        while (self.poll):
            self.microbitval = self.__readVal()
            self.start = findStart(self.microbitval, self.start)
            if self.drive:
                self.microbit.write(b'aaaaa');
                self.drive = False
            else:
                self.microbit.write(int.to_bytes(0x00, 1, 'big'))
            # if self.sleep != 0:
            #     time.sleep(self.sleep)

    def doDrive(self):
        self.drive = True

    def stopPolling(self):
        self.poll = False

    def stopPrinting(self):
        self.print = False


#previous test code to be deleted once complete
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

#prints out the values last captured by microbit - helper for testing
def printv(microbit):
    while (True):
        lst = microbit.getSortedVals()
        print("start")
        for i in lst:
            print(i)
        print("end")
        time.sleep(.05)


# attempt to test out sending characters to microbit to start haptic driver
# Failing
def parallelTesting():
    MB = MicrobitPolling(6)
    print("making threads")
    read_mb_thread = threading.Thread(target=MB.readAndDrive)
    # drive_thread = threading.Thread(target=MB.driveHapticAutomatic)
    print_mb_thread = threading.Thread(target=printv, args=(MB,))
    print("starting")
    read_mb_thread.start()
    print_mb_thread.start()
    print("here")
    MB.doDrive()
    print("did it once")
    time.sleep(2000)
    MB.doDrive()
    print("both drives done")
    # print_mb_thread.start()
    read_mb_thread.join()
    print_mb_thread.join()
    print("You shouldn't have reached this point, but the functions have both ended")


# parallelTesting()
