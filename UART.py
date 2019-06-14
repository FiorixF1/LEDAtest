import time
import serial

# configure the serial connections (the parameters differs on the device you are connecting to)
ser = serial.Serial(
    port='/dev/ttyACM0',
    baudrate=115200,
    parity=serial.PARITY_ODD,
    stopbits=serial.STOPBITS_TWO,
    bytesize=serial.SEVENBITS
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
        # (note that I happend a \r\n carriage return and line feed to the characters - this is requested by my device)
        if val != '':
            payload = val.encode('utf-8')
            ser.write(payload)

        out = ''
        # let's wait one second before reading output (let's give device time to answer)
        time.sleep(1)
        while ser.inWaiting() > 0:
            out += ser.read(1).decode('utf-8')
        if out != '':
            print(">>" + out)