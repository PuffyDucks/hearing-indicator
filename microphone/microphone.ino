#include <rgb_lcd.h>
#define SAMPLE_SIZE 1000
#define QUIET true
#define LOUD false

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

float max_of_5(float* averages) {
  float max = 0;
  for (int i = 0; i < 5; i++) {
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
  sampled_avgs[j] = rms(mic_val_raw);
  }
  loudest_bg = max_of_5(sampled_avgs);
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(500); //wait half a second
  for (int i; i < SAMPLE_SIZE; i++) {
    mic_val_raw[i] = analogRead(mic);
  }
  rms_mic = rms(mic_val_raw);
  Serial.println(rms_mic);
  if (volume == QUIET) {
    //lcd.display();
    Serial.println(message);
  }
  else if (volume == LOUD) {
    //lcd.noDisplay();
    Serial.println("loud");
  }

}
