//*************************************************************************
//Author : Jonas Haberkorn
//File : Spotlight module
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
#define PWM_STROBOSCOPE_MIN_VALUE 1500 //below = always on/off
#define PWM_STROBOSCOPE_MAX_VALUE 2000 //above = SOS mode

int message_nr = 0;

bool input_spotlight_state = false;
bool spotlight_state = false;
int pwm_value = 0;
int index_frequencies = 0; // if = 0, then spotight always on
unsigned long frequencies[] = {0, 500, 650, 800, 1000, 1250, 1500}; 
unsigned long last_frequency_time = 0;
bool sos_status = false;
unsigned long sos_delays[] = {500, 100, 100, 100, 100, 100, 100, 500, 100, 500, 100, 500, 100, 100, 100, 100, 100, 100}; //3.1s
int sos_index = 0;
unsigned long last_sos_time = 0;

void setup() {
  Serial.begin(57600);   // opens serial port, sets data rate to 57600 to communicate with UART2 from APM
  Serial.setTimeout(7);  //set the timeout between 2 messages input (200ms for 3.3Hz,70ms for 10Hz, 15ms for 50Hz, 7ms for 100Hz)
  pinMode(SPOT_DIGITAL_PIN, OUTPUT);
  EstablishingConnection();
}

void loop() {
  if(sos_status && input_spotlight_state)
    SendSOS();
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
  
  pwm_value = pulseIn(PWM_DIGITAL_PIN, HIGH);
  UpdateFrequency();
  UpdateSpotlightState();

  if(spotlight_state)
    message += "on";
  else
    message += "off";
  message += ":/:";
  message += pwm_value;
  message += ":/:";
  if(sos_status)
    message += "SOS mode";
  else
    message += frequencies[index_frequencies];
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

void UpdateFrequency()
{
  if(pwm_value > PWM_STROBOSCOPE_MAX_VALUE)
  {
    sos_status = true;
    return;
  }
  else
    sos_status = false;
    
  int temp_pwm_value = pwm_value - PWM_STROBOSCOPE_MIN_VALUE;
  int pwm_value_interval = (PWM_STROBOSCOPE_MAX_VALUE - PWM_STROBOSCOPE_MIN_VALUE)/6;
  int i = 0;
  for(; temp_pwm_value >0; i++)
  {
    temp_pwm_value -= pwm_value_interval;
  }
  index_frequencies = i;
}

void UpdateSpotlightState()
{
  if(input_spotlight_state)
  {
    if(sos_status)
      return;
    UpdateSpotlightFrequencyState();
  }
  else
  {
    digitalWrite(SPOT_DIGITAL_PIN, LOW);
    spotlight_state = false;
  } 
}


void SendSOS()
{
  while(sos_index != 18)
  {
    Serial.print("~");
    if(millis()-last_sos_time > sos_delays[sos_index])
    {
      sos_index++;
      last_sos_time = millis();
      if(sos_index%2)
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
  }
  sos_index = 0;
}

void UpdateSpotlightFrequencyState()
{
  if(index_frequencies != 0 &&millis()-last_frequency_time > frequencies[index_frequencies])
  {
    last_frequency_time = millis();
    if(spotlight_state)
      digitalWrite(SPOT_DIGITAL_PIN, LOW);
    else
      digitalWrite(SPOT_DIGITAL_PIN, HIGH);
    spotlight_state = !spotlight_state;
  }
  else
  {
    if(index_frequencies == 0)
    {
      digitalWrite(SPOT_DIGITAL_PIN, HIGH);
      spotlight_state = true;
    }
  }
}

