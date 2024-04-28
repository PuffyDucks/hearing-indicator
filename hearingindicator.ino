#define SAMPLE_SIZE 1000

#include "Arduino_LED_Matrix.h"
#include "amogus.h"
#include "icons.h"

#define AMOGUS_PIN 8
#define NO_SOUND_PIN 7
#define NORMAL_MODE 0
#define AMOGUS_MODE 1
const int mic = A0;

#define THRESHOLD 210
const int amogus_delay = 30;
const int normal_delay = 55;
int delay_duration = normal_delay;

ArduinoLEDMatrix matrix;
int mode = NORMAL_MODE;
int framecount = sizeof(icons) / 12;
int current_frame = 0;

float mic_val_raw[SAMPLE_SIZE];
float rms_mic;
float mic_threshold = THRESHOLD;

// calcualtes rms value of array
float rms(float* num_array) {
  float sum = 0.0;
  float avg = 0.0;

  for (int i = 0; i < SAMPLE_SIZE; i++) {
    sum += pow(num_array[i], 2);
  }
  avg = sum / SAMPLE_SIZE;
  return pow(avg, 0.5);
}

// checks for change in pin voltages
void check_mode() {
  int new_mode;
  if (digitalRead(AMOGUS_PIN)) {
    new_mode = AMOGUS_MODE;
    delay_duration = amogus_delay;
  } else {
    new_mode = NORMAL_MODE;
    delay_duration = normal_delay;
  }
  if (new_mode != mode) {
    mode = new_mode;
    current_frame = 0;
    
    if (mode == AMOGUS_MODE) {
      framecount = sizeof(amogus) / 12;
      matrix.loadFrame(amogus[current_frame]);
      current_frame++;

    } else {
      framecount = sizeof(icons) / 12; 
    }
  }

  if(digitalRead(NO_SOUND_PIN)) {
    mic_threshold = -999;
  } else {
    mic_threshold = THRESHOLD;
  }
}

void setup() {
  // initialize serial
  Serial.begin(115200);

  //initialize microphone input
  pinMode(mic, INPUT);
  pinMode(AMOGUS_PIN, INPUT);
  pinMode(NO_SOUND_PIN, INPUT);

  if(digitalRead(NO_SOUND_PIN)) {
    delay_duration = 0;
  }

  // initialize LED matrix output
  matrix.begin();
  check_mode();
  // matrix.loadFrame();
  current_frame++;
}

void loop() {
  for (int i = 0; i < SAMPLE_SIZE; i++) {
    mic_val_raw[i] = analogRead(mic);
  }
  rms_mic = rms(mic_val_raw);
  Serial.println(rms_mic);
  check_mode();
  if (rms_mic >= mic_threshold) {
    // Serial.println("loud");
    if (mode == AMOGUS_MODE) {
      matrix.loadFrame(amogus[current_frame]);
      // increment frame count and loop if needed
      current_frame++;
      if (current_frame == framecount) {
        current_frame = 0;
      }
    } else {
      matrix.loadFrame(icons[0]);
    }
  } else {
    if (mode == NORMAL_MODE) {
      matrix.loadFrame(icons[1]);
    }
  }
  delay(delay_duration);
}
