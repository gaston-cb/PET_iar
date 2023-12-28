/*********************************************************************************
 *  @file main.cpp 
 *  @brief Rotador IAR
 *            Placa maestro para el control del rotador.
 *            Comunicacion I2C a los esclavos para el control de los 
 *            motores y lectura de encoders.
 *  
 *          Hardware:
 *            Raspberry pi Pico
 *            ENC28J60 - Ethernet
 *            INA3221 - Sensor de tension y corriente
 *            DS18B20 - Sensor de temperatura
 *
 *  @author Dario Capucchio
 *  @date 28-03-2023
 *  @version 0.1
 *
 *********************************************************************************/
/* ============= LIBRERIAS ===================================================== */
#include <Arduino.h>
#include <EthernetENC.h>
#include <PubSubClient.h>
#include "ina3221.h"
#include "roti2c.h"
/* ============= DEFINICIONES ================================================== */
// HARDWARE
#define DO1_PIN 1      // Salida digital 1 a GPIO 1 (pico pin 2)
#define DO2_PIN 2      // Salida digital 2 a GPIO 2 (pico pin 4)
#define DO3_PIN 3      // Salida digital 3 a GPIO 3 (pico pin 5)
#define SDA_PIN 4      // Datos de la comunicacion i2c GPIO 4 (pico pin 6)
#define SCL_PIN 5      // Clock de la comunicacion i2c GPIO 5 (pico pin 7)
#define LED1_PIN 14    // LED 1 conectado a GPIO 14 (pico pin 19)
#define LED2_PIN 15    // LED 1 conectado a GPIO 15 (pico pin 20)
// MQTT
#define DELAY_MQTT 500     // Tiempo de espera entre publicaciones en ms
#define CLIENT_ID "ROT_IAR" // ID del cliente mqtt

#define V_MAX 12500     // Valor de la cuenta del esclavo para la velocidad maxima
#define TOPIC1  "rotador/movimiento_h"
#define TOPIC2  "rotador/pid_h"
#define TOPIC3  "rotador/movimiento_v"
#define TOPIC4  "rotador/pid_v"

// FUNCIONES
void callback(char *topic, byte *payload, unsigned int length); // Funcion para la recepcion via MQTT
void reconnect(void);                                           // Reconectar al servidor mqtt
void ftostr(float val, char *dato);
// DEFINICIONES PARA LA CONEXION ETHERNET CON ENC28J60
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEF};  // Dirección MAC del módulo Ethernet
IPAddress server(192, 168, 1,100);                  // IP del broker MQTT
EthernetClient client;                              // Cliente Ethernet
PubSubClient mqttClient(client);                    // Cliente mqtt

// DEFINICION DE OBJETO PARA SENSOR DE CORRIENTE INA3221


// DEFINICION DE OBJETO PARA LA COMUNICACION I2C CON LAS PLACAS ESCLAVO
rot_iar rotador_h(ROT_IAR_PE_H_ADDR);
rot_iar rotador_v(ROT_IAR_PE_V_ADDR);

// VARIABLES DE FLUJO DE PROGRAMA
typedef enum{
  HORARIO,
  ANTIHORARIO,
  STOP,
  CERO,
  PID_ON
}rot_iar_estado_motor;

rot_iar_estado_motor estado_motor_h;
rot_iar_estado_motor estado_motor_v;
long previousMillis;  // Variable para contar el tiempo entre publicaciones
bool flag_mqtt;       // Flag para reconocer la recepcion por mqtt
uint16_t dato_mqtt;   // Dato recibido por mqtt
char comando;         // Comando para la maquina de estados
uint16_t sp_h;        // Set point movimiento horizontal
uint16_t sp_v;        // Set point movimiento vertical
uint16_t velocidad_h; // Velocidad del motor horizontal
uint16_t velocidad_v; // Velocidad del motor vertical
float angulo_h;       // Angulo del movimiento horizontal
float angulo_v;       // Angulo del movimiento vertical
char angulo_str[4];

/* ============= SETUP CORE 0 ================================================== */
void setup()
{
  Serial.begin(115200); // Comunicacion serie con la PC - USB
  pinMode(DO1_PIN, OUTPUT);
  pinMode(DO2_PIN, OUTPUT);
  pinMode(DO3_PIN, OUTPUT);
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  digitalWrite(DO1_PIN, LOW);
  digitalWrite(DO2_PIN, LOW);
  digitalWrite(DO3_PIN, LOW);
  digitalWrite(LED1_PIN, LOW);
  digitalWrite(LED2_PIN, LOW);
  delay(2000) ; 
  Serial.println("init a software master i3c") ; 
  // Espero a que se aprete una tecla para poder verificar por puerto
  // serie la conexion al broker mqtt. Despues se comenta
  /*while(!Serial.available()){
    Serial.println("-> Apreta una tecla cualquiera");
    delay(500);
  }*/
  // Primer mensaje
  Serial.println("----------------------------------------");
  Serial.println("- Rotador IAR ");
  Serial.println("- Placa maestro ");
  Serial.println("----------------------------------------");
  // Inicio modulo Ethernet
  Serial.print("-> Inicializando modulo Ethernet.... ");
  Serial.println(Ethernet.begin(mac));
  if (Ethernet.hardwareStatus() == EthernetNoHardware)
  { // Verifico la comunicacion SPI con el modulo
    Serial.println("--> No hay conexión con el módulo de Ethernet :(");
  }
  if (Ethernet.linkStatus() == LinkOFF)
  { // Verifico si esta el cable de Ethernet conectado
    Serial.println("--> No está conectado el cable de Ethernet!");
  }
  Serial.print("-> IP local: "); // Verifico la direccion de ip
  Serial.println(Ethernet.localIP());
  Serial.print("-> connecting to host...");
  while (!client.connect(server, 1883))
  {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("-> connectado!");
  mqttClient.setServer(server, 1883); // Servidor MQTT
  mqttClient.setCallback(callback);   // Callback para la recepcion via MQTT
  Serial.print("-> Conexion al broker MQTT = ");
  Serial.println(mqttClient.connect(CLIENT_ID)); // Conexion al broker MQTT
  if (!client.connected())
  {
    reconnect();
  }
  // Suscripciones MQTT
  mqttClient.subscribe(TOPIC1);
  mqttClient.subscribe(TOPIC2);
  mqttClient.subscribe(TOPIC3);
  mqttClient.subscribe(TOPIC4);
  mqttClient.loop();
  rotador_h.begin(&Wire);
  rotador_v.begin(&Wire);
  
  // Inicio variables
  flag_mqtt = false;
  previousMillis = millis();
  sp_h = 0;
  sp_v = 0;
  velocidad_h = V_MAX / 2;
  velocidad_v = V_MAX / 2;
  estado_motor_h = STOP;
  estado_motor_v = STOP;
  mqttClient.publish("rotador/estado_h","PARADO");
  mqttClient.publish("rotador/estado_v","PARADO");
  delay(1000);
}
/* ============= LOOP CORE 0 =================================================== */
void loop()
{
  if (Serial.available() > 0 || flag_mqtt == true)
  { // Reviso la comunicacion con la PC o la recepcion por mqtt
    // Obtengo el comando
    if (flag_mqtt == false){
      comando = Serial.read();
    } else {
      flag_mqtt = false;
    }
    // Evaluo el comando
    switch (comando)
    {
      /*** Casos del control horizontal - comandos en minuscula ***/
      case 'o':       // PID ON
        dato_mqtt = 28527;  // 'oo'
        rotador_h.write(ROT_IAR_REG_PID_ON, &dato_mqtt);
        mqttClient.publish("rotador/estado_h","PID ON");
        estado_motor_h = PID_ON;
        break;
      case 'u':
        //sp_h = dato_mqtt * 100;
        rotador_h.write(ROT_IAR_REG_SP, &dato_mqtt);
        break;
      case 'p':
        //kp_h = dato_mqtt * 100;
        rotador_h.write(ROT_IAR_REG_KP, &dato_mqtt);
        break;
      case 'i':
        //ki_h = dato_mqtt * 100;
        rotador_h.write(ROT_IAR_REG_KI, &dato_mqtt);
        break;
      case 'd':
        //kd_h = dato_mqtt * 100;
        rotador_h.write(ROT_IAR_REG_KD, &dato_mqtt);
        break;
      case 's':           // STOP
        Serial.println("-> Detenido");
        dato_mqtt = 29555;  // 'ss'
        rotador_h.write(ROT_IAR_REG_STOP, &dato_mqtt);
        mqttClient.publish("rotador/estado_h","PARADO");
        estado_motor_h = STOP;
        break;
      case 'a':           // Movimiento antihorario
        Serial.println("-> Movimiento antihorario...");
        rotador_h.write(ROT_IAR_REG_ANTIHOR, &velocidad_h);
        mqttClient.publish("rotador/estado_h","MOVIMIENTO ANTIHORARIO");
        estado_motor_h = ANTIHORARIO;
        break;
      case 'h':           // Movimiento horario
        Serial.println("-> Movimiento horario...");
        rotador_h.write(ROT_IAR_REG_HORARIO, &velocidad_h);
        mqttClient.publish("rotador/estado_h","MOVIMIENTO HORARIO");
        estado_motor_h = HORARIO;
        break;
      case 'z':           // Set cero
        Serial.println("-> Buscando cero grados...");
        dato_mqtt = 31354;  // 'zz'
        rotador_h.write(ROT_IAR_REG_SETCERO, &dato_mqtt);     // modifique el registro
        mqttClient.publish("rotador/estado_h","BUSCANDO CERO");
        estado_motor_h = CERO;
        break;
      case 'v':           // Set velocidad
        velocidad_h = (dato_mqtt/100) * V_MAX / 100;
        Serial.print("-> Velocidad horizontal: ");
        Serial.println(velocidad_h);
        break;
      case 'x':                                           // agregue el seteo del maximo en el horizontal
        dato_mqtt = 31354;  // 'zz'
        rotador_h.write(ROT_IAR_REG_SETMAX, &dato_mqtt);
      break ; 
      /*** Casos del control vertical - comandos en MAYUSCULA ***/
      case 'O':       // PID ON
        dato_mqtt = 28527;  // 'oo'
        rotador_v.write(ROT_IAR_REG_PID_ON, &dato_mqtt);
        mqttClient.publish("rotador/estado_v","PID ON");
        estado_motor_v = PID_ON;
        break;
      case 'U':
        //sp_h = dato_mqtt * 100;
        rotador_v.write(ROT_IAR_REG_SP, &dato_mqtt);
        break;
      case 'P':
        //kp_h = dato_mqtt * 100;
        rotador_v.write(ROT_IAR_REG_KP, &dato_mqtt);
        break;
      case 'I':
        //ki_h = dato_mqtt * 100;
        rotador_v.write(ROT_IAR_REG_KI, &dato_mqtt);
        break;
      case 'D':
        //kd_h = dato_mqtt * 100;
        rotador_v.write(ROT_IAR_REG_KD, &dato_mqtt);
        break;
      case 'S':           // STOP
        Serial.println("-> Detenido");
        dato_mqtt = 29555;  // 'ss'
        rotador_v.write(ROT_IAR_REG_STOP, &dato_mqtt);
        mqttClient.publish("rotador/estado_v","PARADO");
        estado_motor_v = STOP;
        break;
      case 'A':           // Movimiento antihorario
        Serial.println("-> Movimiento antihorario...");
        rotador_v.write(ROT_IAR_REG_ANTIHOR, &velocidad_v);
        mqttClient.publish("rotador/estado_v","MOVIMIENTO ANTIHORARIO");
        estado_motor_v = ANTIHORARIO;
        break;
      case 'H':           // Movimiento horario
        Serial.println("-> Movimiento horario...");
        rotador_v.write(ROT_IAR_REG_HORARIO, &velocidad_v);
        mqttClient.publish("rotador/estado_v","MOVIMIENTO HORARIO");
        estado_motor_v = HORARIO;
        break;
      case 'Z':           // Set cero
        Serial.println("-> Buscando cero grados...");
        dato_mqtt = 31354;  // 'zz'
        rotador_v.write(ROT_IAR_REG_SETCERO, &dato_mqtt);
        mqttClient.publish("rotador/estado_v","SET CERO");
        estado_motor_v = CERO;
        break;
      case 'V':           // Set velocidad
        velocidad_v = (dato_mqtt/100) * V_MAX / 100;
        Serial.print("-> Velocidad horizontal: ");
        Serial.println(velocidad_v);
        break ; 
      case 'X': 
        dato_mqtt = 31354;  // 'zz'
        rotador_v.write(ROT_IAR_REG_SETMAX, &dato_mqtt);    // cambie el nombre del registro a SETMAX
        break ;         
      /*** Otros casos ***/
      default:
        Serial.println("-> Comando incorrecto :(");
    }
  }
  if (!mqttClient.connected())    // Reviso comunicacion con el broker mqtt
  {
    Serial.println("-> Reconectando mqtt...");
    reconnect();
  }
  if (millis() - previousMillis > DELAY_MQTT)
  {
    //if (estado_motor_h != STOP)     // Si el motor se esta moviendo
    //{                               // Pido el valor del angulo y lo envio por mqtt
    rotador_h.read(ROT_IAR_REG_ANGULO, &angulo_h);
    Serial.print("-> Angulo h: ");Serial.print(angulo_h);
    sprintf(angulo_str,"%.2f",angulo_h);
    mqttClient.publish("rotador/angulo_h",angulo_str);

    rotador_v.read(ROT_IAR_REG_ANGULO, &angulo_v);
    Serial.print("-> Angulo v: ");Serial.println(angulo_v);
    sprintf(angulo_str,"%.2f",angulo_v);
    mqttClient.publish("rotador/angulo_v",angulo_str);
    //}
    // Toggle led - Alive test
    (digitalRead(LED2_PIN)) ? digitalWrite(LED2_PIN, LOW) : digitalWrite(LED2_PIN, HIGH);
    previousMillis = millis();
  }
  mqttClient.loop();
  //delay(200);
}
/* ============= FUNCIONES ===================================================== */
/**
 * @brief Calback MQTT
 * 
 * @param topic : topico mqtt
 * @param payload : payload mqtt
 * @param length : longitud del payload
*/
void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("-> Mensaje en topico [");
  Serial.print(topic);
  Serial.print("] : ");
  String topic_str(topic); // Topico a String
  String string_aux;       // Payload a String
  for (unsigned int i = 1; i < length; i++)
  {
    string_aux += (char)payload[i];
  }
  comando = (char)payload[0];
  dato_mqtt = string_aux.toFloat() * 100;
  Serial.print(comando);
  Serial.print(dato_mqtt);
  flag_mqtt = true;
  Serial.println();
}

/**
 * @brief Reconexion MQTT
*/
void reconnect()
{
  // Loop until we're reconnected
  while (!mqttClient.connected())
  {
    Serial.print("--> Attempting MQTT connection...");
    // Attempt to connect
    if (mqttClient.connect(CLIENT_ID))
    {
      Serial.println("--> connected");
    }
    else
    {
      Serial.print("--> failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
/**
 * @brief Pasa de float a string
 * 
 * @param val : valor float
 * @param dato : puntero al string resultante
*/
void ftostr(float val, char *dato)
{
  uint8_t centena = (uint8_t) (val / 100);
  uint8_t decena = (uint8_t) (val - centena * 100) / 10;
  uint8_t unidad = (uint8_t) (val - centena * 100 - decena * 10);
  //uint8_t decimal;
  dato[0] = centena + 48;
  dato[1] = decena + 48;
  dato[2] = unidad + 48;
  //dato[3] = '.';
  //dato[4] = decimal + 48;
  dato[3] = '\0';
}

void command_received_topic(){ 

}



/* ============================================================================= */
// EOF