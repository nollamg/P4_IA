# 📌 Proyecto: Sistema Embebido con FreeRTOS + IA (ESP32‑S3)

Este proyecto implementa un sistema embebido basado en **ESP32‑S3** utilizando **FreeRTOS** para gestionar tareas concurrentes, análisis de datos y visualización. Incluye una mejora de IA, comunicación entre tareas mediante colas y una simulación completa en **Wokwi**, permitiendo demostrar el funcionamiento sin hardware físico.

---

## 🚀 Características principales

- ✔ ESP32‑S3 ejecutando FreeRTOS (Arduino framework)  
- ✔ Lectura periódica del sensor **DHT22**  
- ✔ Comunicación entre tareas mediante **colas FreeRTOS**  
- ✔ Visualización en pantalla **OLED SSD1306**  
- ✔ Arquitectura modular con tareas independientes  
- ✔ Simulación completa en **Wokwi**  
- ✔ Tarea de IA con FFT, predicción y clasificación  

---

## 🧩 Arquitectura del sistema

El sistema está dividido en varias tareas FreeRTOS:

| Tarea | Función | Prioridad | Comunicación |
|------|---------|-----------|--------------|
| **TaskSensor** | Lee el DHT22 y envía temperatura | 2 | Cola `qTemperatura` |
| **TaskDisplay** | Muestra datos en la OLED | 1 | Recibe de `qTemperatura` |
| **Task_AI_Analytics** | Procesa datos (FFT, predicción, clasificación) | 2 | Recibe datos DSP / publica resultados |

La comunicación se realiza mediante **colas**, desacoplando productores y consumidores.

---

## 🛠️ Hardware simulado en Wokwi

El proyecto utiliza:

- **ESP32‑S3 DevKitC‑1**
- **Sensor DHT22**
- **Pantalla OLED SSD1306 (I2C)**

Todo está definido en el archivo `diagram.json`.

---

## ▶️ Simulación en Wokwi

Pasos para ejecutar la simulación:

1. Crear un nuevo proyecto en Wokwi con **ESP32‑S3 (Arduino)**  
2. Copiar los archivos:
   - `diagram.json`
   - `sketch.ino` (o `main.cpp`)
3. Pulsar **Run**  
4. Observar:
   - Lecturas del DHT22  
   - Logs de FreeRTOS en consola  
   - Actualización de la pantalla OLED  
   - Tareas ejecutándose en paralelo  

Este entorno permite grabar fácilmente un vídeo de 30 segundos para la entrega.

---

## 📂 Estructura del repositorio

/Proyecto-FreeRTOS-IA
│
├── diagram.json        # Simulación Wokwi
├── sketch.ino          # Código principal (Arduino + FreeRTOS)
├── README.md           # Documentación del proyecto
└── /docs               # Capturas, vídeo y documentación adicional

Codi

---

## 🤖 Mejora de IA implementada

La mejora de IA consiste en una tarea dedicada que:

- Procesa datos del consumo eléctrico  
- Calcula FFT y armónicos  
- Usa un modelo TFLite para identificar electrodomésticos  
- Predice consumo mensual  
- Genera recomendaciones de ahorro  

Se ejecuta como tarea independiente con prioridad 2.

---

## 🧪 Limitaciones actuales y mejoras futuras

- Algunas tareas usan un **stack mayor del necesario** → se puede optimizar con `uxTaskGetStackHighWaterMark()`.  
- La latencia aumenta cuando coinciden DSP + IA → se puede mejorar con prioridades dinámicas o dividiendo la IA en subtareas.  

---

## 📸 Vídeo de demostración

Incluye un vídeo de 20–30 segundos mostrando:

- Lectura del DHT22  
- Actualización de la OLED  
- Logs en tiempo real  
- Tareas FreeRTOS ejecutándose en paralelo  

---

## 📜 Licencia

Proyecto académico. Uso libre para aprendizaje y demostración.
