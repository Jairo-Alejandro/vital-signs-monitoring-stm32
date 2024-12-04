#include <Arduino.h>
#include <HardwareSerial.h>
#include <Wire.h>
#include "MAX30105.h"
#include "spo2_algorithm.h"
#include <OneWire.h>
#include <DallasTemperature.h>

// Instancia del sensor MAX30105
MAX30105 particleSensor;

// Buffers de datos para los LEDs rojo e infrarrojo
uint32_t irValue, redValue;

// Variables de cálculo para MAX30105
int32_t spo2;            // Saturación de oxígeno (SpO2)
int8_t validSPO2;        // Indicador de validez de SpO2
int32_t heartRate;       // Frecuencia cardíaca (HR)
int8_t validHeartRate;   // Indicador de validez de HR

// Pines para el AD8232
#define LO_PLUS_PIN PA3  // Conectar al pin L+ del AD8232
#define LO_MINUS_PIN PA4 // Conectar al pin L- del AD8232
#define ECG_PIN PA0       // Conectar el pin de salida del AD8232 (ECG)

// Pines para el OneWire (sensor de temperatura)
#define ONE_WIRE_BUS PA5

// Instancias de hardware
HardwareSerial Serial1(PA10, PA9); // RX, TX
TwoWire myWire(PB7, PB6);          // SDA, SCL
OneWire oneWire(ONE_WIRE_BUS);     // Instancia de comunicación OneWire
DallasTemperature sensors(&oneWire); // Instancia para el sensor de temperatura

void setup() {
    // Inicializa la comunicación UART
    Serial1.begin(9600);  // Comunicación para enviar los datos
    Serial.begin(9600);   // Comunicación para depuración
    myWire.begin();       // Inicializar el bus I²C

    // Intentar inicializar el sensor MAX30105
    if (!particleSensor.begin(myWire, I2C_SPEED_FAST)) {
        Serial1.println("{\"status\":\"ERROR\",\"message\":\"Sensor MAX30105 no encontrado. Revisa conexiones.\"}");
        while (1); // Detener el programa si no se encuentra el sensor
    } else {
        Serial1.println("{\"status\":\"OK\",\"message\":\"Sensor MAX30105 encontrado y listo.\"}");
    }

    // Configuración del sensor MAX30105
    byte ledBrightness = 60;   // Brillo de LED (0 a 255)
    byte sampleAverage = 4;    // Promedio de muestras
    byte ledMode = 2;          // Rojo + IR
    byte sampleRate = 100;     // Tasa de muestreo
    int pulseWidth = 411;      // Duración de pulso
    int adcRange = 4096;       // Rango del ADC
    particleSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange);

    // Configura los pines del AD8232
    pinMode(LO_PLUS_PIN, INPUT);  // Configura PA3 (LO+) como entrada
    pinMode(LO_MINUS_PIN, INPUT); // Configura PA4 (LO-) como entrada

    // Inicializar el sensor de temperatura Dallas
    sensors.begin();
}

void loop() {
    // Verificar si hay nuevos datos disponibles del MAX30105
    if (particleSensor.available()) {
        redValue = particleSensor.getRed(); // Leer el valor del LED rojo
        irValue = particleSensor.getIR();   // Leer el valor del LED infrarrojo

        // Calcular frecuencia cardíaca y SpO2
        maxim_heart_rate_and_oxygen_saturation(&irValue, 1, &redValue, &spo2, &validSPO2, &heartRate, &validHeartRate);

        // Crear el JSON que contiene los datos de todos los sensores
        String jsonData = "{";

        // Verificar desconexión de los electrodos del AD8232
        if ((digitalRead(LO_PLUS_PIN) == HIGH) || (digitalRead(LO_MINUS_PIN) == HIGH)) {
            jsonData += "\"ECG\":null,\"ECG_Status\":\"Electrodos desconectados\",";
        } else {
            // Si los electrodos están conectados, lee el valor de ECG desde A0
            int ecgValue = analogRead(ECG_PIN);
            jsonData += "\"ECG\":" + String(ecgValue) + ",";
        }

        // Incluir los datos del MAX30105 (HR y SpO2)
        if (validHeartRate && validSPO2) { // Solo incluir si los datos son válidos
            jsonData += "\"HR\":" + String(heartRate) + ",";
            jsonData += "\"SpO2\":" + String(spo2) + ",";
        } else {
            jsonData += "\"HR\":null,\"SpO2\":null,";
        }

        // Leer y añadir la temperatura desde el sensor Dallas
        sensors.requestTemperatures(); // Solicitar la temperatura
        float temperatureC = sensors.getTempCByIndex(0); // Obtener temperatura en °C
        jsonData += "\"Temperature\":" + String(temperatureC);

        jsonData += "}";

        // Enviar el JSON al ESP8266-01S
        Serial1.println(jsonData);
    }

    // Procesar datos continuamente, sin detener el bucle
    particleSensor.check();
}
