//*************************************************************************
//Author : Jonas Haberkorn
//File : Sprinkler module calibration
//Project : Mehrzweck-Drohne Thesis - FHNW
//Comment : Use this program to calibrate the water level sensor. 
//          Enter in the macro "HIGHEST_WATER_LEVEL_VALUE" the value 
//          of the sensor when the tank is full. Enter in the macro 
//          "LOWEST_WATER_LEVEL_VALUE" the value of the sensor 
//          when the sensor is dry. 
//Copyright : Jonas Haberkorn - P6 FHNW 2018
//*************************************************************************

#define WATER_LEVEL_ANALOG_PIN 7 

void setup() {
  Serial.begin(57600);
  pinMode(WATER_LEVEL_ANALOG_PIN, INPUT);
}

void loop() {
  Serial.print("Water level sensor value: ");
  Serial.println(analogRead(WATER_LEVEL_ANALOG_PIN));
}

