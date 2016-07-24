
#include "Arduino.h"
#include "AsyncGSM.h"

#define FONA_RX 9
#define FONA_TX 8
#define FONA_RST 4

#include <SoftwareSerial.h>
SoftwareSerial fonaSS = SoftwareSerial(FONA_TX, FONA_RX);
SoftwareSerial *gsmSerial = &fonaSS;

//HardwareSerial *gsmSerial = &Serial3;

AsyncGSM gsm = AsyncGSM(FONA_RST);

long lastudpsent = 0;

uint8_t request_sent = 0;

void setup() {
  while (!Serial);
  
  Serial.begin(57600);
  gsmSerial->begin(4800);

  Serial.println(F("Resetting FONA"));
  pinMode(4, OUTPUT);
  digitalWrite(4, LOW);
  delay(500);
  digitalWrite(4, HIGH);
  delay(10000);

  
  Serial.println(F("Initializing GSM"));

  gsm.initialize(*gsmSerial);
}


void loop() {
  gsm.process();

  if (gsm.messageAvailable()) {
    ShortMessage message = gsm.readMessage();
    DEBUG_PRINT(F("Received: "));
    DEBUG_PRINTLN(message.message);
    time_t current_time = gsm.getCurrentTime();
    DEBUG_PRINT(F("Current_time: "));
    DEBUG_PRINTLN(current_time);

    gsm.sendMessage(message);
  }

  if (gsm.incomingCall()) {
    gsm.answerIncomingCall();
  }

  /*
  for (int j = 0; j < 6; j++) {
    if (!gsm.isConnected(j) && gsm.isModemIdle()) {
      char address[16] = "XXX.XXX.XXX.XX";
      gsm.connectUDP(address, 1195 + j, j);
    }
  } 

  if (millis() > (lastudpsent + 60000)) {
    for (int i = 0; i < 6; i++) {
      if (gsm.isConnected(i)) {
	char data[16];
	sprintf(data, "Test%u", millis());
	gsm.writeData(data, strlen(data), i);
      }
    }
    lastudpsent = millis();
  }
  */

  if (!gsm.isConnected(0) && gsm.isModemIdle()) {
      char address[16] = "XXX.XXX.XXX.XX";
      gsm.connect(address, 2223, 0, CONNECTION_TYPE_TCP);    
  }

  if (gsm.isConnected(0) && gsm.isModemIdle() && !request_sent) {
    char data[32];
    sprintf(data, "GET / HTTP/1.0\r\n\r\n");
    gsm.writeData(data, strlen(data), 0);
    request_sent = 1;
  }
  
  
  /*
  if (gsm.isConnected(0) && gsm.isModemIdle()) {
    gsm.disconnect(0);
  }
  */

}
