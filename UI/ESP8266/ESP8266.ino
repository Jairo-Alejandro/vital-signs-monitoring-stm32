#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>  // Asegúrate de usar esta biblioteca para WebSocket en lugar de WebSocket_Generic.h

#include <ArduinoJson.h>

// Configura tu red Wi-Fi
const char* ssid = "John SENA";
const char* password = "alejo0425";

// Servidor WebSocket
WebSocketsServer webSocket = WebSocketsServer(81);

// Servidor HTTP en el puerto 80
WiFiServer server(80);

// Variables para almacenar los datos recibidos
String verificationMessage = "Esperando datos...";
float hr = 0.0;
float spo2 = 0.0;
float temperature = 0.0;
int ecgValue = 0;

// Variables para graficar ECG
const int maxEcgDataPoints = 100; // Número máximo de puntos en la gráfica
int ecgData[maxEcgDataPoints];    // Datos del ECG
int ecgIndex = 0;                 // Índice para guardar datos en el buffer

void setup() {
    // Inicialización de comunicación serial
    Serial.begin(9600); // Comunicación con STM32 y para depuración

    // Conexión a Wi-Fi
    WiFi.begin(ssid, password);
    Serial.print("Conectando a WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println("\nConectado a WiFi");
    Serial.print("Dirección IP: ");
    Serial.println(WiFi.localIP());

    // Inicializar WebSocket
    webSocket.begin();
    webSocket.onEvent(webSocketEvent); // Asegúrate de definir la función de evento
    Serial.println("Servidor WebSocket iniciado en el puerto 81.");

    // Inicializar servidor HTTP
    server.begin();
    Serial.println("Servidor HTTP iniciado en el puerto 80.");
}

void loop() {
    // Procesar eventos de WebSocket
    webSocket.loop();

    // Procesar solicitudes del servidor HTTP
    WiFiClient client = server.available();
    if (client) {
        String request = "";
        while (client.connected()) {
            if (client.available()) {
                char c = client.read();
                request += c;
                if (request.indexOf("\r\n\r\n") != -1) break;
            }
        }

        // Si la solicitud es para la página principal
        if (request.indexOf("GET / ") != -1) {
            serveHomePage(client);
        }
        client.stop();
    }

    // Verificar si hay datos desde la STM32
    if (Serial.available()) {
        String data = Serial.readStringUntil('\n'); // Leer datos hasta el salto de línea

        // Parsear los valores del JSON recibido
        DynamicJsonDocument doc(256);
        DeserializationError error = deserializeJson(doc, data);

        if (!error) {
            // Comprobar estado de verificación
            if (doc.containsKey("status")) {
                verificationMessage = doc["message"].as<String>();
            }

            // Actualizar los valores
            if (doc.containsKey("ECG")) {
                ecgValue = doc["ECG"];
                ecgData[ecgIndex] = ecgValue;
                ecgIndex = (ecgIndex + 1) % maxEcgDataPoints;
            }

            if (doc.containsKey("HR")) hr = doc["HR"];
            if (doc.containsKey("SpO2")) spo2 = doc["SpO2"];
            if (doc.containsKey("Temperature")) temperature = doc["Temperature"];
        }

        // Enviar datos al cliente WebSocket
        sendSensorData();
    }
}

// Manejo de eventos de WebSocket
void webSocketEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t length) {
    if (type == WStype_CONNECTED) {
        Serial.println("Cliente conectado.");
    } else if (type == WStype_DISCONNECTED) {
        Serial.println("Cliente desconectado.");
    }
}

// Función para enviar los datos de los sensores en formato JSON
void sendSensorData() {
    DynamicJsonDocument json(512);
    json["verification"] = verificationMessage;
    json["HR"] = hr;
    json["SpO2"] = spo2;
    json["Temperature"] = temperature;
    json["ECG"] = ecgValue;

    String jsonString;
    serializeJson(json, jsonString);
    webSocket.broadcastTXT(jsonString);  // Enviar datos a todos los clientes WebSocket conectados
}

// Función para servir la página HTML
void serveHomePage(WiFiClient client) {
    String html = "<!DOCTYPE html>"
                  "<html>"
                  "<head>"
                  "<title>Datos en Tiempo Real</title>"
                  "<script src='https://cdn.jsdelivr.net/npm/chart.js'></script>"
                  "</head>"
                  "<body style='font-family: Arial, sans-serif;'>"
                  "<h1>Datos Recibidos desde STM32</h1>"
                  "<p><strong>Estado de Verificación:</strong> <span id='verification'>Esperando datos...</span></p>"
                  "<p><strong>HR:</strong> <span id='hr'>0.0</span></p>"
                  "<p><strong>SpO2:</strong> <span id='spo2'>0.0</span></p>"
                  "<p><strong>Temperature:</strong> <span id='temperature'>0.0</span> °C</p>"
                  "<p><strong>ECG Actual:</strong> <span id='ecgValue'>0.0</span> mV</p>"
                  "<canvas id='ecgChart' width='400' height='200'></canvas>"
                  "<script>"
                  "let ecgData = [];"
                  "let ctx = document.getElementById('ecgChart').getContext('2d');"
                  "let chart = new Chart(ctx, {"
                  "    type: 'line',"
                  "    data: {"
                  "        labels: Array(100).fill(''),"
                  "        datasets: [{"
                  "            label: 'ECG (mV)',"
                  "            data: ecgData,"
                  "            borderColor: 'rgba(75, 192, 192, 1)',"
                  "            borderWidth: 1,"
                  "            fill: false,"
                  "            tension: 0.1"
                  "        }]"
                  "    },"
                  "    options: {"
                  "        animation: { duration: 0 },"
                  "        responsive: true,"
                  "        scales: { x: { display: false }, y: { beginAtZero: false } },"
                  "        maintainAspectRatio: false"
                  "    }"
                  "});"
                  "let socket = new WebSocket('ws://' + window.location.hostname + ':81/');"
                  "socket.onmessage = function(event) {"
                  "    let data = JSON.parse(event.data);"
                  "    document.getElementById('verification').textContent = data.verification;"
                  "    document.getElementById('hr').textContent = data.HR;"
                  "    document.getElementById('spo2').textContent = data.SpO2;"
                  "    document.getElementById('temperature').textContent = data.Temperature;"
                  "    document.getElementById('ecgValue').textContent = data.ECG;"
                  "    ecgData.push(data.ECG);"
                  "    if (ecgData.length > 100) {"
                  "        ecgData.shift();"
                  "        chart.update(0);"
                  "    } else {"
                  "        chart.update(0);"
                  "    }"
                  "};"
                  "</script>"
                  "</body>"
                  "</html>";

    client.print(html);
}
