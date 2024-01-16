import serial 
import datetime
import re 
import time
FILE_SAMPLE = ''
PORT_SERIAL = '/dev/ttyACM0' 
BAUDRATE = 115200 
NAME_FILE= '../../../results/TC-V-ADC/VERT/TC-V-ADC-3.10.txt'
expresion_regular = r'^\d+(,\d+){0,2}\r\n$'

def read_serial(objSerial,cmd_send):
    print(f'cmd: {cmd_send}')
    objSerial.write(cmd_send.encode('ascii'))
    with open(NAME_FILE,'w') as f: 
        i = 0 
        flag = True
        while flag:
            str0 = objSerial.readline().decode('ascii')   
            if (',' in str0):
                data = str0.split(',')
                f.write(str0)
                if (int(data[0])==2000-1):
                    flag = False
    

def set_angle(objSerial,cmd_send): 
    print(f'cmd: {cmd_send}')
    objSerial.write(cmd_send.encode('ascii'))
    
    



ser = serial.Serial(PORT_SERIAL,BAUDRATE)
str = input("presione a para continuar ")
print(datetime.datetime.now())

if str!='Z' and str!='z': 
    read_serial(ser,str) 
else: 
    ser.write(str.encode('ascii'))
    print(f"ESCRIBI {str}")

# read_serial(ser,str) 
time.sleep(1)
ser.close() 
print(datetime.datetime.now())
# print("end of scripting") 
