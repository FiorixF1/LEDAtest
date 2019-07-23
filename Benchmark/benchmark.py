import time
import serial
import sys

# configure the serial connections (the parameters differs on the device you are connecting to)
ser = serial.Serial(
    port='/dev/ttyACM0',
    baudrate=115200,
    parity=serial.PARITY_NONE,
    stopbits=serial.STOPBITS_ONE,
    bytesize=serial.EIGHTBITS
)

ser.isOpen()



SL = sys.argv[1]
N0 = sys.argv[2]
assert SL in ["1", "2", "3", "4", "5"], "SL must be 1, 2, 3, 4 or 5"
assert N0 in ["2", "3", "4"], "N0 must be 2, 3 or 4"

SLEEPING_TIME = { '1': 0.03,
                  '2': 0.09,
                  '3': 0.09,
                  '4': 0.27,
                  '5': 0.27 }
SLEEPING_TIME = SLEEPING_TIME[SL]

# extract seeds
with open("seeds.txt", "r") as f:
    seeds = f.readlines()
seeds = [seed[:-1] for seed in seeds]



count = 0
while count < 30:
    val = seeds[count]
    print(">> " + val)
    if val == 'exit':
        ser.close()
        exit()
    else:
        # send the character to the device
        if val != '':
            payload = val.encode('utf-8')
            ser.write(payload)
            ser.flushInput()

        # let's wait before reading output (give device time to answer)
        out = ''
        ending_time = time.time() + 10
        while ser.inWaiting() == 0 and time.time() < ending_time:
            pass
        while ser.inWaiting() > 0:
            out += ser.read().decode('utf-8')
            time.sleep(SLEEPING_TIME)
        print(out)
        
        # let's do it a second time
        out = ''
        ending_time = time.time() + 10
        while ser.inWaiting() == 0 and time.time() < ending_time:
            pass
        while ser.inWaiting() > 0:
            out += ser.read().decode('utf-8')
            time.sleep(SLEEPING_TIME)
        print(out)
        
    count += 1
    
# print final results
out = ''
ending_time = time.time() + 10
while ser.inWaiting() == 0 and time.time() < ending_time:
    pass
while ser.inWaiting() > 0:
    out += ser.read().decode('utf-8')
    time.sleep(SLEEPING_TIME)
print(out)



# convert from long long little-endian hex to human-readable decimal number
# int(''.join(reversed([x[i:i+2] for i in range(0, len(x), 2)])), 16)
