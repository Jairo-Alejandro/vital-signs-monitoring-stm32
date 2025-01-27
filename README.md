# Estación Portátil para el Monitoreo de Signos Vitales

Este proyecto consiste en el desarrollo de una estación portátil para el monitoreo de signos vitales. El dispositivo permite medir el ritmo cardíaco, nivel de oxígeno en la sangre, un electrocardiograma (EKG) básico y la temperatura corporal. Está basado en un microcontrolador STM32 Black Pill y utiliza diversos sensores para capturar las señales biomédicas.

## Sensores Utilizados

- **MAX30105**: Sensor para medir el ritmo cardíaco y la saturación de oxígeno en sangre (SpO2).
- **AD8232**: Sensor para adquirir señales de electrocardiograma (EKG) básico.
- **DS18B20**: Sensor para medir la temperatura corporal.

## Características

- **Monitoreo de Ritmo Cardíaco**: Medición continua del ritmo cardíaco mediante el sensor MAX30105.
- **Monitoreo de SpO2**: Medición de la saturación de oxígeno en sangre utilizando el MAX30105.
- **Electrocardiograma Básico (EKG)**: Obtención de señales EKG básicas a través del sensor AD8232.
- **Temperatura Corporal**: Medición de la temperatura corporal con el sensor DS18B20.
- **Interfaz Gráfica**: Visualización de los datos en una interfaz gráfica, facilitando el análisis de las mediciones.
- **Carcasa Impresa en 3D**: Diseño compacto y portátil, fabricado con impresión 3D.

## Requisitos

- **Hardware**:
  - Tarjeta de desarrollo STM32 Black Pill.
  - Sensores MAX30105, AD8232 y DS18B20.
  - Componentes electrónicos adicionales (resistencias, cables, etc.).
  - Carcasa impresa en 3D.

- **Software**:
  - IDE de desarrollo para STM32 (como STM32CubeIDE).
  - Librerías necesarias para el funcionamiento de los sensores.
  - Interfaz gráfica (puede ser desarrollada en software como LabVIEW o similar).

## Instalación

1. Clona el repositorio:
   ```bash
   git clone https://github.com/Jairo-Alejandro/vital-signs-monitoring-stm32.git
