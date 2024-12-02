import sys
import serial
import time
import json
import matplotlib.pyplot as plt
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
from PyQt5.QtWidgets import QApplication, QMainWindow, QVBoxLayout, QWidget, QLabel, QHBoxLayout
from PyQt5.QtCore import QTimer

# Configurar el puerto serial
ser = serial.Serial('COM8', 9600)  # Cambia 'COM3' por tu puerto correcto
ser.flushInput()

# Listas para almacenar los datos
time_stamps = []
hr_data = []
spo2_data = []
ecg_data = []

# Crear la figura de los gráficos
fig, (ax1, ax2, ax3) = plt.subplots(3, 1, figsize=(8, 8))
fig.tight_layout(pad=3.0)

# Configurar los gráficos
ax1.set_title('Heart Rate (HR)')
ax1.set_ylim(0, 200)
ax2.set_title('SpO2 (%)')
ax2.set_ylim(0, 100)
ax3.set_title('ECG Signal')
ax3.set_ylim(-2000, 2000)

# Colores para los gráficos
hr_color = 'red'
spo2_color = 'blue'
ecg_color = 'green'

class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()

        self.setWindowTitle("Real-Time Data from ESP8266")
        self.setGeometry(100, 100, 900, 650)

        # Crear el widget central y el layout
        self.central_widget = QWidget(self)
        self.setCentralWidget(self.central_widget)
        layout = QVBoxLayout(self.central_widget)

        # Etiqueta de texto
        self.label = QLabel("Gráficas en tiempo real: HR, SpO2, ECG", self)
        self.label.setStyleSheet("font-size: 20px; font-weight: bold; text-align: center;")
        layout.addWidget(self.label)

        # Crear un layout horizontal para mostrar los valores numéricos
        value_layout = QHBoxLayout()

        # Etiquetas para los valores numéricos
        self.hr_label = QLabel("HR: 0", self)
        self.spo2_label = QLabel("SpO2: 0%", self)
        self.ecg_label = QLabel("ECG: 0", self)

        # Estilo de las etiquetas numéricas
        self.hr_label.setStyleSheet("font-size: 18px; color: red; font-weight: bold;")
        self.spo2_label.setStyleSheet("font-size: 18px; color: blue; font-weight: bold;")
        self.ecg_label.setStyleSheet("font-size: 18px; color: green; font-weight: bold;")

        # Agregar las etiquetas a la interfaz
        value_layout.addWidget(self.hr_label)
        value_layout.addWidget(self.spo2_label)
        value_layout.addWidget(self.ecg_label)

        # Agregar el layout de los valores al layout principal
        layout.addLayout(value_layout)

        # Crear el canvas de matplotlib y agregarlo al layout
        self.canvas = FigureCanvas(fig)
        layout.addWidget(self.canvas)

        # Configurar un temporizador para actualizar los gráficos
        self.timer = QTimer(self)
        self.timer.timeout.connect(self.update_graph)
        self.timer.start(1000)  # Actualizar cada 1000 ms (1 segundo)

    def read_serial_data(self):
        if ser.in_waiting > 0:
            line = ser.readline().decode('utf-8').strip()
            try:
                data = json.loads(line)  # Se espera que los datos lleguen en formato JSON
                hr = data.get("HR", None)
                spo2 = data.get("SpO2", None)
                ecg = data.get("ECG", None)

                if hr is not None and spo2 is not None and ecg is not None:
                    # Agregar los datos a las listas
                    time_stamp = time.strftime("%H:%M:%S")
                    time_stamps.append(time_stamp)
                    hr_data.append(hr)
                    spo2_data.append(spo2)
                    ecg_data.append(ecg)

                    # Limitar el número de datos a mostrar en los gráficos
                    if len(time_stamps) > 20:
                        time_stamps.pop(0)
                        hr_data.pop(0)
                        spo2_data.pop(0)
                        ecg_data.pop(0)

                    # Actualizar las etiquetas con los valores más recientes
                    self.hr_label.setText(f"HR: {hr}")
                    self.spo2_label.setText(f"SpO2: {spo2}%")
                    self.ecg_label.setText(f"ECG: {ecg}")

            except json.JSONDecodeError:
                print("Error al decodificar los datos JSON")

    def update_graph(self):
        # Leer los datos del puerto serial
        self.read_serial_data()

        # Limpiar los gráficos y dibujarlos de nuevo
        ax1.clear()
        ax2.clear()
        ax3.clear()

        ax1.plot(time_stamps, hr_data, label="HR (bpm)", color=hr_color)
        ax2.plot(time_stamps, spo2_data, label="SpO2 (%)", color=spo2_color)
        ax3.plot(time_stamps, ecg_data, label="ECG", color=ecg_color)

        # Configuración de los gráficos
        ax1.set_title('Heart Rate (HR)')
        ax1.set_ylim(0, 200)
        ax2.set_title('SpO2 (%)')
        ax2.set_ylim(0, 100)
        ax3.set_title('ECG Signal')
        ax3.set_ylim(-2000, 2000)

        ax1.legend()
        ax2.legend()
        ax3.legend()

        # Redibujar el gráfico en la interfaz
        self.canvas.draw()

# Crear la aplicación y la ventana principal
app = QApplication(sys.argv)
window = MainWindow()
window.show()

# Ejecutar el loop de la aplicación
sys.exit(app.exec_())
