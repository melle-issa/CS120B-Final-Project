#include <dht11.h>
#include <SevSegShift.h>
const int tempSensor = A1;
dht11 DHT11;
enum Modes{H, C, F}state = F;
const int tempBut = A0;
const int humBut = A2;
const int fanBut = A3;
const int SHIFT_PIN_SHCP = 6;
const int SHIFT_PIN_STCP = 5;
const int SHIFT_PIN_DS = 4;
double printF;
double lastRead = 0.0;
double value = 0;
double answer = 0;
int speed = 70;
int motorPin = 3;
bool turnOff = false;

SevSegShift sevseg(SHIFT_PIN_DS, SHIFT_PIN_SHCP, SHIFT_PIN_STCP, 1, true);

void setup(){
  int reading = DHT11.read(tempSensor);
  answer = (DHT11.temperature * 1.8) + 32;
  Serial.begin(9600);
  pinMode(tempBut, INPUT);
  pinMode(humBut, INPUT);
  byte numDigits = 4;
  byte digitPins[ ] = {12, 11, 10, 9};
  byte segmentPins[ ] = {0, 1, 2, 3, 4, 5, 6, 7}; 
  bool resistorsOnSegments = false;
  byte hardwareConfig = COMMON_CATHODE;
  bool updateWithDelays = true;
  bool leadingZeros = false;
  bool disableDecPoint = false; 

  sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments, updateWithDelays, leadingZeros, disableDecPoint);
  sevseg.setBrightness(10); //100?

  pinMode (motorPin, OUTPUT);
}

void reset(){
  digitalWrite(tempBut, LOW);
  digitalWrite(humBut, LOW);
}

void tempReading(){
  switch(state){
    case F:
      if(digitalRead(tempBut) == HIGH){
        state = C;
      }
      else if(digitalRead(humBut) == HIGH){
        state = H;
      }
      break;
    case C:
      if(digitalRead(tempBut) == HIGH){
        state = F;
      }
      else if(digitalRead(humBut) == HIGH){
        state = H;
      }
      break;
    case H:
      if(digitalRead(tempBut) == HIGH){
        state = F;
      }
      break;
  }
  switch(state){
    case F:
      printF = DHT11.temperature;
      printF = (printF * 1.8) + 32; 
      value = printF;
      if(value > 80 && !turnOff){
        digitalWrite(motorPin, speed); //analog?
      }
      else{
        digitalWrite(motorPin, 0);
      }
      break;
    case C:
      value = DHT11.temperature;
      if(value > 27 && !turnOff){
        digitalWrite(motorPin, speed);
      }
      else{
        digitalWrite(motorPin, 0);
      }
      break;
    case H:
      value = DHT11.humidity;
      if(value > 40 && !turnOff){
        digitalWrite(motorPin, speed);
      }
      else{
        digitalWrite(motorPin, 0);
      }
      break;
  }
  if(lastRead != value){
    answer = value;
    lastRead = value;
  }
}

void segDisplay(){
  sevseg.setNumber(answer);
  sevseg.refreshDisplay();
}

void fan(){
  if(turnOff){
    turnOff = false;
  }
  else{
    turnOff = true;
  }
}

void loop(){
  segDisplay();
  if(digitalRead(tempBut) == HIGH || digitalRead(humBut) == HIGH){
    tempReading();
  }
  if(digitalRead(fanBut) == HIGH){
    fan();
  }
}