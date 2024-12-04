#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>  
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
    String html = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Patient Monitoring System</title>
    <link href="https://cdn.jsdelivr.net/npm/chart.js" rel="stylesheet">
    <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.5.0/css/all.min.css">
    <style>
        :root {
            --background-dark: #1e293b;
            --card-dark: #334155;
            --text-dark: #f1f5f9;
            --background-light: #f8fafc;
            --card-light: #e2e8f0;
            --text-light: #1e293b;
            --accent: #0ea5e9;
            --success: #22c55e;
            --warning: #facc15;
            --danger: #ef4444;
        }

        body {
            margin: 0;
            font-family: 'Arial', sans-serif;
            background-color: var(--background-dark);
            color: var(--text-dark);
            transition: all 0.3s ease-in-out;
        }

        .dark-mode {
            --background-dark: #f8fafc;
            --card-dark: #e2e8f0;
            --text-dark: #1e293b;
        }

        .container {
            max-width: 1200px;
            margin: 0 auto;
            padding: 20px;
        }

        h1 {
            text-align: center;
            margin-bottom: 20px;
            font-size: 2rem;
        }

        .card-grid {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));
            gap: 20px;
        }

        .card {
            background-color: var(--card-dark);
            padding: 20px;
            border-radius: 10px;
            box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1);
            transition: all 0.3s ease-in-out;
            position: relative;
        }

        .card:hover {
            transform: scale(1.02);
        }

        .card h2 {
            margin: 0;
            font-size: 1.5rem;
            display: flex;
            align-items: center;
        }

        .card .icon {
            font-size: 2rem;
            margin-right: 10px;
            color: var(--accent);
        }

        .card .value {
            font-size: 2.5rem;
            font-weight: bold;
            margin: 10px 0;
        }

        .card .status {
            font-weight: bold;
        }

        .status.normal {
            color: var(--success);
        }

        .status.warning {
            color: var(--warning);
        }

        .status.danger {
            color: var(--danger);
        }

        .chart-container {
            margin-top: 20px;
            background-color: var(--card-dark);
            padding: 20px;
            border-radius: 10px;
            box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1);
        }

        button {
            position: fixed;
            top: 10px;
            right: 10px;
            background-color: var(--accent);
            color: white;
            border: none;
            padding: 10px 20px;
            border-radius: 5px;
            cursor: pointer;
            transition: all 0.3s ease-in-out;
            z-index: 10;
        }

        button:hover {
            background-color: #0284c7;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>Patient Monitoring System</h1>
        <div class="card-grid">
            <!-- Card for Heart Rate -->
            <div class="card">
                <h2><i class="fas fa-heartbeat icon"></i>Heart Rate</h2>
                <p class="value" id="hr">75.5 bpm</p>
                <p class="status normal" id="hr-status">Normal</p>
            </div>

            <!-- Card for SpO2 -->
            <div class="card">
                <h2><i class="fas fa-lungs icon"></i>SpO₂</h2>
                <p class="value" id="spo2">95.35%</p>
                <p class="status normal" id="spo2-status">Normal</p>
            </div>

            <!-- Card for Temperature -->
            <div class="card">
                <h2><i class="fas fa-thermometer-half icon"></i>Temperature</h2>
                <p class="value" id="temperature">36.89 °C</p>
                <p class="status normal" id="temp-status">Normal</p>
            </div>
        </div>
        <div class="chart-container">
            <h2><i class="fas fa-chart-line icon"></i>ECG Monitor</h2>
            <canvas id="ecgChart" width="400" height="200"></canvas>
        </div>
    </div>
    <button id="toggleMode"><i class="fas fa-adjust"></i> Toggle Mode</button>

    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
    <script src="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.5.0/js/all.min.js"></script>
    <script>
        // Chart configuration
        const ctx = document.getElementById('ecgChart').getContext('2d');
        const ecgChart = new Chart(ctx, {
            type: 'line',
            data: {
                labels: Array(100).fill(''),
                datasets: [{
                    label: 'ECG (mV)',
                    data: Array(100).fill(0),
                    borderColor: 'rgba(75, 192, 192, 1)',
                    borderWidth: 2,
                    tension: 0.1
                }]
            },
            options: {
                animation: { duration: 0 },
                responsive: true,
                scales: {
                    x: { display: false },
                    y: { beginAtZero: false }
                }
            }
        });

        // WebSocket connection
        const socket = new WebSocket('ws://' + window.location.hostname + ':81/');
        socket.onmessage = function (event) {
            const data = JSON.parse(event.data);
            document.getElementById('hr').textContent = `${data.HR} bpm`;
            document.getElementById('spo2').textContent = `${data.SpO2}%`;
            document.getElementById('temperature').textContent = `${data.Temperature} °C`;

            // Update status
            updateStatus('hr-status', data.HR, 60, 100);
            updateStatus('spo2-status', data.SpO2, 95, 100);
            updateStatus('temp-status', data.Temperature, 36.1, 37.2);

            ecgChart.data.datasets[0].data.push(data.ECG);
            if (ecgChart.data.datasets[0].data.length > 100) {
                ecgChart.data.datasets[0].data.shift();
            }
            ecgChart.update();
        };

        function updateStatus(elementId, value, min, max) {
            const element = document.getElementById(elementId);
            if (value < min) {
                element.textContent = 'Low';
                element.className = 'status danger';
            } else if (value > max) {
                element.textContent = 'High';
                element.className = 'status warning';
            } else {
                element.textContent = 'Normal';
                element.className = 'status normal';
            }
        }

        // Dark/Light mode toggle
        const toggleModeButton = document.getElementById('toggleMode');
        toggleModeButton.addEventListener('click', () => {
            document.body.classList.toggle('dark-mode');
        });
    </script>
</body>
</html>


)rawliteral";

    client.print(html);
}
