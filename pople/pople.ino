/**
   BasicHTTPClient.ino
   미세먼지 알리미
   - 포플 -

*/

#include <Arduino.h>
//아두이노 Uno -> R1 호환 돕는 라이브러리 호출

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
//R1 내부에 있는 ESP8266 WiFi 연결 라이브러리 호출

#include <ESP8266HTTPClient.h>
//ESP8266과 HTTPClient 연결 호환 돕는 라이브러리 호출

#include <WiFiClient.h>

//LCD Display Libary
#include <LiquidCrystal_I2C.h>

#include <Wire.h>

LiquidCrystal_I2C lcd (0x27,16,2);

//


ESP8266WiFiMulti WiFiMulti;
//호출은 해놓았으나 아직 사용하지 않음

//sido는 내가 원하는 API 정보를 받아오는 지역, key는 공공데이터 포털에서 미세먼지 관련 API를 받아오게 끔 발급된 키 값, url은 키값을 연결해 받아오고자 하는 링크
String sido = "서울"; // 서울, 부산, 대구, 인천, 광주, 대전, 울산, 경기, 강원, 충북, 충남, 전북, 전남, 경북, 경남, 제주, 세종 중 입력
String key = "API Key값";
String url = "http://openapi.airkorea.or.kr/openapi/services/rest/ArpltnInforInqireSvc/getCtprvnMesureSidoLIst?serviceKey=" + key + "&numOfRows=30&pageNo=1&sidoName=" + sido + "&searchCondition=DAILY";

float so2, co, o3, no2, pm10, pm25 = 0; // 아황산가스(이산화황), 일산화탄소, 오존, 이산화질소, 미세먼지, 초미세먼지
int score = 0;

void setup()
{
  // 시리얼 세팅. ESP 8266 기본 시리얼 통신 보드레이트는 115200
  Serial.begin(115200);
  Serial.println();

  // 와이파이 접속
  WiFi.begin("SSID", "Password"); // 전: 공유기 이름, 후: 비밀번호 

  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) // 와이파이 접속하는 동안 "." 출력. 와이파이 Loading
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP()); // 접속된 와이파이 주소 출력. 메이커 챌린지 당일에 연결할 와이파이 모듈 확인

  //LED Pin Setting. 현재 전구 전압이 2V인데 6V 연결하면 전구 터짐. 그리고 아두이노에 직접 연결하면 ESP8266에 공급되는 전압이 부족해서 ESP 작동 안함.
//  pinMode(8, OUTPUT); //Green LED 초록 전구 5번 핀 
//  pinMode(9, OUTPUT); //Yellow LED 노란 전구 6번 핀
//  pinMode(10, OUTPUT); //Blue LED 파란 전구 7번 핀
//  pinMode(0, OUTPUT); //Red LED 빨간 전구 9번 핀

 lcd.begin();
 lcd.backlight();
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) // 와이파이가 접속되어 있는 경우
  {
    WiFiClient client; // 와이파이 클라이언트 객체
    HTTPClient http; // HTTP 클라이언트 객체

    if (http.begin(client, url)) {  // HTTP
      // 서버에 연결하고 HTTP 헤더 전송
      int httpCode = http.GET();

      // httpCode 가 음수라면 에러
      if (httpCode > 0) { // 에러가 없는 경우
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          String payload = http.getString(); // 받은 XML 데이터를 String에 저장
          int cityIndex = payload.indexOf("마포구");
          Serial.println(httpCode);
          Serial.println(cityIndex);
          so2 = getNumber(payload, "<so2Value>", cityIndex); // 아황산가스(이산화황)
          co = getNumber(payload, "<coValue>", cityIndex); // 일산화탄소
          o3 = getNumber(payload, "<o3Value>", cityIndex); // 오존
          no2 = getNumber(payload, "<no2Value>", cityIndex); // 이산화질소
          pm10 = getNumber(payload, "<pm10Value>", cityIndex); // 미세먼지
          pm25 = getNumber(payload, "<pm25Value>", cityIndex); // 초미세먼지
        Serial.println(so2); // 아황산가스(이산화황)
        Serial.println(co); // 일산화탄소
        Serial.println(o3); // 오존
        Serial.println(no2); // 이산화질소
        Serial.println(pm10); // 미세먼지
        Serial.println(pm25); // 초미세먼지
        //지금 시리얼 통신 받아오면 0.0이라 받아오는데, 문제는 에어 코리아에서 측정소 일시 점검 중이라 값을 받아오는 게 안됨..
        }
      } else {
        Serial.printf("[HTTP] GET... 실패, 에러코드: %s\n", http.errorToString(httpCode).c_str());
      }
      http.end();
    } else {
      Serial.printf("[HTTP] 접속 불가\n");
    }

    lcd.setCursor(2,0);
    lcd.print(pm10); //미세먼지
    lcd.setCursor(2,1);
    lcd.print(pm25); //초미세먼지
    delay(600000);



//    score = getScore(); // score 변수에 대기오염점수 저장
//    Serial.println(score); // 시리얼로 출력
//    setLEDColor(score); // 점수에 따라 LED 색상 출력
//    delay(600000);
  }
}

float getNumber(String str, String tag, int from) {
  float num = -1;
  int f = str.indexOf(tag, from) + tag.length();
  int t = str.indexOf("<", f);
  String s = str.substring(f, t);
  return s.toFloat();
}


//int getScore() {
//  int s = -1;
//  if (pm10 >= 151 || pm25 >= 76 || o3 >= 0.38 || no2 >= 1.1 || co >= 32 || so2 > 0.6) // 최악
//    s = 7;
//  else if (pm10 >= 101 || pm25 >= 51 || o3 >= 0.15 || no2 >= 0.2 || co >= 15 || so2 > 0.15) // 매우 나쁨
//    s = 6;
//  else if (pm10 >= 76 || pm25 >= 38 || o3 >= 0.12 || no2 >= 0.13 || co >= 12 || so2 > 0.1) // 상당히 나쁨
//    s = 5;
//  else if (pm10 >= 51 || pm25 >= 26 || o3 >= 0.09 || no2 >= 0.06 || co >= 9 || so2 > 0.05) // 나쁨
//    s = 4;
//  else if (pm10 >= 41 || pm25 >= 21 || o3 >= 0.06 || no2 >= 0.05 || co >= 5.5 || so2 > 0.04) // 보통
//    s = 3;
//  else if (pm10 >= 31 || pm25 >= 16 || o3 >= 0.03 || no2 >= 0.03 || co >= 2 || so2 > 0.02) // 양호
//    s = 2;
//  else if (pm10 >= 16 || pm25 >= 9 || o3 >= 0.02 || no2 >= 0.02 || co >= 1 || so2 > 0.01) // 좋음
//    s = 1;
//  else // 최고
//    s = 0;
//  return s;
//}

//void setLEDColor(int s) {
//  int color;
//  if (s == 0) // 최고
//    digitalWrite(10, HIGH);
//  else if (s == 1) // 좋음
//    digitalWrite(10, HIGH);
//  else if (s == 2) // 양호
//    digitalWrite(8, HIGH);
//  else if (s == 3) // 보통
//    digitalWrite(8, HIGH);
//  else if (s == 4) // 나쁨
//    digitalWrite(9, HIGH);
//  else if (s == 5) // 상당히 나쁨
//    digitalWrite(9, HIGH);
//  else if (s == 6) // 매우 나쁨
//    digitalWrite(0, HIGH);
//  else // 최악
//    digitalWrite(7, HIGH);
//
//}  
