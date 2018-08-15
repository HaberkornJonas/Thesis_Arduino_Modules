//*************************************************************************
//Author : Jonas Haberkorn
//File : Delivery module
//Project : Mehrzweck-Drohne Thesis - FHNW
//Comment : The first generated message after a start up will not 
//          be transmitted to the GSC by the APM, because 
//          it is used to synchronise the controller with the 
//          APM and isn't always received entirely.
//Copyright : Jonas Haberkorn - P6 FHNW 2018
//*************************************************************************

#include <NewPing.h>

#define MODULE_NR 3
#define PWM_DIGITAL_PIN 6
#define PWM_CLOSED_VALUE 1000 //fully closed
#define PWM_OPEN_VALUE 1800 //fully open
#define ULTRASONIC_TRIGGER_PIN 3 // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ULTRASONIC_ECHO_PIN 2 // Arduino pin tied to echo pin on the ultrasonic sensor.
#define ULTRASONIC_MAX_DISTANCE 20 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.
#define ULTRASONIC_PRESENCE_DISTANCE 5 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

int message_nr = 0;

int pwm_value = 0;
unsigned int echo_distance = 0;
bool packet_detected = false;



NewPing sonar(ULTRASONIC_TRIGGER_PIN, ULTRASONIC_ECHO_PIN, ULTRASONIC_MAX_DISTANCE); // NewPing setup of pins and maximum distance.

void setup() {
  Serial.begin(57600);   // opens serial port, sets data rate to 57600 to communicate with UART2 from APM
  Serial.setTimeout(7);  //set the timeout between 2 messages input (200ms for 3.3Hz,70ms for 10Hz, 15ms for 50Hz, 7ms for 100Hz)
  pinMode(ULTRASONIC_TRIGGER_PIN, OUTPUT);
  pinMode(ULTRASONIC_ECHO_PIN, INPUT);
  EstablishingConnection();
}

void loop() {
  SendMessage(GeneratingMessage());
  message_nr++;
}

void EstablishingConnection() {
   while (Serial.available() <= 0) {
    Serial.print("$");
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
  
  UpdatePresenceStatus();

  if(packet_detected)
    message += "yes";
  else
    message += "no";
  message += ":/:";
  message += GetPinStatus();
  message += ":/:";
  message += echo_distance;
  message += ":/:";
  message += pwm_value;
  message += ":;\n";
  return message;
}

void UpdatePresenceStatus()
{
  echo_distance = sonar.ping() / US_ROUNDTRIP_CM;
  if(echo_distance <= ULTRASONIC_PRESENCE_DISTANCE && echo_distance != 0)
    packet_detected = true;
  else
    packet_detected = false;
}

String GetPinStatus()
{
  pwm_value = pulseIn(PWM_DIGITAL_PIN, HIGH);
  if(pwm_value < PWM_CLOSED_VALUE)
    return "closed";
  if(pwm_value > PWM_OPEN_VALUE)
    return "open";
  return "ajar";
    
}

