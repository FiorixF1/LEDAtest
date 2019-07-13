import time
import serial

# configure the serial connections (the parameters differs on the device you are connecting to)
ser = serial.Serial(
    port='/dev/ttyACM0',
    baudrate=115200,
    parity=serial.PARITY_NONE,
    stopbits=serial.STOPBITS_ONE,
    bytesize=serial.EIGHTBITS
)

ser.isOpen()


print('Enter your commands below.\r\nInsert "exit" to leave the application.')

val = 1
while True:
    # get keyboard input
    val = input(">> ")
    if val == 'exit':
        ser.close()
        exit()
    else:
        # send the character to the device
        if val != '':
            payload = val.encode('utf-8')
            ser.write(payload)
            ser.flushInput()

        out = ''
        # let's wait one second before reading output (let's give device time to answer)
        time.sleep(5)
        while ser.inWaiting() > 0:
            out += ser.read(1).decode('utf-8')
        if out != '':
            print(">>" + out)


"""
with open("./KAT/PQCkem-ephKAT_24_3.rsp", "r") as f:
    data = f.readlines()
    
for line in data:
    words = line.split()
    if len(words) == 3 and words[0] == 'seed':
        print("Testing with seed {}...".format(words[-1]))
        payload = words[-1].encode('utf-8')
        ser.write(payload)
        ser.flushInput()
        time.sleep(10)
"""