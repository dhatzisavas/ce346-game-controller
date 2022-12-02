import serial

microbit = serial.Serial('/dev/ttyACM0', 38400)
print(microbit.name)
while(True):
    microbit.write('a')
    raw = microbit.read(6)
    value = int.from_bytes(raw, "little")