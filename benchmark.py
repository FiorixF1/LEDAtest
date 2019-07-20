import time
import serial
import sys
from subprocess import Popen, PIPE

# configure the serial connections (the parameters differs on the device you are connecting to)
ser = serial.Serial(
    port='/dev/ttyACM0',
    baudrate=115200,
    parity=serial.PARITY_NONE,
    stopbits=serial.STOPBITS_ONE,
    bytesize=serial.EIGHTBITS
)

ser.isOpen()



SLEEPING_TIME = 0.03  # SL 1
#SLEEPING_TIME = 0.09  # SL 2-3
#SLEEPING_TIME = 0.9 #0.27  # SL 4-5

# extract seeds
with open("./KAT/PQCkem-ephKAT_24_2.rsp", "r") as f:
    data = f.readlines()

seeds = []
count = 0

for row in data:
    words = row.split()
    if len(words) != 0 and words[0] == 'seed':
        seeds.append(words[-1])


"""
# questo funziona manualmente, NON mettere input quando devono uscir ei benchmark, perche' lui non riceve niente!
while count < 20:
    # get keyboard input
    val = input(">> ")
    #val = seeds[count]
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
        # let's wait before reading output (let's give device time to answer)
        ending_time = time.time() + 10
        while ser.inWaiting() == 0 and time.time() < ending_time:
            pass
        while ser.inWaiting() > 0:
            out += ser.read().decode('utf-8')
            time.sleep(0.01)
        print(out)
        
    count += 1
"""

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

        # let's wait before reading output (let's give device time to answer)
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
        # and even a third time... it is needed for sync purposes
        #out = ''
        #ending_time = time.time() + 10
        #while ser.inWaiting() == 0 and time.time() < ending_time:
        #    pass
        #while ser.inWaiting() > 0:
        #    out += ser.read().decode('utf-8')
        #    time.sleep(0.01)
        #print(out)
        
    count += 1
    
# final results
out = ''
ending_time = time.time() + 10
while ser.inWaiting() == 0 and time.time() < ending_time:
    pass
while ser.inWaiting() > 0:
    out += ser.read().decode('utf-8')
    time.sleep(SLEEPING_TIME)
print(out)


# da long long little endian hex a human-readable
#int(''.join(reversed([x[i:i+2] for i in range(0, len(x), 2)])), 16)

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