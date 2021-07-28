#include <Wire.h>               
#include "cubecell_SH1107Wire.h"
#include "LoRaWan_APP.h"
#include "Arduino.h"
#include <SoftwareSerial.h>

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

#define TX_OUTPUT_POWER                             14        // dBm

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

int16_t txNumber;

int16_t rssi,rxSize;

char str[10];

unsigned long ALERT_TIME = 10000;
unsigned long DELAY_TIME = 20000;
unsigned long delayStart = 0; // the time the delay started
bool delayRunning = false; // true if still waiting for delay to finish
unsigned long lastPulse = 0;

//extern HardwareSerial Serial1;
SoftwareSerial MH10(17, 18);

void setup() {
    boardInitMcu( );
    Serial.begin(9600);
    Serial1.begin(9600);

    txNumber=0;
    rssi=0;
	
	  RadioEvents.RxDone = OnRxDone;
    Radio.Init( &RadioEvents );
    Radio.SetChannel( RF_FREQUENCY );
	
	  Radio.SetRxConfig( MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
                                   LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
                                   LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
                                   0, true, 0, 0, LORA_IQ_INVERSION_ON, true );
   turnOnRGB(COLOR_SEND,0); //change rgb color
   Serial.println("into RX mode");

    VextON();
    delay(100);

    // Initialising the UI will init the display too.
    display.init();
    display.setFont(ArialMT_Plain_10);
    Serial.println("Display initiated");
    Serial1.println("Display initiated");

    delayStart = millis();
   }



void loop()
{
  display.connect();

  display.setTextAlignment(TEXT_ALIGN_RIGHT);
  display.drawString(10, 128, String(millis()));
  // write the buffer to the display
  display.display();

  if (delayRunning && (((millis() - lastPulse) - delayStart) >= DELAY_TIME)) {
    turnOnRGB(COLOR_SEND,0);
    Serial.write("Emergency\n");
  }
  else if (delayRunning && (((millis() - lastPulse) - delayStart) >= ALERT_TIME)) {
    Serial1.println("Alert");
    Serial.write("Alert\n");
    turnOnRGB(COLOR_SEND,0);
    delay(100);
    turnOffRGB();
  }
  
	Radio.Rx( 0 );
  delay(500);
  Radio.IrqProcess( );
}

void OnRxDone( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr )
{
    display.clear();

    lastPulse = millis();
    delayRunning = true;
    delayStart = 0;

    Serial1.write("Hello");
    
    rssi=rssi;
    rxSize=size;
    memcpy(rxpacket, payload, size );
    rxpacket[size]='\0';
    turnOnRGB(COLOR_RECEIVED,0);
    Radio.Sleep( );
    
    VextON();
    display.setFont(ArialMT_Plain_10);
    sprintf(str, "%s rssi(%idb)\n", rxpacket, rssi);
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.drawString(10, 5, str);
    Serial.write(str);
    

    turnOffRGB();

}

void  DoubleToString( char *str, double double_num,unsigned int len) { 
  double fractpart, intpart;
  fractpart = modf(double_num, &intpart);
  fractpart = fractpart * (pow(10,len));
  sprintf(str + strlen(str),"%d", (int)(intpart)); //Integer part
  sprintf(str + strlen(str), ".%d", (int)(fractpart)); //Decimal part
}


void VextON(void)
{
  pinMode(Vext,OUTPUT);
  digitalWrite(Vext, LOW);
}

void VextOFF(void) //Vext default OFF
{
  pinMode(Vext,OUTPUT);
  digitalWrite(Vext, HIGH);
}
