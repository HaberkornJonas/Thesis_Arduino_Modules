//*************************************************************************
//Author : Jonas Haberkorn
//File : Meteorological module
//Project : Mehrzweck-Drohne Thesis - FHNW
//Comment : The first generated message after a start up will not 
//          be transmitted to the GSC by the APM, because 
//          it is used to synchronise the controller with the 
//          APM and isn't always received entirely.
//Copyright : Jonas Haberkorn - P6 FHNW 2018
//*************************************************************************

#include <SimpleDHT.h>

#define MODULE_NR 4
#define TEMPERATURE_HUMIDITY_DIGITAL_PIN 2
#define LUMINOSITY_ANALOG_PIN 7

int message_nr = 0;

bool input_pump_state = false;
bool pump_state = false;

SimpleDHT11 temperature_and_humidity_sensor;
byte temperature = 0; // in °C
byte humidity = 0; // in %
byte raw_data[40] = {0};
int luminosity = 0; // dark = 1023, light = 0

void setup() {
  Serial.begin(57600);   // opens serial port, sets data rate to 57600 to communicate with UART2 from APM
  Serial.setTimeout(7);  //set the timeout between 2 messages input (200ms for 3.3Hz,70ms for 10Hz, 15ms for 50Hz, 7ms for 100Hz)
  pinMode(TEMPERATURE_HUMIDITY_DIGITAL_PIN, INPUT);
  pinMode(LUMINOSITY_ANALOG_PIN, INPUT);
  EstablishingConnection();
}

void loop() {
  SendMessage(GeneratingMessage());
  message_nr++;
}

void EstablishingConnection() {
   while (Serial.available() <= 0) {
    Serial.print("a");
    delay(200);
  }
  Serial.readString();
}

void SendMessage(String message)
{
  int x = 0;
  for(; message.charAt(x)!='\0'; x++)
  {
    Serial.print(message.charAt(x));
    while (Serial.available() <= 0) {}
    Serial.readString();
  }
  Serial.print(message.charAt(x));
    while (Serial.available() <= 0) {}
    Serial.readString();
}

String GeneratingMessage(){
  
  String message;
  message += ":";
  message += MODULE_NR;
  message += ":/:";
  message += message_nr;
  message += ":/:";
  
  UpdateTemperatureAndHumidity();
  message += temperature;
  message += ":/:";
  message += humidity;
  message += ":/:";

  UpdateLuminosity();
  message += luminosity;
  message += ":;\n";
  return message;
}

void UpdateTemperatureAndHumidity()
{
  temperature_and_humidity_sensor.read(TEMPERATURE_HUMIDITY_DIGITAL_PIN, &temperature, &humidity, raw_data);
}

void UpdateLuminosity()
{
  luminosity = analogRead(LUMINOSITY_ANALOG_PIN);
}

