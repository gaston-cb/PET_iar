import paho.mqtt.client as mqtt
import random
import time 
import socket 
### parameters of Broker MQTT
BROKER_MQTT= "localhost" 
PORT_MQTT  = 1883
TOPIC = "test/topic"

### parameters of Gpredict rotator
HOST_GPREDICT = "localhost"
SOCKET_GPREDICT = 4533 

antenna_pointing_electronic_device = {
    "azimuth":32.00, 
    "altura":45.83, 
}

antenna_pointing_gpredict = {
    "azimuth":0.00, 
    "altura":0.00, 
}




def onConnect():
    pass 



def onDisconnect(client, userdata, message):
    pass 



## Message arrived to electronic device
def Rxmessage(client, userdata, message): 
    print("message received " ,str(message.payload.decode("utf-8")))
    print("message topic=",message.topic) # if topic-> update position #uopdate antena_pointing variable
    print("message qos=",message.qos)
    print("message retain flag=",message.retain) 
    #send a position to Gpredict 


## Message send to a electronic device
def Txmessage(string_gpredict,topic):
    local_param = string_gpredict.split(' ')
    az = float(local_param[1])
    h  = float(local_param[2])
    client.publish(TOPIC,f'{az} and {h}')
#def R

## INIT_BROKER 
client_name = "rot_iar_pet"
client =mqtt.Client(client_name)
client.on_message = Rxmessage
#client.on_disconnect = onDisconnect --> define a pi and pm action or decision if using these case
client.connect(BROKER_MQTT)
client.subscribe(TOPIC)
client.loop_start()
#client.publish(TOPIC,"HOLA MUNDO DESDE PYTHON")
#client.loop_forever()



CMD_SEND = {"OK_COMMAND": "RPRT 0\r\n", 
             "SEND_POSITION":'P',
             "GET_POSITION":'p'} 

## SOCKETS_GPREDICT
gpredict_socket = socket.socket()
gpredict_socket.bind((HOST_GPREDICT,SOCKET_GPREDICT))
gpredict_socket.listen(1)
#@FIXME: ADD A SIGNAL CAUGHT! 
while True:
    print ("wait connectrion")
    conn, address = gpredict_socket.accept()
    #conn.sendall(CMD_SEND["OK_COMMAND"].encode('ascii'))
    while conn:
        #print ("wait rx data")
        data = conn.recv(100).decode('ascii')
        ## mientras gpredict esre conectado !  
        #print(f'rx: {data}')
        if (data[0] == 'S'):
            print('exit command')
            conn.close()
            break  
        elif (data[0] == 'p'):
            print("received p") 
            conn.sendall(f'{antenna_pointing_electronic_device["azimuth"]}\r\n{antenna_pointing_electronic_device["altura"]}\r\n'.encode('ascii'))
        elif (data[0] == 'P'):
            Txmessage(data,TOPIC)
            conn.sendall(CMD_SEND["OK_COMMAND"].encode('ascii'))
        else:   
            print("command error")
    #conn.close()             

