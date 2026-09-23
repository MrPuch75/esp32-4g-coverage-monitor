#ifdef ESP8266
 #include <ESP8266WiFi.h>
#else
 #include <WiFi.h>
#endif
#include <ModbusIP_ESP8266.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

#define SDA 13  // Define SDA pin
#define SCL 14  // Define SCL pin

LiquidCrystal_I2C LCD = LiquidCrystal_I2C(0x27, 16, 2);


//Modbus Registers Offset (0-999)
const int REG = 4;               // Valor de los dB 4G
const int IMSI = 348;             // Valor de IMSI SIM

IPAddress remote(192, 168, 1, 1);  // IP del dispositivo Modbus Slave
const int LOOP_COUNT = 10;

ModbusIP mb;  // Objeto ModbusIP



// Logo de ejemplo (16x16)
const uint8_t logo[16] = {
  0b00000000, 0b01111100, 0b10000010, 0b10000010,
  0b10000010, 0b10000010, 0b10000010, 0b10000010,
  0b10000010, 0b10000010, 0b10000010, 0b10000010,
  0b10000010, 0b01111100, 0b00000000, 0b00000000
};

void showLogo() {
  for (int i = 0; i < 8; i++) {  // Solo 8 caracteres para un logo 8x8
    uint8_t logoChar[8];  // Crear un array temporal de 8 bytes
    for (int j = 0; j < 8; j++) {
      logoChar[j] = logo[i * 8 + j];  // Copiar los datos del logo al array temporal
    }
    LCD.createChar(i, logoChar);  // Crear el caracter en el LCD
    LCD.setCursor(i, 0);
    LCD.write(byte(i));  // Mostrar el caracter en la pantalla LCD
    delay(100);
  }
}

void showFailureMessage() {
  LCD.clear();
  LCD.setCursor(0, 1);
  LCD.print("Fallo Conexion");
  delay(3000);  // Mostrar el mensaje de fallo por 3 segundos
}

void setup() {
  Serial.begin(115200);

  LCD.init();   // Inicializar el LCD
  LCD.backlight();
  
  // Mostrar el logo al arrancar
  showLogo();
  delay(5000);  // Mostrar el logo por 5 segundos

  // Mostrar "Encendiendo Repetidor"
  LCD.clear();
  LCD.setCursor(0, 0);
  LCD.print("Encendiendo");
  LCD.setCursor(0, 1);
  LCD.print("Repetidor");
  delay(5000);
  
  
  // Mostrar "Encendiendo Router 4G" 
  LCD.clear();
  LCD.setCursor(0, 0);
  LCD.print("Encendiendo");
  LCD.setCursor(0, 1);
  LCD.print("Router 4G");
  delay(10000);

  // Mostrar Conectando a la Red
  LCD.clear();
  LCD.setCursor(0, 0);
  LCD.print("Conexion a WiFi");
  LCD.setCursor(0, 1);
  LCD.print("");

  for (int i = 0; i < 16; i++) {
    
    for (int j = 0; j <= i; j++) {
      LCD.setCursor(j, 1);  // Mover el cursor a la posición de la fila 1
      LCD.print(".");  // Mostrar un punto
    }
    delay(500);  // Esperar medio segundo antes de continuar
  }
  delay(1000);

  // Conectar a la red WiFi
  WiFi.begin("RUT240_6721", "b4BCn27A", 6);
  int wifiRetryCount = 0;
  int maxRetries = 50;  // Número máximo de intentos

  // Intentar conectarse a WiFi hasta maxRetries veces
  while (WiFi.status() != WL_CONNECTED && wifiRetryCount < maxRetries) {
    delay(250);  // Esperar 250 ms entre intentos
    wifiRetryCount++;
  }
  // Si no se conecta en el número máximo de intentos, mostrar mensaje de error en LCD
if (WiFi.status() != WL_CONNECTED) {
  LCD.clear();
  LCD.setCursor(0, 0);
  LCD.print("Error Conexion");
  LCD.setCursor(0, 1);
  LCD.print("Comprobar Router");
  delay(3000);  // Mostrar mensaje de error durante 3 segundos
  return;
} else {

  // Si se conecta exitosamente, mostrar mensaje de conexión
  LCD.clear();
  LCD.setCursor(0, 0);
  LCD.print("Conexion a WiFi");
  LCD.setCursor(0, 1);
  LCD.print("Conectado");
  delay(3000);  // Mostrar mensaje de éxito durante 3 segundos

  

  // Intentar conectar con el Modbus y leer el valor del registro WIFI
  uint16_t res = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(5000);  // Comprobación cada 5 segundos
  }

  
  
  mb.client();  // Conectar a Modbus Slave después de la conexión exitosa
}

}

void loop() {

  
  // El ciclo puede incluir tareas de lectura de Modbus, actualización de pantalla, etc.
  uint16_t res = 0;     // Variable para almacenar el valor recibido del registro Modbus dB
  uint16_t resIMSI = 0; // Variable para almacenar el valor recibido del registro Modbus IMSI
  
  // Comprobamos si estamos conectados al dispositivo Modbus
  if (mb.isConnected(remote)) {
  
  // Leer el primer registro (REG) y el segundo registro (IMSI)
    mb.readHreg(remote, REG, &res);   // Lee el valor de dB
    mb.readHreg(remote, IMSI, &resIMSI);  // Lee el valor de IMSI
  
  } else {
    mb.connect(remote);  // Intenta conectarse si no hay conexión
  }
    
  mb.task();            // Tarea Modbus común
  delay(10000);          // Intervalo de actualización

// Mostrar los valores de Modbus en el LCD si son válidos
  if (res == 0) {
    LCD.clear();
    LCD.setCursor(0, 0);
    LCD.print("Conectando 4G");
    LCD.setCursor(0, 1);
    LCD.print("");

  for (int i = 0; i < 16; i++) {
    
    for (int j = 0; j <= i; j++) {
      LCD.setCursor(j, 1);  // Mover el cursor a la posición de la fila 1
      LCD.print(".");  // Mostrar un punto
    }
    delay(500);  // Esperar medio segundo antes de continuar
    }
    
    delay(3000);
    // Salir de la función si no se obtuvo el valor correctamente
  }

  // Convertir ambos valores a números con signo
  int16_t signalValue = (int16_t)res;  // Convierte el valor a un número con signo
  int16_t ValorIMSI = (int16_t)resIMSI;  // Valor del segundo registro
  
// Si resIMSI es 0, significa que no hay SIM
  if (ValorIMSI == 0) {
    LCD.clear();
    LCD.setCursor(0, 0);
    LCD.print("Fallo SIM");
    LCD.setCursor(0, 1);
    LCD.print("Comprobar Router");
    delay(3000);  // Mostrar mensaje de fallo por 3 segundos

   } else if (ValorIMSI == 0 && signalValue == 0) {
  LCD.clear();
  LCD.setCursor(0, 0);
  LCD.print("Fallo Router");
  LCD.setCursor(0, 1);
  LCD.print("Comprobar Router");
  delay(3000);  // Mostrar el mensaje de fallo por 3 segundos

// Clasificar la cobertura
    } else if (signalValue == 0) {
      LCD.clear();
      LCD.setCursor(0, 0);
      LCD.print("Signal ERROR");
      LCD.setCursor(0, 1);
      LCD.print("X dBm");


    // Clasificar la cobertura
    } else if (signalValue >= -65) {
      LCD.clear();
      LCD.setCursor(0, 0);
      LCD.print("Signal 4G ALTA");
      LCD.setCursor(0, 1);
      LCD.print(signalValue);
      LCD.print(" dBm");
      
    } else if (signalValue >= -75) {
      LCD.clear();
      LCD.setCursor(0, 0);
      LCD.print("Signal 4G MEDIA");
      LCD.setCursor(0, 1);
      LCD.print(signalValue);
      LCD.print(" dBm");

    } else if (signalValue >= -92) {
      LCD.clear();
      LCD.setCursor(0, 0);
      LCD.print("Signal 4G BAJA");
      LCD.setCursor(0, 1);
      LCD.print(signalValue);
      LCD.print(" dBm");

      } else if (signalValue > -92) {
      LCD.clear();
      LCD.setCursor(0, 0);
      LCD.print("Signal 4G MALA");
      LCD.setCursor(0, 1);
      LCD.print("Reubica vehiculo");
  }
    delay(2000);  // Esperar 3 segundos antes de la siguiente iteración
  }
