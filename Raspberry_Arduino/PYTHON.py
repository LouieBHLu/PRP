import serial    
ser = serial.Serial('/dev/ttyACM0', 9600,timeout=1)   

#try and exceptstructure are exception handler
try:
  while 1:
    ser.write('s')
    response = ser.readall()
    print(response)
except:
  ser.close()

