#include <Wire.h>               
#include "cubecell_SH1107Wire.h"
#include "LoRaWan_APP.h"
#include "Arduino.h"
#include <Timezone.h>
#include <TinyGPS++.h>
TinyGPSPlus gps;

extern SH1107Wire display;
/*
 * set LoraWan_RGB to 1,the RGB active in loraWan
 * RGB red means sending;
 * RGB green means received done;
 */
#ifndef LoraWan_RGB
#define LoraWan_RGB 0
#endif

#define RF_FREQUENCY                                915000000 // Hz

#define TX_OUTPUT_POWER                             22        // dBm

#define LORA_BANDWIDTH                              0         // [0: 125 kHz,
                                                              //  1: 250 kHz,
                                                              //  2: 500 kHz,
                                                              //  3: Reserved]
#define LORA_SPREADING_FACTOR                       7         // [SF7..SF12]
#define LORA_CODINGRATE                             1         // [1: 4/5,
                                                              //  2: 4/6,
                                                              //  3: 4/7,
                                                              //  4: 4/8]
#define LORA_PREAMBLE_LENGTH                        8         // Same for Tx and Rx
#define LORA_SYMBOL_TIMEOUT                         0         // Symbols
#define LORA_FIX_LENGTH_PAYLOAD_ON                  false
#define LORA_IQ_INVERSION_ON                        false


#define RX_TIMEOUT_VALUE                            1000
#define BUFFER_SIZE                                 100 // Define the payload size here

char txpacket[BUFFER_SIZE];
char rxpacket[BUFFER_SIZE];

static RadioEvents_t RadioEvents;

double txNumber;

extern HardwareSerial Serial;
extern HardwareSerial Serial1;

String received_data;  
String received_data1;
char string1[BUFFER_SIZE];
char *charMsgToSend;

//Timezone setup
TimeChangeRule NZST = {"NZST", Last, Sun, Apr, 4, 720};
TimeChangeRule NZDT = {"NZDT", Last, Sun, Sep, 26, 780};
Timezone NZ(NZST, NZDT);

// time variables
time_t local, utc, prev_set;
int timesetinterval = 60; //set microcontroller time every 60 seconds


double lontemp = -92.1234;
double lattemp = 31.8765;

char latt[9];
char longi[9];
char complete[BUFFER_SIZE];

// For stats that happen every 5 seconds
unsigned long last = 0UL;

int16_t rssi,rxSize;
void  DoubleToString( char *str, double double_num,unsigned int len);

void setup() {
    Serial.begin(115200);
    Serial1.begin(9600);

    txNumber=0;
    rssi=0;

    Radio.Init( &RadioEvents );
    Radio.SetChannel( RF_FREQUENCY );
    Radio.SetTxConfig( MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
                                   LORA_SPREADING_FACTOR, LORA_CODINGRATE,
                                   LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
                                   true, 0, 0, LORA_IQ_INVERSION_ON, 3000 ); 

    delay(100);

  smartDelay(1000);
  while (!gps.time.isValid()) // wait a valid GPS UTC time (valid is defined in the TinyGPS++ library)
  {
    smartDelay(1000);
  }
    setthetime();
    prev_set = now();
}



void loop()
{ 
  while (Serial1.available() > 0)
    gps.encode(Serial1.read());

  if (gps.location.isUpdated())
  {
    sendHeartbeat();
  }
  else if (millis() - last > 5000)
  {
    if (gps.location.isValid())
    {
      static const double LONDON_LAT = 51.508131, LONDON_LON = -0.128002;
      double distanceToLondon =
        TinyGPSPlus::distanceBetween(
          gps.location.lat(),
          gps.location.lng(),
          LONDON_LAT, 
          LONDON_LON);
      double courseToLondon =
        TinyGPSPlus::courseTo(
          gps.location.lat(),
          gps.location.lng(),
          LONDON_LAT, 
          LONDON_LON);

    if (gps.charsProcessed() < 10)
      Serial.println(F("WARNING: No GPS data.  Check wiring."));

    last = millis();
    Serial.println();
  }
}
if (now() - prev_set > timesetinterval && gps.time.isValid())  // set the microcontroller time every interval, only if there is a valid GPS time
  {
    setthetime();
    prev_set = now();
    Serial.print("time is set");
    smartDelay(1000);
  }
}

void sendHeartbeat()
{
  char buf[40];
  char m[4];    // temporary storage for month string (DateStrings.cpp uses shared buffer)
  TimeChangeRule *tcr;        // pointer to the time change rule, use to get the TZ abbrev

  utc = now();  // read the time in the correct format to change via the TimeChangeRules
  time_t t = NZ.toLocal(utc, &tcr);
  strcpy(m, monthShortStr(month(t)));
  sprintf(buf, "%s %02d %s %d\n%02d:%02d:%02d %s", dayShortStr(weekday(t)), day(t), m, year(t), hour(t), minute(t), second(t), tcr -> abbrev);

  uint16_t voltage = getBatteryVoltage();
  Serial.println(voltage);

  lontemp = gps.location.lng();
  lattemp = gps.location.lat();
  dtostrf(lontemp, 8, 4, longi);
  dtostrf(lattemp, 8, 4, latt);
  sprintf(complete, "%s\nlat(%s) long(%s)\nbat(%d)", buf, latt, longi, voltage);
  
  Radio.Send( (uint8_t *) complete, BUFFER_SIZE);
  Serial.println(complete);
}

static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (Serial1.available())
      gps.encode(Serial1.read());
  } while (millis() - start < ms);
}

void displayInfo()
{
  Serial.print(F(" (Location: ")); 
  if (gps.location.isValid())
  {
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
    Serial.print(")");
  }
  else
  {
    Serial.print(F("INVALID"));
  }
  Serial.println();
}
/**
  * @brief  Double To String
  * @param  str: Array or pointer for storing strings
  * @param  double_num: Number to be converted
  * @param  len: Fractional length to keep
  * @retval None
  */
void  DoubleToString( char *str, double double_num,unsigned int len) { 
  double fractpart, intpart;
  fractpart = modf(double_num, &intpart);
  fractpart = fractpart * (pow(10,len));
  sprintf(str + strlen(str),"%d", (int)(intpart)); //Integer part
  sprintf(str + strlen(str), ".%d", (int)(fractpart)); //Decimal part
}

void setthetime(void)
{
  int Year = gps.date.year();
  byte Month = gps.date.month();
  byte Day = gps.date.day();
  byte Hour = gps.time.hour();
  byte Minute = gps.time.minute();
  byte Second = gps.time.second();
  setTime(Hour, Minute, Second, Day, Month, Year);  // set the time of the microcontroller to the UTC time from the GPS
  Serial.println("Time set");
}

void displaythetime(void)
{

  char buf[40];
  char m[4];    // temporary storage for month string (DateStrings.cpp uses shared buffer)
  TimeChangeRule *tcr;        // pointer to the time change rule, use to get the TZ abbrev

  utc = now();  // read the time in the correct format to change via the TimeChangeRules
  time_t t = NZ.toLocal(utc, &tcr);
  strcpy(m, monthShortStr(month(t)));
  sprintf(buf, "%d:%d:%d %s %d %s %d %s", hour(t), minute(t), second(t), dayShortStr(weekday(t)), day(t), m, year(t), tcr -> abbrev);
  Serial.print(buf);
}
