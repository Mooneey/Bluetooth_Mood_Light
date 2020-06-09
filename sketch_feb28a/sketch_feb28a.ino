#include <SoftwareSerial.h> //시리얼 통신 라이브러리 호출
#include <DS1302.h> 

DS1302 RTC(4,5, 6);

const int  pinLatch   = 8; //STcp - Storage register clock input 12
const int  pinClock   = 12; //SHcp - Shift register clock input 11
const int  pinData    = 11; // Serial Data input 14

int blueTx=2;   //Tx (블투 보내는핀 설정)
int blueRx=3;   //Rx (블투 받는핀 설정)
int relay = 10; //릴레이 Signal 핀 설정

int hour;
int min;   

int segValue[10] = {
                          //digit(a b c d e f g dp )
                    127,//0(        1 1 1 1 1 1 0  0 = 252)
                    12,//1
                    182,//2
                    158,//3
                    204,//4
                    218,//5
                    250,//6
                    14, //7
                    254,//8
                    222 //9
                    };

SoftwareSerial mySerial(blueTx, blueRx);  //시리얼 통신을 위한 객체선언
String myString=""; //받는 문자열
 
void setup() {
  pinMode (pinLatch, OUTPUT);
  pinMode (pinClock, OUTPUT);
  pinMode (pinData, OUTPUT);
  
  mySerial.begin(9600); //블루투스 시리얼 개방
  pinMode (relay, OUTPUT); // relay를 output으로 설정

   /*디지털 시간*/
  Serial.begin(9600);
  //SetTime();          // 초기 시간 설정 이후 주석처리
 // Moode();
  digitalWrite(relay, 0);
}
 
void loop() {
  BlueTooth();
  GetTime();
  TransLED();
  delay(500);
}

void SetTime() {            // 초기 시간 설정
  RTC.halt(false);
  RTC.writeProtect(false);
  RTC.setDOW(FRIDAY);
  RTC.setTime(17, 20, 20);
  RTC.setDate(02, 03, 2020);
}

void GetTime() {            // 시간 출력
  hour = RTC.getTime().hour;
  min = RTC.getTime().min;
  Serial.print(hour); Serial.print(" : ");
  Serial.print(min); Serial.print(" : ");
  Serial.println(RTC.getTime().sec);
}

void TransLED() {
  digitalWrite(pinLatch, LOW);
  shiftOut (pinData, pinClock, MSBFIRST, segValue[min % 10]);// 분 : 일의 자리
  shiftOut (pinData, pinClock, MSBFIRST, segValue[min / 10]);// 분 : 십의 자리
  shiftOut (pinData, pinClock, MSBFIRST, segValue[hour % 10]);// 시 : 일의 자리
  shiftOut (pinData, pinClock, MSBFIRST, segValue[hour / 10]);// 시 : 십의 자리
  digitalWrite (pinLatch, HIGH); 
}

void Moode() {
  digitalWrite(relay, HIGH);
  delay(5000);
  digitalWrite(relay, LOW);
}

void BlueTooth() {         // 블루투스 통신
  while(mySerial.available())  //mySerial 값이 있으면
  {
    char myChar = (char)mySerial.read();  //mySerial int형식의 값을 char형식으로 변환
    myString+=myChar;   //수신되는 문자열을 myString에 모두 붙임 (1바이트씩 전송되는 것을 모두 붙임)
    delay(5);           //수신 문자열 끊김 방지
  }
  
  if(!myString.equals(""))  //myString 값이 있다면
  {
    Serial.println("input value: "+myString); //시리얼모니터에 myString값 출력
 
      if(myString=="on")  //myString 값이 'on' 이라면
      {
        digitalWrite (relay, HIGH); // 릴레이 ON
      } else {
        digitalWrite (relay, LOW); // 릴레이 OFF
      }
    myString="";  //myString 변수값 초기화
  }
}
