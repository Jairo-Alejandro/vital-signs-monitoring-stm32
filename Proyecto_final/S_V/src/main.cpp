#include <Arduino.h>
#include <HardwareSerial.h>
#include <Wire.h>
#include "MAX30105.h"
#include "spo2_algorithm.h"
#include <OneWire.h>
#include <DallasTemperature.h>

// Instancia del sensor MAX30105
MAX30105 particleSensor;

// Tamaño mínimo del buffer para cálculo continuo
const int bufferLength = 25; // Ventana pequeña para cálculo
uint32_t irBuffer[bufferLength];
uint32_t redBuffer[bufferLength];
int bufferIndex = 0;

// Variables de cálculo para MAX30105
int32_t spo2;            // Saturación de oxígeno (SpO2)
int8_t validSPO2;        // Indicador de validez de SpO2
int32_t heartRate;       // Frecuencia cardíaca (HR)
int8_t validHeartRate;   // Indicador de validez de HR

// Pines para el AD8232
#define LO_PLUS_PIN PA3  // Conectar al pin L+ del AD8232
#define LO_MINUS_PIN PA4 // Conectar al pin L- del AD8232
#define ECG_PIN PA0      // Conectar el pin de salida del AD8232 (ECG)

// Pines para el OneWire (sensor de temperatura)
#define ONE_WIRE_BUS PA5

// Instancias de hardware
HardwareSerial Serial1(PA10, PA9); // RX, TX
TwoWire myWire(PB7, PB6);          // SDA, SCL
OneWire oneWire(ONE_WIRE_BUS);     // Instancia de comunicación OneWire
DallasTemperature sensors(&oneWire); // Instancia para el sensor de temperatura

void setup() {
    // Inicializa la comunicación UART
    Serial1.begin(9600); // Comunicación para enviar los datos
    myWire.begin();      // Inicializar el bus I²C

    // Intentar inicializar el sensor MAX30105
    if (!particleSensor.begin(myWire, I2C_SPEED_FAST)) {
        Serial1.println("{\"Error\":\"MAX30105 no inicializado\"}");
        while (1);
    }

    // Configuración del sensor MAX30105
    byte ledBrightness = 100;   // Brillo de LED
    byte sampleAverage = 4;     // Promedio de muestras (optimizado)
    byte ledMode = 2;           // Rojo + IR
    byte sampleRate = 100;      // Tasa de muestreo
    int pulseWidth = 411;       // Duración de pulso
    int adcRange = 4096;        // Rango del ADC
    particleSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange);

    // Configura los pines del AD8232
    pinMode(LO_PLUS_PIN, INPUT);
    pinMode(LO_MINUS_PIN, INPUT);

    // Inicializar el sensor de temperatura Dallas
    sensors.begin();

    // Inicializar buffers en cero
    memset(irBuffer, 0, sizeof(irBuffer));
    memset(redBuffer, 0, sizeof(redBuffer));
}

void loop() {
    // Verificar si hay nuevos datos disponibles del MAX30105
    if (particleSensor.check()) {
        // Almacenar valores en los buffers
        redBuffer[bufferIndex] = particleSensor.getRed();
        irBuffer[bufferIndex] = particleSensor.getIR();
        bufferIndex++;

        // Procesar datos cuando el buffer está lleno
        if (bufferIndex == bufferLength) {
            maxim_heart_rate_and_oxygen_saturation(
                irBuffer, bufferLength,
                redBuffer,
                &spo2, &validSPO2,
                &heartRate, &validHeartRate
            );

            bufferIndex = 0; // Reiniciar el índice
        }

        // Crear el JSON para enviar datos
        String jsonData = "{";

        // Verificar desconexión de los electrodos del AD8232
        if ((digitalRead(LO_PLUS_PIN) == HIGH) || (digitalRead(LO_MINUS_PIN) == HIGH)) {
            jsonData += "\"ECG\":null,\"ECG_Status\":\"Electrodos desconectados\",";
        } else {
            // Leer el valor de ECG desde A0
            int ecgValue = analogRead(ECG_PIN);
            jsonData += "\"ECG\":" + String(ecgValue) + ",";
        }

        // Incluir los datos del MAX30105
        if (validHeartRate && validSPO2) {
            jsonData += "\"HR\":" + String(heartRate) + ",";
            jsonData += "\"SpO2\":" + String(spo2) + ",";
        } else {
            jsonData += "\"HR\":null,\"SpO2\":null,";
        }

        // Leer y añadir la temperatura desde el sensor Dallas
        sensors.requestTemperatures();
        float temperatureC = sensors.getTempCByIndex(0);
        if (temperatureC != DEVICE_DISCONNECTED_C) {
            jsonData += "\"Temperature\":" + String(temperatureC, 2);
        } else {
            jsonData += "\"Temperature\":null";
        }

        jsonData += "}";

        // Enviar datos en formato JSON por UART
        Serial1.println(jsonData);
    }
}
