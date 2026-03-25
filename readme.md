
# TinyWaveML: Inferencia de Onda Senoidal en ESP32

Este proyecto es una guía práctica de **TinyML** que recorre todo el ciclo de trabajo de un modelo de Inteligencia Artificial: desde el entrenamiento de una red neuronal que aprende la función seno, hasta su despliegue físico en un microcontrolador ESP32 para controlar un LED mediante PWM.

##  Cómo empezar

Puedes ejecutar este proyecto de dos maneras:

### Opción A: Google Colab (Recomendado)
Entrena y convierte el modelo en la nube sin instalar nada localmente:

[![Open In Colab](https://colab.research.google.com/assets/colab-badge.svg)](https://colab.research.google.com/github/pinpacho/tinywaveml/blob/main/hello_wave.ipynb)

### Opción B: Entorno Local (Python)
Si prefieres trabajar localmente, sigue estos pasos para configurar tu entorno virtual:

```bash
# Verificar versiones
python --version
pip --version

# Crear entorno virtual
python -m venv tinyhello

# Activar entorno (Linux/Mac)
source tinyhello/bin/activate

# Activar entorno (Windows)

 .\tinyhello\Scripts\Activate.ps1

# Instalar dependencias
pip install -r requirements.txt
```

-----

##  Flujo de Trabajo

El proyecto se divide en tres fases críticas:

1.  **Entrenamiento**: Usamos TensorFlow para crear una red neuronal con dos capas de 16 neuronas que aprende a predecir la curva de una función seno a partir de datos ruidosos.
2.  **Optimización y Conversión**: El modelo se convierte a formato **TFLite** . Esto reduce el tamaño del modelo y permite que el procesador del ESP32 realice cálculos más rápidos con números enteros.
3.  **Despliegue (C++)**: El modelo se exporta como un arreglo hexadecimal (`.cc`) y se integra en un sketch de Arduino que utiliza la librería `TensorFlowLite_ESP32`.



##  Despliegue en el ESP32

Para que el modelo funcione en tu hardware, debes incluir los siguientes archivos en tu proyecto de Arduino:

  * **`esp32_deploy.ino`**: El código que gestiona el **Tensor Arena (20 KB)** y ejecuta la inferencia.
  * **`model.h`**: Contiene la declaración `extern const unsigned char g_model[]`.
  * **`model.cpp`**: El archivo generado por `xxd` con los datos del modelo. **Asegúrate de que el nombre de la variable sea `g_model`.**

### Detalles Técnicos

  * **Librerías**: Requiere `TensorFlowLite_ESP32` y el core de ESP32 versión **2.0.11**.
  * **Inferencia**: El modelo recibe un valor `x` y predice `y`. Este valor se mapea al rango **0-255** para controlar el brillo del LED mediante `ledcWrite`.



##  Resultados Visuales

Al cargar el modelo, puedes abrir el **Serial Plotter** (115200 baudios) para ver la precisión de la IA frente a la función matemática real.

![Despliegue del modelo en ESP32](images/led_ml.gif)




## Contacto

Para cualquier pregunta o comentario, no dudes en ponerte en contacto :


Julian Francisco Pinchao Ortiz

* Correo: jfrancisco.pinchao@udea.edu.co

* GitHub:[@pinpacho](https://github.com/pinpacho)



##  Referencias y Recursos

Este proyecto se basa en las siguientes herramientas y documentación oficial de TinyML:

* **Librería TensorFlowLite_ESP32**: Puerto oficial de TensorFlow Lite Micro para la arquitectura ESP32. [Repositorio en GitHub](https://github.com/tanakamasayuki/Arduino_TensorFlowLite_ESP32)
* **Libro "TinyML: Machine Learning with TensorFlow Lite on Arduino"**: Guía fundamental de Pete Warden y Daniel Situnayake (O'Reilly, 2020) para el desarrollo de IA en sistemas embebidos. [Versión PDF](https://kolegite.com/EE_library/books_and_lectures/%D0%9F%D1%80%D0%BE%D0%B3%D1%80%D0%B0%D0%BC%D0%B8%D1%80%D0%B0%D0%BD%D0%B5/TinyML%20Machine%20Learning%20with%20TensorFlow%20Lite%20on%20Arduino%20and%20Ultra-Low-Power%20Microcontrollers%20%28Pete%20Warden%2C%20Daniel%20Situnayake%29%20%28Z-Library%29.pdf)
* **TensorFlow Lite para Microcontroladores**: Guía oficial de Google sobre el flujo de trabajo, conversión de modelos y optimización para hardware restringido. [Sitio Oficial](https://www.tensorflow.org/lite/microcontrollers?hl=es-419)
