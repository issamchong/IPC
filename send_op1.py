import serial
import time
ser= serial.Serial("/dev/ttyUSB2", 9600,timeout=1)
ser.write("7B3135305468697")
time.sleep(0.2)
ser.write("32069732052544F")
time.sleep(0.2)
ser.write("5320636F7572736")
time.sleep(0.2)
ser.write("52054503120696E")
time.sleep(0.2)
ser.write("20746573742C616")
time.sleep(0.2)
ser.write("E64206974206973")
time.sleep(0.2)
ser.write("20776F726B696E6")
time.sleep(0.2)
ser.write("7217D")
ser.close




