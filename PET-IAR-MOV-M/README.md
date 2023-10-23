# PET-IAR-MOV-M
Firmware de la placa maestro del sistema de control de movimiento del Prototipo de Estación Terrena (PET).
La placa maestro funciona como una interfaz entre un servidor MQTT y las placas esclavo (*[repo PET-IAR-MOV-S](https://github.com/dariocapucchio/PET-IAR-MOV-S.git)*) que controlan los motores. La cominicación con el servidor de MQTT se realiza vía ethernet para publicar y recibir los datos de control del movimiento de la antena. Y la comunicación con las placas esclavo se raliza vía I2C. 

## Compilación
El proyecto utiliza el framework de arduino de la extensión PlatformIO.

## Hardware
En uso
- Raspberry pi pico
- ENC28J60 - Módulo para la comunicación vía ethernet con el servidor MQTT.

Con posibilidad de conexión a

- INA322 - Módulo para la medición de tensión y corriente.
- DS18B20 - Sensor de temperatura digital de -55°C a +125°C.