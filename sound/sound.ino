#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN 6

#define NUM_LEDS 60

#define BRIGHTNESS 50

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);

int gamma[] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
    2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
    5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
   10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
   17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
   25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
   37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
   51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
   69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
   90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
  115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
  144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
  177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
  215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255 };



int soundPin = A1;
float sensorValue;

//parameters
const float filterValue1 = 0.995;
const float filterValue2 = 0.98;
const float filterValue3 = 0.985;

const float range = 15;
const float power = 0.5;
const float gain = 20;
const int bias =0;

const int nLED = 60;

//variables
int LEDBrightness;

void setup() {
  // put your setup code here, to run once:
  pinMode(soundPin, INPUT);
  Serial.begin(115200);

  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
  #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif
  // End of trinket special code
  strip.setBrightness(BRIGHTNESS);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

void loop() {
  // put your main code here, to run repeatedly:

//  Serial.println(LEDBrightness);
//  setAllLEDs(LEDBrightness);
  rainbow();
}

float lowPass1(float soundSignal){
  static float average1 = 1023.0/2.0;
  average1 = (average1*filterValue1) + ((1.0 - filterValue1)*soundSignal);
  return average1;
}

float lowPass2(float soundSignal){
  static float average2 = 1023.0/2.0;
  average2 = (average2*filterValue2) + ((1.0 - filterValue2)*soundSignal);
  return average2;
}

float lowPass3(float soundSignal){
  static float average3 = 0;
  average3 = (average3*filterValue3) + ((1.0 - filterValue3)*soundSignal);
  return average3;
}

float compress(float input){
  float output = pow(input/range, power)* range * gain;
  return output;
}

float soundAmplitude(float soundSignal){
  float amplitude = lowPass2(abs(soundSignal - lowPass1(soundSignal)));
  float lowPassAmplitude = lowPass3(amplitude);
  float output = amplitude - lowPassAmplitude;
  output += bias;
  if(output < 0){
    output = 0;
  }
  return compress(output);
}

int clamp(float input){
  int output = (int)input;
  if(output > 255){
    output = 255;
  }
  return output;
}

void setAllLEDs(int brightness){
  for(int i=0; i<nLED; i++){
    strip.setPixelColor(i, gamma[brightness], gamma[brightness], gamma[brightness]);
  }
  strip.show();
}

uint32_t Wheel(byte WheelPos, float brightness) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color((int)((float)(255 - WheelPos * 3)*brightness), 0, (int)((float)(WheelPos * 3)*brightness),0);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, (int)((float)(WheelPos * 3)*brightness), (int)((float)(255 - WheelPos * 3)*brightness),0);
  }
  WheelPos -= 170;
  return strip.Color((int)((float)(WheelPos * 3)*brightness), (int)((float)(255 - WheelPos * 3)*brightness), 0,0);
}

void rainbow() {
  uint16_t i, j;

  for(j=0; j<256; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      sensorValue = float(abs(analogRead(soundPin))); //asdf
      float LEDBrightness = (float)(clamp(soundAmplitude(sensorValue))); //asdf
      LEDBrightness /= 255;
      Serial.println(LEDBrightness);
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255, LEDBrightness));
    }
    strip.show();
  }
}





