//*************************************************************************
//Author : Jonas Haberkorn
//File : Spotlight module first prototype
//Project : Mehrzweck-Drohne Thesis - FHNW
//Comment : The first generated message after a start up will not 
//          be transmitted to the GSC by the APM, because 
//          it is used to synchronise the controller with the 
//          APM and isn't always received entirely.
//Copyright : Jonas Haberkorn - P6 FHNW 2018
//*************************************************************************

#define MODULE_NR 1
#define SPOT_DIGITAL_PIN 2
#define PWM_DIGITAL_PIN 5

int message_nr = 0;

bool input_spotlight_state = false;
bool spotlight_state = false;
int pwm_value = 0;

void setup() {
  Serial.begin(57600);   // opens serial port, sets data rate to 57600 to communicate with UART2 from APM
  Serial.setTimeout(7);  //set the timeout between 2 messages input (200ms for 3.3Hz,70ms for 10Hz, 15ms for 50Hz, 7ms for 100Hz)
  pinMode(SPOT_DIGITAL_PIN, OUTPUT);
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
  CheckingRead(Serial.readString());
}

void SendMessage(String message)
{
  int x = 0;
  for(; message.charAt(x)!='\0'; x++)
  {
    Serial.print(message.charAt(x));
    while (Serial.available() <= 0) {}
    CheckingRead(Serial.readString());
  }
  Serial.print(message.charAt(x));
    while (Serial.available() <= 0) {}
    CheckingRead(Serial.readString());
}

String GeneratingMessage(){
  
  String message;
  message += ":";
  message += MODULE_NR;
  message += ":/:";
  message += message_nr;
  message += ":/:";
  
  UpdateSpotlightState();
  if(spotlight_state)
    message += "on";
  else
    message += "off";
  message += ":/:";
  pwm_value = pulseIn(PWM_DIGITAL_PIN, HIGH);
  message += pwm_value;
  message += ":;\n";
  return message;
}

void CheckingRead(String readed)
{
  if(readed == "1")
    input_spotlight_state = true;
  else
    input_spotlight_state = false;
}

void UpdateSpotlightState()
{
  if(input_spotlight_state)
  {
    digitalWrite(SPOT_DIGITAL_PIN, HIGH);
    spotlight_state = true;
  }
  else
  {
    digitalWrite(SPOT_DIGITAL_PIN, LOW);
    spotlight_state = false;
  } 
}

