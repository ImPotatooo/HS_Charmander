#include <SoftwareSerial.h>
#include "SNIPE.h"

#define ATSerial Serial
#define TXpin 11
#define RXpin 10
// 16byte hex key
String lora_app_key = "11 a2 33 a4 55 a6 77 a8 99 aa bb cc dd ee ff 00";  

// LoRa 모듈과의 통신을 위해 SoftwareSerial은 더 이상 사용하지 않습니다.
SoftwareSerial DebugSerial(RXpin,TXpin);
SNIPE SNIPE(ATSerial);

void setup() {
  ATSerial.begin(115200);

  // put your setup code here, to run once:
  while(ATSerial.read()>= 0) {}
  while(!ATSerial);

  DebugSerial.begin(115200);

  /* SNIPE LoRa Initialization */
  if (!SNIPE.lora_init()) {
    DebugSerial.println("SNIPE LoRa Initialization Fail!");
    while (1);
  }

  /* SNIPE LoRa Set Appkey */
  if (!SNIPE.lora_setAppKey(lora_app_key)) {
    DebugSerial.println("SNIPE LoRa app key value has not been changed");
  }
  
  /* SNIPE LoRa Set Frequency */
  if (!SNIPE.lora_setFreq(LORA_CH_1)) {
    DebugSerial.println("SNIPE LoRa Frequency value has not been changed");
  }

  /* SNIPE LoRa Set Spreading Factor */
  if (!SNIPE.lora_setSf(LORA_SF_7)) {
    DebugSerial.println("SNIPE LoRa Sf value has not been changed");
  }

  /* SNIPE LoRa Set Rx Timeout 
   * If you select LORA_SF_12, 
   * RX Timout use a value greater than 5000  
  */
  if (!SNIPE.lora_setRxtout(5000)) {
    DebugSerial.println("SNIPE LoRa Rx Timout value has not been changed");
  }  
  DebugSerial.println("SNIPE LoRa PingPong Test");
}

void loop() {
  String receivedString = SNIPE.lora_recv();
  if(receivedString){
    int firstIndex = receivedString.indexOf(' ');   // 0
    int secondIndex = receivedString.indexOf(' ', firstIndex+1);  // 1
    int thirdIndex = receivedString.indexOf(' ',secondIndex+1);   // 2
    int fourIndex = receivedString.indexOf(' ', thirdIndex+1);   // 3
    int fiveIndex = receivedString.indexOf(' ', fourIndex+1);   // 4
    int sixIndex = receivedString.indexOf(' ', fiveIndex+1);   // 5
    int sevenIndex = receivedString.indexOf(' ', sixIndex+1); 

    String first = receivedString.substring(0, firstIndex);
    String second = receivedString.substring(firstIndex+1, secondIndex);
    String third = receivedString.substring(secondIndex+1, thirdIndex);
    String four = receivedString.substring(thirdIndex+1, fourIndex);
    String five = receivedString.substring(fourIndex+1, fiveIndex);
    String six = receivedString.substring(fiveIndex+1, sixIndex);
    String seven = receivedString.substring(sixIndex+1, sevenIndex);
    String eight = receivedString.substring(sevenIndex);


    int water_State = first.toInt();
    int gas_State = second.toInt();
    int flame_State = third.toInt();
    float humi = four.toFloat();
    float temp = five.toFloat();
    float ex_LAT = six.toFloat();
    float ex_LON = seven.toFloat();
    int pump_State = eight.toInt();
  
    DebugSerial.println(receivedString);
    Serial.println(receivedString);
  }
}