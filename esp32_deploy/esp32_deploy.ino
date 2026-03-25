#include <TensorFlowLite_ESP32.h>
#include "model.h"

// TFLite
#include "tensorflow/lite/micro/all_ops_resolver.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/schema/schema_generated.h"

// =============================
// PWM LED CONFIG
// =============================
#define LED_PIN 2        // LED integrado (puedes cambiar)
#define PWM_CH 0
#define PWM_FREQ 5000
#define PWM_RES 8        // 0–255

// =============================
// Configuración memoria
// =============================
constexpr int kTensorArenaSize = 20 * 1024;
uint8_t tensor_arena[kTensorArenaSize];

// =============================
// Objetos TFLite
// =============================
tflite::MicroInterpreter* interpreter;
TfLiteTensor* input;
TfLiteTensor* output;
tflite::ErrorReporter* error_reporter = nullptr;

// =============================
// Variables
// =============================
float x = 0.0;
const float step = 0.05;

// =============================
// SETUP
// =============================
void setup() {
  Serial.begin(115200);
  delay(2000);

  // PWM setup
  ledcSetup(PWM_CH, PWM_FREQ, PWM_RES);
  ledcAttachPin(LED_PIN, PWM_CH);

  Serial.println("Inicializando modelo...");

  const tflite::Model* model = tflite::GetModel(g_model);

  if (model->version() != TFLITE_SCHEMA_VERSION) {
    Serial.println("Modelo incompatible!");
    while (1);
  }

  static tflite::MicroErrorReporter micro_error_reporter;
  error_reporter = &micro_error_reporter;

  static tflite::AllOpsResolver resolver;

  static tflite::MicroInterpreter static_interpreter(
      model,
      resolver,
      tensor_arena,
      kTensorArenaSize,
      error_reporter
  );

  interpreter = &static_interpreter;

  if (interpreter->AllocateTensors() != kTfLiteOk) {
    Serial.println("Fallo en AllocateTensors()");
    while (1);
  }

  input = interpreter->input(0);
  output = interpreter->output(0);

  Serial.println("Sistema listo");
}

// =============================
// LOOP
// =============================
void loop() {

  if (x > 2 * PI) {
    x = 0;
  }

  float y_pred = 0;

  // =============================
  // INFERENCIA
  // =============================
  input->data.f[0] = x;

  if (interpreter->Invoke() != kTfLiteOk) {
    Serial.println("Error en inferencia");
    return;
  }

  y_pred = output->data.f[0];

  // =============================
  // CONVERSIÓN A PWM
  // =============================
  float y_pwm = (y_pred + 1.0) * 0.5 * 255.0;

  // saturación por seguridad
  if (y_pwm > 255) y_pwm = 255;
  if (y_pwm < 0) y_pwm = 0;

  int pwm_value = (int)y_pwm;

  // aplicar al LED
  ledcWrite(PWM_CH, pwm_value);
  float y_real = sin(x);
  // =============================
  // DEBUG (opcional)
  // =============================
  Serial.print(y_real);
  Serial.print(" ");
  Serial.println(y_pred);

  x += step;

  delay(20);
}