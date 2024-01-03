import serial 
import datetime

FILE_SAMPLE = ''
PORT_SERIAL = '/dev/ttyACM0' 
BAUDRATE = 115200 
NAME_FILE= 'data.txt'

def read_serial(objSerial,cmd_send):
    print(f'cmd: {cmd_send}')
    objSerial.write(cmd_send.encode('ascii'))
    with open(NAME_FILE,'w') as f: 
        i = 0 
        while i<2000:
            str0 = objSerial.readline().decode('ascii') 
            if (str0[0].isdigit() == True):
                i =i+1 
                f.write(str0)
    

def set_angle(objSerial,cmd_send): 
    print(f'cmd: {cmd_send}')
    objSerial.write(cmd_send.encode('ascii'))
    
    



ser = serial.Serial(PORT_SERIAL,BAUDRATE)
str = input("presione a para continuar ")
print(datetime.datetime.now())
read_serial(ser,str) 
ser.close() 
print(datetime.datetime.now())
print("end of scripting") 
