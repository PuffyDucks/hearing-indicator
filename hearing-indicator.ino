#define SAMPLE_SIZE 1000
#define QUIET true
#define LOUD false
#define threshold 0.7

#include "Arduino_LED_Matrix.h"
#include "amogus.h"
#include "icons.h"

#define AMOGUS_PIN 8
#define NORMAL_MODE 0
#define AMOGUS_MODE 1
const int mic = A0;

const int amogus_delay = 0;
const int normal_delay = 70;
int delay_duration = normal_delay;

ArduinoLEDMatrix matrix;
int mode = NORMAL_MODE;
int framecount = sizeof(icons) / 12;
int current_frame = 0;

bool last[5];
bool volume;

float mic_val_raw[SAMPLE_SIZE];
float rms_mic;
float sampled_avgs[10];
float loudest_bg;
float myDB;

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

float max_of_10(float* averages) {
  float max = 0;
  for (int i = 0; i < 10; i++) {
    if (averages[i] > max) max = averages[i]; 
  }
  return max;
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
}

void setup() {
  // initialize serial
  Serial.begin(115200);

  //initialize microphone input
  pinMode(mic, INPUT);
  pinMode(AMOGUS_PIN, INPUT);

  // initialize LED matrix output
  matrix.begin();
  check_mode();
  // matrix.loadFrame();
  current_frame++;

  //sample background noise
  for (int j = 0; j < 10; j++) {
    for (int i = 0; i < SAMPLE_SIZE; i++) {
      mic_val_raw[i] = analogRead(mic);
    }
    if (mode == NORMAL_MODE) {
      delay(10);
    }
  sampled_avgs[j] = rms(mic_val_raw);
  }
  loudest_bg = max_of_10(sampled_avgs);
  Serial.println(loudest_bg);

  for (int i = 0; i < 5; i++) {
    last[i] = QUIET;
  }
}

void loop() {
  check_mode();
  delay(delay_duration);

  int numAverages;
  if (mode == NORMAL_MODE) {
    numAverages = 10;
  } else {
    numAverages = 1;
  }
  for (int j = 0; j < numAverages; j++) {
    for (int i = 0; i < SAMPLE_SIZE; i++) {
      mic_val_raw[i] = analogRead(mic);
    }
    delay(10);
  rms_mic = rms(mic_val_raw);
  sampled_avgs[j] = rms_mic;
  }
  rms_mic = max_of_10(sampled_avgs);
  myDB = 20 * log10(rms_mic / loudest_bg);
  Serial.println(myDB);
  if (myDB < threshold) volume = QUIET;
  else volume = LOUD;

  // add value to last measured
  for (int i = 0; i < 4; i++) {
    last[i] = last[i + 1];
  }
  last[4] = volume;

  int numQuiets = 0;
  for (int i = 0; i  < 5; i++) {
    if (last[i] == QUIET) numQuiets++;
  }
  Serial.println(numQuiets);
  if (numQuiets >= 4) {
    // show icon for cannot hear
    if (mode == NORMAL_MODE) {
      matrix.loadFrame(icons[1]);
    }
  }
  else {
    if (mode == AMOGUS_MODE) {
      matrix.loadFrame(amogus[current_frame]);
      // increment frame count and loop if needed
      current_frame++;
      if (current_frame == framecount) {
        current_frame = 0;
      }
    } else {
      // show icon for able to hear
      matrix.loadFrame(icons[0]);
    }
  }
}
