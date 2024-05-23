#include <DHT.h>
#include <SoftwareSerial.h>
#include <TinyGPS.h>

#define TXpin 1
#define RXpin 0
#define WATER_LIMIT 200 // 200 밑이면 부저 울림
#define GAS_LIMIT 300   // 300 이상이면 워터펌프 작동

/*======================온습도 측정======================*/
void temp_humi();   
DHT dht(7, DHT11);
float humi;   // Stores humidity value
float temp;   // Stores temperature value
/*=======================================================*/

/*======================불꽃 측정=========================*/
void flamefunc();
const int flame_PIN = 2;
int flame_State = 0;
/*=======================================================*/


/*======================가스 측정=========================*/ 
void gasfunc();
const int gas_PIN = A0;
int gas_State = 0;
/*=======================================================*/

/*======================워터 펌프=========================*/ 
const int AA = 9; // A1A 연결
const int AB = 8; // A1B 연결
int pump_State = 0; 
/*=======================================================*/

/*====================== 물 수위 =========================*/ 
void waterfunc();
const int water_PIN = 6;
int water_State = 0;
/*=======================================================*/


/*====================== GPS =========================*/ 
TinyGPS gps;
SoftwareSerial ss(4, 3);
double exLAT=37.582946;
double exLON=127.010391;
void GPS();
/*====================================================*/ 



int count = 0;

void setup() {  
  /* 온습도 센서 */
  dht.begin();
  /* 비접촉 물수위 센서 */
  pinMode(water_PIN, INPUT);

  /* 불꽃감지 센서 */
  pinMode(flame_PIN, INPUT);

  /* 가스측정 센서 */
  pinMode(gas_PIN, INPUT);

  /* 워터펌프 */
  pinMode(AA, OUTPUT);
  pinMode(AB, OUTPUT);

  /* GPS */
  ss.begin(9600);
  Serial.print("Simple TinyGPS library v. "); Serial.println(TinyGPS::library_version());

  Serial.begin(115200);
}

void loop() {
  /***************************************************/
  pump_State = 0;
  waterfunc();  // 물 수위 측정
  gasfunc();  // 가스 측정
  temp_humi();  // 온습도 측정
  flamefunc();    // 불꽃 측정
  /* =================== 워터펌프 작동 =================== */
   if(gas_State >= 400 || flame_State == 1){
    pump_State = 1;
    digitalWrite(AA, HIGH);
    digitalWrite(AB, LOW);
    delay(2000);  // 2초 동작
    digitalWrite(AA, LOW);
    digitalWrite(AB, LOW);
  }
  // String state = "[" + String(count) + "]" + " " +String(water_State)+ " "+String(gas_State)+ " "+ String(flame_State)+ " "+String(humi)+ " "+String(temp)+ " "+String(ex_LAT)+ " "+String(ex_LON);
  String state = String(water_State)+ " "+String(gas_State)+ " "+ String(flame_State)+ " "+String(humi)+ " "+String(temp)+ " "+String(exLAT)+ " "+String(exLON)+ " "+String(pump_State);
  Serial.println(state);        
  // count += 1;
  delay(1000);
       
}

/* =================== 물 수위 측정 =================== */
void waterfunc(){
  water_State = digitalRead(water_PIN);
}

/* =================== 가스 측정 =================== */
void gasfunc(){
  gas_State = analogRead(gas_PIN);
}

/* =================== 불꽃 측정 =================== */
void flamefunc(){  
  flame_State = digitalRead(flame_PIN);
}

/* =================== 온습도 측정 =================== */
void temp_humi(){ 
  humi = dht.readHumidity();
  temp = dht.readTemperature();
}

/*====================== GPS =========================*/ 
void GPS(){
  bool newData = false;
  unsigned long chars;
  unsigned short sentences, failed;
  

  // For one second we parse GPS data and report some key values
  for (unsigned long start = millis(); millis() - start < 1000;)
  {
    while (ss.available())
    {
      char c = ss.read();
      // Serial.write(c); // uncomment this line if you want to see the GPS data flowing
      if (gps.encode(c)) // Did a new valid sentence come in?
        newData = true;
    }
  }
  

  if (newData)
  {
    float flat, flon;
    unsigned long age;
    gps.f_get_position(&flat, &flon, &age);
    // Serial.print("LAT=");
    // Serial.print(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6);
    exLAT = (flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6);
    exLON = (flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6);
    // Serial.print(ex_LAT);  // 임시
    // Serial.print(" LON=");
    // Serial.print(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6);
    // Serial.print(ex_LON);  // 임시
    // Serial.print(" SAT=");
    // Serial.print(gps.satellites() == TinyGPS::GPS_INVALID_SATELLITES ? 0 : gps.satellites());
    // Serial.print(" PREC=");
    // Serial.print(gps.hdop() == TinyGPS::GPS_INVALID_HDOP ? 0 : gps.hdop());
  }
  
  gps.stats(&chars, &sentences, &failed);
  // Serial.print(" CHARS=");
  // Serial.print(chars);
  // Serial.print(" SENTENCES=");
  // Serial.print(sentences);
  // Serial.print(" CSUM ERR=");
  // Serial.println(failed);
  // if (chars == 0)
  //   Serial.println("** No characters received from GPS: check wiring **");

}





