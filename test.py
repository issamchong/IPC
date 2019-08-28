import serial
import time
ser= serial.Serial("/dev/ttyUSB1", 9600,timeout=1)
ser.write("{404hoLA}")

time.sleep(1)
ser.close




