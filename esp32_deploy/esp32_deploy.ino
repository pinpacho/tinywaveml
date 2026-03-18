#include <eloquent_tensorflow32.h>
#include <utility.h>
#include <exception.h>

#include <eloquent.h>


#include <tflm_esp32.h>

#include <eloquent_tinyml.h>
// sine_model.h contains the array you exported from the previous step
// with either xxd or tinymlgen


#include "model.h"


#define NUMBER_OF_INPUTS 1
#define NUMBER_OF_OUTPUTS 1
// in future projects you may need to tweak this value.
// it's a trial and error process
#define TENSOR_ARENA_SIZE 2*1024

Eloquent::TinyML::TfLite<1, 1, 10000> ml;

void setup() {
    Serial.begin(115200);
}

void loop() {
    // pick up a random x and predict its sine
    float x = 3.14 * random(100) / 100;
    float y = sin(x);
    float input[1] = { x };
    float predicted = ml.predict(input);

    Serial.print("sin(");
    Serial.print(x);
    Serial.print(") = ");
    Serial.print(y);
    Serial.print("\t predicted: ");
    Serial.println(predicted);
    delay(1000);
}
