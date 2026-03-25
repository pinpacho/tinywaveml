#include <TensorFlowLite_ESP32.h>

#include "tensorflow/lite/micro/all_ops_resolver.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/schema/schema_generated.h"

// 🔥 TU MODELO
#include "model.h"

// ---------------- CONFIG ----------------
constexpr int kTensorArenaSize = 8 * 1024;
uint8_t tensor_arena[kTensorArenaSize];

constexpr int kInferencesPerCycle = 100;
constexpr float kXrange = 2.0f * 3.1416f;

// ---------------- VARIABLES ----------------
tflite::ErrorReporter* error_reporter = nullptr;
const tflite::Model* model = nullptr;
tflite::MicroInterpreter* interpreter = nullptr;

TfLiteTensor* input = nullptr;
TfLiteTensor* output = nullptr;

int inference_count = 0;

// ---------------- OUTPUT HANDLER ----------------
void HandleOutput(float x, float y) {
  Serial.print("x: ");
  Serial.print(x);
  Serial.print(" -> y: ");
  Serial.println(y);
}

// ---------------- SETUP ----------------
void setup() {
  Serial.begin(115200);

  // Error reporter
  static tflite::MicroErrorReporter micro_error_reporter;
  error_reporter = &micro_error_reporter;

  // Cargar modelo
  model = tflite::GetModel(g_model);
  if (model->version() != TFLITE_SCHEMA_VERSION) {
    Serial.println("Modelo incompatible!");
    return;
  }

  // Resolver ops
  static tflite::AllOpsResolver resolver;

  // Interpreter
  static tflite::MicroInterpreter static_interpreter(
      model, resolver, tensor_arena, kTensorArenaSize, error_reporter);

  interpreter = &static_interpreter;

  // Reservar memoria
  if (interpreter->AllocateTensors() != kTfLiteOk) {
    Serial.println("AllocateTensors() fallo");
    return;
  }

  // Tensores
  input = interpreter->input(0);
  output = interpreter->output(0);

  Serial.println("Modelo listo!");
}

// ---------------- LOOP ----------------
void loop() {

  // Generar entrada (tipo seno)
  float position = (float)inference_count / kInferencesPerCycle;
  float x = position * kXrange;

  // 🔥 CUANTIZACIÓN (IMPORTANTE si es INT8)
  int8_t x_q = x / input->params.scale + input->params.zero_point;
  input->data.int8[0] = x_q;

  // Ejecutar modelo
  if (interpreter->Invoke() != kTfLiteOk) {
    Serial.println("Invoke fallo");
    return;
  }

  // Obtener salida
  int8_t y_q = output->data.int8[0];
  float y = (y_q - output->params.zero_point) * output->params.scale;

  // Mostrar resultado
  HandleOutput(x, y);

  // Incrementar contador
  inference_count++;
  if (inference_count >= kInferencesPerCycle)
    inference_count = 0;

  delay(100);
}