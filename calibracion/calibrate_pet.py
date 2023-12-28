from astropy.coordinates import EarthLocation, get_sun, AltAz
from astropy.time import Time
import paho.mqtt.client as mqtt
from astropy.coordinates import SkyCoord
import astropy.units as u
from queue import Queue
import math
import socket
import threading
import time
from bitstring import ConstBitStream
import coords
mutex = threading.Lock() 
POSITION_OF_ANTENNA = [-34.866212, -58.139255] 
MQTT_BROKER ='192.168.1.100'
PORT_BROKER = 1883
TOPICS_SUBSCRIBE_RX = [("rotador/angulo_v",0),("rotador/angulo_h",0)]
TOPICS_SUBSCRIBE_TX = ['']


pet_pointing= {
    "azimuth":None, 
    "altura":None, 
}

set_point_pet_antenna = {
    "azimuth": None, 
    "altura": None, 
    
}

stellarium_point = {
    "ra" : None,
    "dec": None, 
}


detener_hilo = threading.Event() #EVENT FOR FINISH MQTT SERVICE IN SCRIPT 
detener_hilo_hijo = threading.Event() #EVENT FOR FINISH MQTT SERVICE IN SCRIPT 

#subscribe_functions 


def Rxmessage(client, userdata, message): 
    #print(TOPICS_SUBSCRIBE_RX[0][0])
    global mutex 
    mutex.acquire()
    if (message.topic == TOPICS_SUBSCRIBE_RX[0][0]):
        pet_pointing["altura"] = float(message.payload.decode("utf-8"))
        if (pet_pointing["altura"]>=90.0 ):
            pet_pointing["altura"]=90.0
        #print(f' {str(message.payload.decode("utf-8"))}  ') 
    elif (message.topic == TOPICS_SUBSCRIBE_RX[1][0]):
        pet_pointing["azimuth"] = float(message.payload.decode("utf-8"))
        #print(f' {str(message.payload.decode("utf-8"))}  ') 
    mutex.release()

    # print("message qos=",message.qos)
    # print("message retain flag=",message.retain) 
    # #pet_pointing = 





def mqtt_service(eq): 
    client_name = "rot_iar_pet"
    client =mqtt.Client(client_name)
    client.on_message = Rxmessage
    client.connect(MQTT_BROKER,port= PORT_BROKER)
    client.subscribe(TOPICS_SUBSCRIBE_RX)
    client.loop_start()
    mqtt_pet = True
    while (mqtt_pet): 
#    while not event.is_set():
        num = eq.get() 
        eq.task_done()
        if (num == 'exit'):
           break 
        set_point_pet_antenna["altura"]  = num[0] 
        set_point_pet_antenna["azimuth"] = num[1] 
        client.publish('rotador/pid_v',f'U{num[0]}')
    client.disconnect() 
    print(f'end of service mqtt') 


def set_point_stellarium(evento,cola): 
    pet_station = EarthLocation(lat=POSITION_OF_ANTENNA[0],lon=POSITION_OF_ANTENNA[1],height=10*u.m)
    while not evento.is_set():
        if (stellarium_point["dec"]==None and stellarium_point["ra"] == None):
            time.sleep(0.1)
            continue
        current_time = Time(time.time(),format ='unix')            
        aa = AltAz(obstime=current_time, location=pet_station)
        sky_obj = SkyCoord(stellarium_point["ra"], stellarium_point["dec"])
        local_coord = sky_obj.transform_to(aa) 
        coords_local = [local_coord.alt.deg, local_coord.az.deg]
#        print(sky_obj.frame)
        cola.put(coords_local)
        #publish a set_pount 
        time.sleep(0.5)

    print ("end child sp")







def send_data_stellarium(sock ,ev): 
    msize = '0x1800'
    mtype = '0x0000'
    pet_station = EarthLocation(lat=POSITION_OF_ANTENNA[0],lon=POSITION_OF_ANTENNA[1],height=10*u.m)
    sum = 0.1 
    count = 0 
    while not ev.is_set():
        try: 
            if (pet_pointing["altura"]==None and pet_pointing["azimuth"] ==None):
               time.sleep(0.1) 
               continue 
            #convertir altura y azimut a coordenadas ecuatoriales
            current_time = Time(time.time(),format ='unix')  
            mutex.acquire()
            aa = SkyCoord(AltAz(alt = pet_pointing["altura"]* u.deg, az =pet_pointing["azimuth"]* u.deg,obstime=current_time, location=pet_station))
            mutex.release()

            aa_2 = aa.transform_to('icrs')
            ra_send =  float(aa_2.ra.value) *(math.pi/180.0) #+ 0.1 *sum*count #pet_position_pointing FIXME: S
            dec_send = float(aa_2.dec.value)*(math.pi/180.0) #+ 0.1 *sum*count #pet_position_pointing FIXME: S
            (ra_p, dec_p) = coords.rad_2_stellarium_protocol(ra_send, dec_send)
            cad_bits_str = 'int:64=%r' % time.time() 
            localtime = ConstBitStream(str.replace(cad_bits_str, '.', ''))
            sdata = ConstBitStream(msize) + ConstBitStream(mtype)
            sdata += ConstBitStream(intle=localtime.intle, length=64) + ConstBitStream(uintle=ra_p, length=32)
            sdata += ConstBitStream(intle=dec_p, length=32) + ConstBitStream(intle=0, length=32)
            count = count+1 
            sock.send(sdata.bytes)
            print(pet_pointing)
            time.sleep(2)
        except socket.error: 
            print("socket error")
            sock.close() 
            
    print ("close thread stellarium send")


def handle_client(conn, addr,e):
    # test point data 
    pet_station = EarthLocation(lat=POSITION_OF_ANTENNA[0],lon=POSITION_OF_ANTENNA[1],height=10*u.m)
    connected =True 
    hilo_publish = threading.Thread(target=set_point_stellarium, args=(detener_hilo_hijo,e)) #launch a child thread 
    hilo_publish.start() 
    hilo_dstell = threading.Thread(target=send_data_stellarium, args=(conn,detener_hilo_hijo,)) #launch a child thread 
    hilo_dstell.start()
    while connected: 
        try:
            message = conn.recv(160)
            if len(message) == 0:
                break
            sra = decode_pos(message)
            current_time =Time(sra[2], format='unix')
            #add a mutex 
            stellarium_point["dec"] = sra[1]
            stellarium_point["ra"] = sra[0]
            # free mutex 
        except socket.error: 
            connected = False
            conn.close()
    
    detener_hilo_hijo.set()
    hilo_dstell.join() 
    hilo_publish.join()
    print("[thread] ending")


def decode_pos(data0):
    if data0:			
        data = ConstBitStream(bytes=data0, length=160)
        msize = data.read('intle:16')
        mtype = data.read('intle:16')
        mtime = data.read('intle:64')
        # RA: 
        ant_pos = data.bitpos
        ra = data.read('hex:32')
        data.bitpos = ant_pos
        ra_uint = data.read('uintle:32')
        # DEC:
        ant_pos = data.bitpos
        dec = data.read('hex:32')
        data.bitpos = ant_pos
        dec_int = data.read('intle:32')
        #convert protocol 
        (sra, sdec, stime) = coords.eCoords2str(float("%f" % ra_uint), float("%f" % dec_int), float("%f" %  mtime))
        sdec = sdec.replace('º','d')
        sdec = sdec.replace("''",'s')
        sdec = sdec.replace("'",'m')
        return sra, sdec,math.floor(mtime / 1000000)
            



            
def launch_threads(): 
    host = 'localhost'
    port = 10001
    s = socket.socket()
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)  
    s.bind((host, port))
    s.listen(1)
    e = Queue()
    all_threads = []
    try:
        t = threading.Thread(target=mqtt_service, args=(e,))
        t.start()
        all_threads.append(t)
        while True:
            conn, addr = s.accept()
            print("Client:", addr)
            t = threading.Thread(target=handle_client, args=(conn, addr,e))
            t.start()
            all_threads.append(t)
    except KeyboardInterrupt:
        print("Stopped by Ctrl+C")
    finally:
        print ("disconnect client ")
        e.put('exit')
        if s:
            s.close()
        for t in all_threads:
            print("join threads")
            t.join()





def main(): 
    launch_threads()


main() 
