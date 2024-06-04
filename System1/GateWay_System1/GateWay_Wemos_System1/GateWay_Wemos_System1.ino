#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

String apiKey = "Your api for ThingSpeak";  // ThingSpeak API key값 입력
const char* server = "api.thingspeak.com";   // ThingSpeak URL
const char* ssid = "Your SSID"; // ESP8266이 연결되는 SSID
const char* password = "PW of your SSID"; // SSID 비밀번호

#define API_KEY "Your api for firebase" // Firebase API key값 입력
#define DATABASE_URL "URL of firebase"   // Firebase URL 입력

#define ATSerial Serial
#define TXpin 11
#define RXpin 10

WiFiClient client;
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
bool signupOK = false;

SoftwareSerial DebugSerial(RXpin, TXpin);

int water_state = 0;  // 물 수위 센서
int gas_state = 0;  // 가스 감지 센서
int flame_state = 0;  // 불꽃감지 센서

void setup() {
  Serial.begin(115200);
  delay(10);

  while (ATSerial.read() >= 0) {}
  while (!ATSerial);

  WiFi.begin(ssid, password);
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("wait..");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  Serial.println();

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  // 시리얼 통신으로 데이터가 들어올 경우
  if (Serial.available() > 0) {
    Serial.flush();
    String receivedString = Serial.readStringUntil('\n');
    receivedString.replace("AT+RECV", "");
    receivedString.replace("AT_RX_TIMEOUT", "");
  
    int firstIndex = receivedString.indexOf(' ');   // 0
    int secondIndex = receivedString.indexOf(' ', firstIndex+1);  // 1
    int thirdIndex = receivedString.indexOf(' ',secondIndex+1);   // 2
    int fourIndex = receivedString.indexOf(' ', thirdIndex+1);   // 3
    int fiveIndex = receivedString.indexOf(' ', fourIndex+1);   // 4
    int sixIndex = receivedString.indexOf(' ', fiveIndex+1);   // 5 
    int sevenIndex = receivedString.indexOf(' ', sixIndex+1);  // 6

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

    if(gas_State != 0 && humi != 0.0 && temp != 0.0){
      Serial.println("water : " + String(water_State) + ", gas : " + String(gas_State) + ", flame : " + String(flame_State) + ", Humidity : " + String(humi) + ", Temperature : " + String(temp) + ", Latitude : "+ String(ex_LAT)+ ", Lontitude : "+String(ex_LON)+", water pump : "+String(pump_State));
      //firebase 업로드 시작(realtime)
      if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0)){
        sendDataPrevMillis = millis();
        //물
        if (Firebase.RTDB.setInt(&fbdo, "Sys1/water_State", water_State)){
          Serial.println("PASSED");
          Serial.println("PATH: " + fbdo.dataPath());
          Serial.println("TYPE: " + fbdo.dataType());
        }
        else {
          Serial.println("FAILED");
          Serial.println("REASON: " + fbdo.errorReason());
        }
        //가스
        if (Firebase.RTDB.setInt(&fbdo, "Sys1/gas_State", gas_State)){
          Serial.println("PASSED");
          Serial.println("PATH: " + fbdo.dataPath());
          Serial.println("TYPE: " + fbdo.dataType());
        }
        else {
          Serial.println("FAILED");
          Serial.println("REASON: " + fbdo.errorReason());
        }
        //불꽃
        if (Firebase.RTDB.setInt(&fbdo, "Sys1/flame_State", flame_State)){
          Serial.println("PASSED");
          Serial.println("PATH: " + fbdo.dataPath());
          Serial.println("TYPE: " + fbdo.dataType());
        }
        else {
          Serial.println("FAILED");
          Serial.println("REASON: " + fbdo.errorReason());
        }
        //습도
        if (Firebase.RTDB.setFloat(&fbdo, "Sys1/humidity", humi)){
          Serial.println("PASSED");
          Serial.println("PATH: " + fbdo.dataPath());
          Serial.println("TYPE: " + fbdo.dataType());
        }
        else {
          Serial.println("FAILED");
          Serial.println("REASON: " + fbdo.errorReason());
        }
        //온도
        if (Firebase.RTDB.setFloat(&fbdo, "Sys1/temperature", temp)){
          Serial.println("PASSED");
          Serial.println("PATH: " + fbdo.dataPath());
          Serial.println("TYPE: " + fbdo.dataType());
        }
        else {
          Serial.println("FAILED");
          Serial.println("REASON: " + fbdo.errorReason());
        }
        //위도
        if (Firebase.RTDB.setFloat(&fbdo, "Sys1/ex_LAT", ex_LAT)){
          Serial.println("PASSED");
          Serial.println("PATH: " + fbdo.dataPath());
          Serial.println("TYPE: " + fbdo.dataType());
        }
        else {
          Serial.println("FAILED");
          Serial.println("REASON: " + fbdo.errorReason());
        }
        //경도
        if (Firebase.RTDB.setFloat(&fbdo, "Sys1/ex_LON", ex_LON)){
          Serial.println("PASSED");
          Serial.println("PATH: " + fbdo.dataPath());
          Serial.println("TYPE: " + fbdo.dataType());
        }
        else {
          Serial.println("FAILED");
          Serial.println("REASON: " + fbdo.errorReason());
        }
        //펌프 구동 여부
        if (Firebase.RTDB.setInt(&fbdo, "Sys1/pump_State", pump_State)){
          Serial.println("PASSED");
          Serial.println("PATH: " + fbdo.dataPath());
          Serial.println("TYPE: " + fbdo.dataType());
        }
        else {
          Serial.println("FAILED");
          Serial.println("REASON: " + fbdo.errorReason());
        }
        
      }
    // Fire base 업로드 끝
    //Thingspeak 업로드 시작
      if (client.connect(server,80)) { // "184.106.153.149" or api.thingspeak.com
        String getStr = "GET /update?api_key=";
        getStr += apiKey;
        getStr += "&field1=";
        getStr += String(water_State);
        getStr += "&field2=";
        getStr += String(gas_State);
        getStr += "&field3=";
        getStr += String(flame_State);
        getStr += "&field4=";
        getStr += String(humi);
        getStr += "&field5=";
        getStr += String(temp);
        getStr += "\r\n\r\n";
        client.print(getStr + " HTTP/1.1\r\n" + "Host: " + server + "\r\n" + "Connection: close\r\n\r\n");
        
      }
    }

  }



}