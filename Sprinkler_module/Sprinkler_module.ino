//*************************************************************************
//Author : Jonas Haberkorn
//File : Sprinkler module
//Project : Mehrzweck-Drohne Thesis - FHNW
//Comment : The first generated message after a start up will not 
//          be transmitted to the GSC by the APM, because 
//          it is used to synchronise the controller with the 
//          APM and isn't always received entirely.
//Copyright : Jonas Haberkorn - P6 FHNW 2018
//*************************************************************************

#define MODULE_NR 2
#define PUMP_DIGITAL_PIN 2
#define WATER_LEVEL_ANALOG_PIN 7 
#define WATER_PERCENT_NOT_MEASURED 55
#define LOWEST_WATER_LEVEL_VALUE 80
#define HIGHEST_WATER_LEVEL_VALUE 410

int message_nr = 0;

bool input_pump_state = false;
bool pump_state = false;
int water_level_value = 0;
int water_percent_left = 0;

void setup() {
  Serial.begin(57600);   // opens serial port, sets data rate to 57600 to communicate with UART2 from APM
  Serial.setTimeout(7);  //set the timeout between 2 messages input (200ms for 3.3Hz,70ms for 10Hz, 15ms for 50Hz, 7ms for 100Hz)
  pinMode(PUMP_DIGITAL_PIN, OUTPUT);
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

  UpdatePumpState();
  if(pump_state)
    message += "on";
  else
    message += "off";
  message += ":/:";

  UpdateWaterLevel();
  if(water_percent_left == -1)
  {
    message += "<";
    message += WATER_PERCENT_NOT_MEASURED;
  }
  else
    message += water_percent_left;
  message += ":/:";
  message += water_level_value;
  message += ":;\n";
  return message;
}

void CheckingRead(String readed)
{
  if(readed == "1")
    input_pump_state = true;
  else
    input_pump_state = false;
}


void UpdatePumpState()
{
  if(input_pump_state)
  {
    digitalWrite(PUMP_DIGITAL_PIN, HIGH);
    pump_state = true;
  }
  else
  {
    digitalWrite(PUMP_DIGITAL_PIN, LOW);
    pump_state = false;
  }
}

void UpdateWaterLevel()
{
  water_level_value = analogRead(WATER_LEVEL_ANALOG_PIN); 
  if(water_level_value <= LOWEST_WATER_LEVEL_VALUE)
    water_percent_left = -1;
  else
    water_percent_left = (100 - WATER_PERCENT_NOT_MEASURED) * (water_level_value - LOWEST_WATER_LEVEL_VALUE) / (HIGHEST_WATER_LEVEL_VALUE - LOWEST_WATER_LEVEL_VALUE) + WATER_PERCENT_NOT_MEASURED;
}

