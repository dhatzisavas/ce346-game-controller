import serial

microbit = serial.Serial('/dev/ttyACM0', 38400)
print(microbit.name)
while(True):
    raw = microbit.read(2)
    value = int.from_bytes(raw, "little");
    print("value ", value)
    if value < 100:
        print("right")
    elif value > 130:
        print("left")
    else:
        print("middle")