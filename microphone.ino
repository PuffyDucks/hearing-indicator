#include <rgb_lcd.h>

rgb_lcd lcd;
//const int DISPLAY_RGB_ADDR = 0x62; this is true for v4
//const int DISPLAY_TEXT_ADDR = 0x3e;

bool state;
const bool QUIET = true;
const bool LOUD = false;

const int lcd_scl = A5;
const int lcd_sda = A4;

const int mic = A0;
int mic_val_raw_size = 32;
float mic_val_raw[32];
float rms_mic;

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

  for (int i = 0; i < mic_val_raw_size; i++) {
    sum += pow(num_array[i], 2);
  }
  avg = sum / mic_val_raw_size;
  return pow(avg, 0.5);
}

void setup() {
  // put your setup code here, to run once:
  // inputs: microphone
  // outputs: LCD

  // initialize serial
  Serial.begin(9600);

  // initialize lcd
  pinMode(lcd_scl, OUTPUT);
  pinMode(lcd_sda, INPUT);
  //lcd.begin(16,2);
  //lcd.print(message);

  //initialize microphone
  state = QUIET;
  pinMode(mic, INPUT);
  // later: wifi
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(500); //wait half a second
  for (int i; i < mic_val_raw_size; i++) {
    mic_val_raw[i] = analogRead(mic);
  }
  rms_mic = rms(mic_val_raw);
  Serial.println(rms_mic);
  if (state == QUIET) {
    //lcd.display();
    Serial.println(message);
  }
  else if (state == LOUD) {
    //lcd.noDisplay();
    Serial.println("loud");
  }

}
