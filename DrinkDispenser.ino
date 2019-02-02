#include <LiquidCrystal_I2C.h>
#include <Wire.h>
LiquidCrystal_I2C icd(0x27, 2, 16);

//모터A 컨트롤
int ENA=2;
int IN1=3;
int IN2=4;

//모터B 컨트롤
int ENB=5;
int IN3=6;
int IN4=7;

//완료 표시 LED
int finishLED = 8;

//버튼
int goButton = 9;

// 버튼
int cursorButton = 11;
int selectButton = 10;

//입력 관련 변수
int cursorPosition = 0;
int volumeCounter = 1;
int levelCounter = 1;
double volume1 = 100;
double volume2 = 150;
double volume3 = 300;
double level1Ratio = 9;
double level2Ratio = 4;
double level3Ratio = 2.333;
double level4Ratio = 1;
double level5Ratio = 0.111;
String level1Text = "Sulzzi(1:9)";
String level2Text = "Alss(1:4)";
String level3Text = "Slodka(3:7)";
String level4Text = "Oojung(1:1)";
String level5Text = "Thanos(9:1)";

//모터 변수
double volume;
double level;
double volumePerSecond = 30;


void setup() {
  // LCD 관련
  icd.init();
  icd.backlight();
  icd.setCursor(0,0);
  icd.print(">Volume:");
  icd.print(" 100 mL");
  icd.setCursor(1,1);
  icd.print("Lv: Sulzzi(1:9)");
  pinMode(cursorButton, INPUT); 
  pinMode(selectButton, INPUT);
  //모터 관련 핀
  pinMode(ENA,OUTPUT);
  pinMode(ENB,OUTPUT);
  pinMode(IN1,OUTPUT);
  pinMode(IN2,OUTPUT);
  pinMode(IN3,OUTPUT);
  pinMode(IN4,OUTPUT);
  pinMode(goButton, INPUT);
  pinMode(finishLED, OUTPUT);
  Serial.begin(9600);
}



void loop() {
  
  //========== Cursor 세팅===============
  if(cursorPosition == 0) {
    icd.setCursor(0,1);
    icd.print(" ");
    icd.setCursor(0,0);
    icd.print(">");
    icd.setCursor(8,0);
//    icd.cursor();
  }else {
    icd.setCursor(0,1);
    icd.print(">");
    icd.setCursor(0,0);
    icd.print(" ");
    icd.setCursor(1,4);
//    icd.cursor();
  }

  // ========== select button을 눌렀을 때: 커서 포지션 확인하고 해당되는 값 증가시키고 화면에 출력함========== 
  if(digitalRead(selectButton) == HIGH) {
    if(cursorPosition == 0) {
      volumeCounter++;
      if(volumeCounter == 4) volumeCounter = 1;
      icd.setCursor(9,0);
      icd.print("   ");
      icd.setCursor(9,0);
      if(volumeCounter == 1) icd.print((int) volume1);
      else if(volumeCounter == 2) icd.print((int) volume2);
      else icd.print((int) volume3);
    } else {
      levelCounter++;
      if(levelCounter == 6) levelCounter = 1;
      icd.setCursor(5,1);
      icd.print("               ");
      icd.setCursor(5,1);
      if(levelCounter == 1) icd.print(level1Text);
      else if(levelCounter == 2) icd.print(level2Text);
      else if(levelCounter == 3) icd.print(level3Text);
      else if(levelCounter == 4) icd.print(level4Text);
      else icd.print(level5Text);
    }
  }
  Serial.print("volumeCounter: ");
  Serial.println(volumeCounter);
  
  Serial.print("levelCounter: ");
  Serial.println(levelCounter);

  //==========  cursor button을 눌렀을 때========== 
  if (digitalRead(cursorButton) == HIGH){
    if(cursorPosition == 0) cursorPosition = 1;
    else cursorPosition = 0;
  }

  Serial.print("cursorPosition: ");
  Serial.println(cursorPosition);
  
  delay(200);

  // ========== 모터 관련 코드========== 
  int pressed = digitalRead(goButton);

  //========== go 버튼 눌렸을 때 ========== 
  if(pressed == HIGH) {
    
    // counter 들에 따라 volume 과 level을 설정함
    if(volumeCounter == 1) volume = volume1;
    else if(volumeCounter ==2) volume = volume2;
    else volume = volume3;

    if(levelCounter == 1) level = level1Ratio;
    else if(levelCounter == 2) level = level2Ratio;
    else if(levelCounter == 3) level = level3Ratio;
    else if(levelCounter == 4) level = level4Ratio;
    else level = level5Ratio;


    
    // volume과 level에 따라서 필요한 소주와 맥주의 양을 계산함
    Serial.println(volume);
    Serial.println(1/(1 + level));
    Serial.println(level);
    double sojuVolume = volume * (1 / (1 + level));
    double beerVolume = sojuVolume * level;
    Serial.print("sojuVolume: ");
    Serial.println(sojuVolume);
    Serial.print("beerVolume: ");
    Serial.println(beerVolume);
    
    // 1초에 나오는 양에 따라서 각 모터를 가동시켜야할 시간을 계산함
    int sojuTime = (sojuVolume / volumePerSecond) * 1000;
    int beerTime = (beerVolume / volumePerSecond) * 1000;
  
    sojuMotorStart();
    beerMotorStart();
  
    // 맥주가 더 많으면 소주 먼저 멈추고 맥주 멈춤. 반대로 소주가 더 많으면 소주 먼저 멈추고 맥주 멈춤.
    if(sojuTime <= beerTime) {
      delay(sojuTime);
      sojuMotorStop();
      delay(beerTime - sojuTime);
      beerMotorStop();
    } else {
      delay(beerTime);
      beerMotorStop();
      delay(sojuTime - beerTime);
      sojuMotorStop();
    }
  
    // 다 나온 후 2초 기다린 후에 완료 표시로 초록 LED 켰다가 다시 끔
    delay(2000);
    digitalWrite(finishLED, HIGH);
    delay(3000);
    digitalWrite(finishLED, LOW);
  }
}

// 모터 모두 Start
void bothMotorStart()
{
    digitalWrite(IN1,HIGH);
    digitalWrite(IN2,LOW);
    digitalWrite(IN3,HIGH);
    digitalWrite(IN4,LOW);
}
 
// 모터 모두 Stop
void stopAllMotor()
{
    digitalWrite(IN1,LOW);
    digitalWrite(IN2,LOW);
    digitalWrite(IN3,LOW);
    digitalWrite(IN4,LOW);
}

// 소주 모터 Start
void sojuMotorStart()
{
    digitalWrite(ENA,HIGH);
    digitalWrite(IN1,HIGH);
    digitalWrite(IN2,LOW);
    Serial.println("Soju motor started");
}

// 소주 모터 Stop
void sojuMotorStop()
{
    digitalWrite(ENA,LOW);
    digitalWrite(IN1,LOW);
    digitalWrite(IN2,LOW);
    Serial.println("Soju motor stopped");
}

// 맥주 모터 Start
void beerMotorStart()
{
    digitalWrite(ENB,HIGH);
    digitalWrite(IN3,HIGH);
    digitalWrite(IN4,LOW);
    Serial.println("Beer motor started");
}

// 맥주 모터 Stop
void beerMotorStop()
{
    digitalWrite(ENB,LOW);
    digitalWrite(IN3,LOW);
    digitalWrite(IN4,LOW);
    Serial.println("Beer motor stopped");
}
