#include <Arduino.h>
#include <HardwareSerial.h>
#include <Wire.h>
#include "MAX30105.h"
#include "spo2_algorithm.h"

// Instancia del sensor MAX30105
MAX30105 particleSensor;

// Buffers de datos para los LEDs rojo e infrarrojo
uint32_t irBuffer[100];  // Datos del LED infrarrojo
uint32_t redBuffer[100]; // Datos del LED rojo

// Variables de cálculo
int32_t bufferLength;    // Longitud del buffer
int32_t spo2;            // Saturación de oxígeno (SpO2)
int8_t validSPO2;        // Indicador de validez de SpO2
int32_t heartRate;       // Frecuencia cardíaca (HR)
int8_t validHeartRate;   // Indicador de validez de HR

// Instancia manual de Serial1 en los pines PA9 (TX) y PA10 (RX)
HardwareSerial Serial1(PA10, PA9); // RX, TX
TwoWire myWire(PB7, PB6); // SDA, SCL

void setup() {
    // Inicializa UART para la comunicación con el ESP8266
    Serial1.begin(9600);
    myWire.begin(); // Inicializar el bus I²C en los pines PB7 y PB6
    
    // Intentar inicializar el sensor MAX30105
    if (!particleSensor.begin(myWire, I2C_SPEED_FAST)) {
        Serial1.println("{\"status\":\"ERROR\",\"message\":\"Sensor MAX30105 no encontrado. Revisa conexiones.\"}");
        while (1); // Detener el programa si no se encuentra el sensor
    } else {
        Serial1.println("{\"status\":\"OK\",\"message\":\"Sensor MAX30105 encontrado y listo.\"}");
    }

    // Configuración del sensor
    byte ledBrightness = 60;   // Brillo de LED (0 a 255)
    byte sampleAverage = 4;    // Promedio de muestras
    byte ledMode = 2;          // Rojo + IR
    byte sampleRate = 100;     // Tasa de muestreo
    int pulseWidth = 411;      // Duración de pulso
    int adcRange = 4096;       // Rango del ADC

    particleSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange);
}

void loop() {
    bufferLength = 100; // La longitud del buffer almacena 4 segundos de datos a 25 sps

    // Leer 100 muestras iniciales
    for (byte i = 0; i < bufferLength; i++) {
        while (particleSensor.available() == false) {
            particleSensor.check(); // Verificar si hay nuevos datos disponibles
        }

        redBuffer[i] = particleSensor.getRed(); // Leer el valor del LED rojo
        irBuffer[i] = particleSensor.getIR();   // Leer el valor del LED infrarrojo
        particleSensor.nextSample();            // Pasar a la siguiente muestra
    }

    // Calcular frecuencia cardíaca y SpO2
    maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);

    // Enviar los datos al ESP8266
    if (validHeartRate && validSPO2) { // Solo enviar si los datos son válidos
        Serial1.print("{\"HR\":");
        Serial1.print(heartRate);
        Serial1.print(",\"SpO2\":");
        Serial1.print(spo2);
        Serial1.println("}");
    } else {
        Serial1.println("{\"HR\":null,\"SpO2\":null}");
    }

    delay(1); // Esperar 1 segundo antes de tomar nuevas muestras
}
