
import serial
import time
ser= serial.Serial("/dev/ttyUSB2", 9600,timeout=1)
ser.write("ggghga\r\n")

