import serial

#most significant byte = index 0
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

#ensures input is valid as much as possible
#button values should either be 0 (unpressed) or 1 (pressed)
def validateInputArr(vals: list) -> bool:
    #no checks for first 2 values - anything in range is fine
    for i in range(2, len(vals)):
        if vals[i] != 0 or vals[i] != 1:
            return False;
    return True;

def findStart(val, old = 0):
    map = []
    for i in range(len(val)):
        if val[i] > 1:
            map.append(i)
    #if there are not exactly 2 elements identified as joystick, returns last known start val
    if len(map) != 2:
        return old
    #are the sequential?
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

microbit = serial.Serial('/dev/ttyACM0', 38400)
print(microbit.name)
start = 0
while(True):
    raw = microbit.read(6)
    print("message start")
    rawToInts = byteToIntArr(raw)
    start = findStart(rawToInts, start)
    for i in range(6):
        print(accessArray(rawToInts, start, i))
    print("message end")
    # maybe insert a delay here
    # or a way to trigger a software interrupt if the byte value is valid/makes sense