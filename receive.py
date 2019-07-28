import serial, time

ser = serial.Serial('/dev/ttyUSB1', 9600,timeout=1)

while 1:
    serial_line = ser.readline()


    print(serial_line)
   

ser.close() # Only executes once the loop exisize
