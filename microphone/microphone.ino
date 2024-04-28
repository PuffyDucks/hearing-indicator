#include <rgb_lcd.h>
#define SAMPLE_SIZE 1000
#define QUIET true
#define LOUD false
#define threshold 0

rgb_lcd lcd;
//const int DISPLAY_RGB_ADDR = 0x62; this is true for v4
//const int DISPLAY_TEXT_ADDR = 0x3e;

bool volume;

const int lcd_scl = A5;
const int lcd_sda = A4;

const int mic = A0;
float mic_val_raw[SAMPLE_SIZE];
float rms_mic;
float sampled_avgs[10];
float loudest_bg;
float myDB;

bool last[5];

const String message = "SPEAK UP I CAN'T HEAR YOU";

// int read_val() {
//   int soundValue = 0; //create variable to store many different readings
//   for (int i = 0; i < 32; i++) //create a for loop to read 
//   { soundValue += analogRead(sound_sensor);  } //read the sound sensor
 
//   soundValue >>= 5; //bitshift operation 
//   Serial.println(soundValue); //print the value of sound sensor
// }

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

void setup() {
  // put your setup code here, to run once:
  // inputs: microphone
  // outputs: LCD

  // initialize serial
  Serial.begin(115200);

  // initialize lcd
  pinMode(lcd_scl, OUTPUT);
  pinMode(lcd_sda, INPUT);
  //lcd.begin(16,2);
  //lcd.print(message);

  //initialize microphone
  pinMode(mic, INPUT);
  
  //sample background noise
  for (int j = 0; j < 10; j++) {
    for (int i = 0; i < SAMPLE_SIZE; i++) {
      mic_val_raw[i] = analogRead(mic);
    }
    delay(10);
  sampled_avgs[j] = rms(mic_val_raw);
  }
  loudest_bg = max_of_10(sampled_avgs);
  Serial.println(loudest_bg);

  for (int i = 0; i < 5; i++) {
    last[i] = QUIET;
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(250); //wait 1/4 a second
  for (int j = 0; j < 10; j++) {
    for (int i = 0; i < SAMPLE_SIZE; i++) {
      mic_val_raw[i] = analogRead(mic);
    }
    delay(10);
  rms_mic = rms(mic_val_raw);
  sampled_avgs[j] = rms_mic;
  }
  rms_mic = max_of_10(sampled_avgs);
  myDB = 20 * log10(rms_mic / loudest_bg);
  //Serial.println(rms_mic);
  //Serial.print(" ");
  Serial.println(myDB);
  if (myDB < threshold) volume = QUIET;
  else volume = LOUD;

  int numQuiets = 0;
  for (int i = 0; i  < 5; i++) {
    if (last[i] == QUIET) numQuiets++;
  }
  Serial.println(numQuiets);
  if (numQuiets >= 4) {
    // output quiet message
    Serial.println(message);
  }
  else {
    // output loud message
    Serial.println("loud");
  }

  // add value to last measured
  for (int i = 0; i < 4; i++) {
    last[i] = last[i + 1];
  }
  last[4] = volume;
}
