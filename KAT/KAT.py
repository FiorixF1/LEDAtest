import time
import serial
import sys



SL = sys.argv[1]
N0 = sys.argv[2]
assert SL in ["1", "2", "3", "4", "5"], "SL must be 1, 2, 3, 4 or 5"
assert N0 in ["2", "3", "4"], "N0 must be 2, 3 or 4"
key = (SL, N0)

files = {("1", "2"): "PQCkem-ephKAT_24_2.rsp",
         ("1", "3"): "PQCkem-ephKAT_24_3.rsp",
         ("1", "4"): "PQCkem-ephKAT_24_4.rsp",
         ("2", "2"): "PQCkem-ephKAT_32_2.rsp",
         ("2", "3"): "PQCkem-ephKAT_32_3.rsp",
         ("2", "4"): "PQCkem-ephKAT_32_4.rsp",
         ("3", "2"): "PQCkem-ephKAT_32_2.rsp",
         ("3", "3"): "PQCkem-ephKAT_32_3.rsp",
         ("3", "4"): "PQCkem-ephKAT_32_4.rsp",
         ("4", "2"): "PQCkem-ephKAT_40_2.rsp",
         ("4", "3"): "PQCkem-ephKAT_40_3.rsp",
         ("4", "4"): "PQCkem-ephKAT_40_4.rsp",
         ("5", "2"): "PQCkem-ephKAT_40_2.rsp",
         ("5", "3"): "PQCkem-ephKAT_40_3.rsp",
         ("5", "4"): "PQCkem-ephKAT_40_4.rsp"}

SLEEPING_TIME = { '1': 0.09,
                  '2': 0.27,
                  '3': 0.27,
                  '4': 0.27,
                  '5': 0.27 }
SLEEPING_TIME = SLEEPING_TIME[SL]

TIMEOUT = { '1': 10,
            '2': 20,
            '3': 20,
            '4': 30,
            '5': 30 }
TIMEOUT = TIMEOUT[SL]

with open(files[key], "r") as f:
    data = f.readlines()



# configure the serial connections (the parameters differs on the device you are connecting to)
ser = serial.Serial(
    port='/dev/ttyACM0',
    baudrate=115200,
    parity=serial.PARITY_NONE,
    stopbits=serial.STOPBITS_ONE,
    bytesize=serial.EIGHTBITS
)

ser.isOpen()



for i in range(len(data)):
    words = data[i].split()
    if len(words) != 0 and words[0] == 'seed':
        val = words[-1]
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
            ending_time = time.time() + TIMEOUT
            while ser.inWaiting() == 0 and time.time() < ending_time:
                pass
            while ser.inWaiting() > 0:
                out += ser.read().decode('utf-8')
                if ser.inWaiting() < 10:
                    time.sleep(SLEEPING_TIME)
            print(out.split('\n'))  # here I get the seed
            
            # let's do it a second time
            out = ''
            ending_time = time.time() + TIMEOUT
            while ser.inWaiting() == 0 and time.time() < ending_time:
                pass
            while ser.inWaiting() > 0:
                out += ser.read().decode('utf-8')
                if ser.inWaiting() < 10:
                    time.sleep(SLEEPING_TIME)
            print(out.split('\n'))  # here I get pk, sk, ct, ss
            
            expected_pk = data[i+1][:-1]  # remove ending \n
            expected_sk = data[i+2][:-1]
            expected_ct = data[i+3][:-1]
            expected_ss = data[i+4][:-1]
            
            out = out.split('\n')
            real_pk = out[1][1:]  # remove starting \r
            real_sk = out[2][1:]
            real_ct = out[3][1:]
            real_ss = out[4][1:]
            
            assert expected_pk == real_pk, "\aPublic key mismatch at row {}\nExpected: {}\nReal: {}".format(i+2, expected_pk, real_pk)
            assert expected_sk == real_sk, "\aSecret key mismatch at row {}\nExpected: {}\nReal: {}".format(i+3, expected_sk, real_sk)
            assert expected_ct == real_ct, "\aCiphertext mismatch at row {}\nExpected: {}\nReal: {}".format(i+4, expected_ct, real_ct)
            assert expected_ss == real_ss, "\aShared key mismatch at row {}\nExpected: {}\nReal: {}".format(i+5, expected_ss, real_ss)
            
            # read data again to flush them
            out = ''
            ending_time = time.time() + TIMEOUT
            while ser.inWaiting() == 0 and time.time() < ending_time:
                pass
            while ser.inWaiting() > 0:
                out += ser.read().decode('utf-8')
                if ser.inWaiting() < 10:
                    time.sleep(SLEEPING_TIME)
            print(out.split('\n'))
