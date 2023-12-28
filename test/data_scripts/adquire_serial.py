import serial 

FILE_SAMPLE = ''
PORT_SERIAL = '/dev/ttyACM0' 
BAUDRATE = 115200 
NAME_FILE= 'data.txt'

def read_serial(objSerial):
    objSerial.write('a'.encode('ascii'))
    print(objSerial.readline()) 
    with open(NAME_FILE,'w') as f: 
        i = 0 
        while i<2000:

            str0 = objSerial.readline().decode('ascii') 
            i =i+1 
            f.write(str0)
    


ser = serial.Serial(PORT_SERIAL,BAUDRATE)
str = input("presione a para continuar")
read_serial(ser) 




