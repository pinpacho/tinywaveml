# Guía de Despliegue: Modelo de Inferencia de Onda Senoidal en ESP32

Este documento describe los pasos necesarios para implementar un modelo de Machine Learning (TinyML) entrenado en TensorFlow Lite dentro de un microcontrolador ESP32, utilizando el entorno de desarrollo de Arduino.

## 1. Requisitos del Entorno

Para asegurar la compatibilidad con las librerías de inferencia, se deben cumplir los siguientes requisitos:

* **IDE**: Arduino IDE 2.0 o superior.
* **Core ESP32**: Versión **2.0.11** (disponible en el Gestor de Tarjetas).
* **Librerías**: 
    * `TensorFlowLite_ESP32` (instalada desde el Gestor de Librerías).

## 2. Estructura de Archivos del Proyecto

El proyecto de Arduino debe contener al menos tres archivos en la misma carpeta para compilar correctamente:

1.  **`esp32_deploy.ino`**: Contiene la lógica de inicialización del intérprete y el bucle de inferencia.
2.  **`model.h`**: Define la interfaz para acceder a los datos binarios del modelo.
3.  **`model.cc`**: Contiene el arreglo hexadecimal generado por la herramienta `xxd`.

### 2.1 Configuración de model.h
Este archivo debe contener las declaraciones `extern` para que el compilador vincule el arreglo de bytes definido en el archivo `.cc`:

```cpp
#ifndef MODEL_H_
#define MODEL_H_

extern const unsigned char g_model[];
extern const int g_model_len;

#endif
```

### 2.2 Configuración de model.cc

Para integrar el modelo en Arduino, sigue estos pasos:

1. **Importación**: Copia el archivo `model.cc` generado en Python a la carpeta de tu sketch de Arduino. 
2. **Sincronización**: Abre el archivo y asegúrate de que el nombre del arreglo sea exactamente `g_model` y su longitud `g_model_len`. 
3. **Ubicación**: El archivo debe residir en el mismo directorio que el `.ino` y el `model.h` para que el compilador vincule los datos correctamente durante el despliegue.

> **Nota**: Si el nombre generado por `xxd` incluye rutas de carpetas, edítalo manualmente en el archivo de texto para que coincida con las declaraciones del encabezado.



### 3. Detalles de Implementación y Despliegue

Para que el modelo funcione correctamente en el hardware, el código de Arduino gestiona la memoria y los recursos de TensorFlow Lite siguiendo estos pilares:

#### 3.1 Librerías Esenciales
El código importa componentes críticos del motor de inferencia:
* **`TensorFlowLite_ESP32.h`**: La librería base para ejecutar modelos en el ESP32.
* **`"tensorflow/lite/micro/all_ops_resolver.h"`**: Carga las operaciones matemáticas (como sumas o activaciones ReLU) que el modelo necesita.
* **`"tensorflow/lite/micro/micro_interpreter.h"`**: El "cerebro" que procesa los datos de entrada a través de la red neuronal.

#### 3.2 Gestión de Memoria (Tensor Arena)
A diferencia de una PC, el ESP32 requiere que asignemos manualmente un espacio de memoria RAM para el trabajo del modelo:
* **`kTensorArenaSize`**: Hemos definido un tamaño de **20 KB** (`20 * 1024`).
* **`tensor_arena`**: Es el arreglo de bytes donde el intérprete almacena los cálculos intermedios. Es vital que este tamaño sea suficiente para la complejidad del modelo; de lo contrario, el sistema fallará al iniciar.

#### 3.3 Vinculación del Modelo (`g_model`)
Es fundamental que el nombre del modelo sea idéntico en todos los archivos del proyecto:
* **`g_model`**: Este es el nombre del arreglo hexadecimal que vive en `model.cc` y se declara en `model.h`.
* **Consistencia**: El comando `tflite::GetModel(g_model)` busca este nombre exacto. Si lo cambias en un archivo, debes cambiarlo en todos, de lo contrario el compilador no podrá vincular los pesos de la red neuronal.

#### 3.4 El Proceso de Inferencia
En cada iteración del `loop()`, el microcontrolador realiza lo siguiente:
1.  **Entrada**: El valor de `x` se asigna al tensor de entrada (`input->data.f[0] = x`).
2.  **Invocación**: Se ejecuta `interpreter->Invoke()`, procesando el dato por todas las capas.
3.  **Salida**: Se extrae el resultado predicho (`y_pred`) del tensor de salida.

#### 3.5 Conversión a PWM (LED)
Como el modelo entrega valores entre **-1.0 y 1.0**, aplicamos una fórmula para que el ESP32 controle el brillo del LED (rango 0 a 255):
* `y_pwm = (y_pred + 1.0) * 0.5 * 255.0`
* Se incluye una etapa de **saturación** para garantizar que el valor siempre esté dentro de los límites seguros del hardware.



## 4. Resultados y Verificación

Una vez cargado el programa, puedes validar el funcionamiento de dos formas:

### 4.1 Serial Plotter
Abre el **Ploter serie** (Herramientas > Ploter serie) a **115200 baudios**. Verás dos líneas superpuestas: la función matemática real y la predicción de tu red neuronal. Si el entrenamiento fue correcto, ambas señales serán casi idénticas.

![Imagen onda seno](https://i.imgur.com/Z6xUd4j.png)

### 4.2 Control de Hardware (PWM)
La predicción se traduce a una señal PWM que varía el brillo del LED (Pin 2). El resultado es un desvanecimiento suave que sigue el ritmo de la onda senoidal.

![Despliegue del modelo en ESP32](../images/led_ml.gif)

---

**¡Felicidades!** Has completado el flujo de TinyML: desde el entrenamiento en Python hasta la ejecución física en un ESP32.