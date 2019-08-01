#include <Adafruit_NeoPixel.h>

/*
 Music Art Robot v1.0 (2012.May.30)
 ---------------------------------------------------------
 아두이노 + MIDI악기쉴드 이용한 DIY 건반악기 예제소스 (Arduino 1.0 호환)
 ---------------------------------------------------------
 http://RoboBob.co.kr
 ---------------------------------------------------------
 */
#include <Keypad.h>
#include <SoftwareSerial.h>

#define PIXEL_PIN    24    // Digital IO pin connected to the NeoPixels.
#define PIXEL_COUNT 18

#define btn1  11  // 버튼1의 아두이노 핀번호 정의
#define btn2  10  // 버튼2의 아두이노 핀번호 정의
#define btn3  9   // 버튼3의 아두이노 핀번호 정의
#define btn4  8   // 버튼4의 아두이노 핀번호 정의
#define btn5  7   // 버튼5의 아두이노 핀번호 정의
#define btn6  6   // 버튼6의 아두이노 핀번호 정의
#define btn7  5   // 버튼7의 아두이노 핀번호 정의
// 3:midi rx , 4:midi reset  아두이노 핀 3번 4번은 이미 사용중
#define btn8  2  // 버튼8의 아두이노 핀번호 정의 
                  //(SoftSerial에서 Rx핀으로 선언되지만 재 세팅 후 버튼용으로 사용)

#define btn11 A3  // 버튼11의 아두이노 핀번호 정의
#define btn12 A2  // 버튼12의 아두이노 핀번호 정의
#define btn13 A1 

#define defaultPatch 15 //악기 초기화 버튼 설정 악기번호

SoftwareSerial mySerial(2, 3); //SW시리얼핀 정의 D3이 MIDI신호 전송용,  D2는 미사용 

byte note = 0; //The MIDI연주될 note(음계)
byte resetMIDI = 4; // VS1053 Reset용 핀
byte ledPin = 13; //MIDI 트래픽 표시용 LED

const byte ROWS = 4; 
const byte COLS = 4; 
byte value = 0;
char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {14, 15, 16 ,17}; 
byte colPins[COLS] = {18, 19, 20, 21}; 

Keypad customKeypad= Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);
//bool oldState = t;

boolean bs1 = false;  // 버튼1의 현재상태(눌림 or 안눌림)
boolean bs2 = false;  // 이하, 위와 유사
boolean bs3 = false;
boolean bs4 = false;
boolean bs5 = false;
boolean bs6 = false;
boolean bs7 = false;
boolean bs8 = false;
boolean bs11 = false;
boolean bs12 = false;

boolean br1 =0;  // 버튼1 상태 확인용 입력값 임시저장용
boolean br2 =0;  // 이하, 위와 유사
boolean br3 =0;
boolean br4 =0;
boolean br5 =0;
boolean br6 =0;
boolean br7 =0;
boolean br8 =0;
boolean br11 =0;
boolean br12 =0;

int patch = 0; //악기 대응, 연주될 악기 종류 (0~127: 기본 128 가지 선택가능)

int bn1 = 60; //버튼1의  note(음계)  가령 "도"  0~127까지 지정가능 (정확한 음계 설정은 MIDI관련정보참고)
int bn2 = 62; //버튼2의  note(음계)  가령 "레"
int bn3 = 64; //버튼3의  note(음계)  가령 "미"
int bn4 = 65; //버튼4의  note(음계)  가령 "파"
int bn5 = 67; //버튼5의  note(음계)  가령 "솔"
int bn6 = 69; //버튼6의  note(음계)  가령 "라"
int bn7 = 71; //버튼7의  note(음계)  가령 "시"
int bn8 = 72; //버튼8의  note(음계)  가령 "도~"

byte byteData;

//cylinder 
int ledPins[] = {24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51};
// Defines an array to store the pin numbers
// the pinMode() functions below. 

int delayTime = 100; // to pause between LEDs
int velocity_music = 200;
//int sensorValue = 0;  

void setup() {
  Serial.begin(9600);

  strip.begin();
  strip.show(); // Initialize all pixels to 'off'


  //미디컨트롤을 위한 소프트웨어 시리얼을 준비합니다.
  mySerial.begin(31250);
//  mySerial2.begin(57600);
  
  //VS1053를 초기화하고 사용할 준비를 합니다.
  pinMode(resetMIDI, OUTPUT);
  digitalWrite(resetMIDI, LOW);
  delay(100);
  digitalWrite(resetMIDI, HIGH);
  delay(100);
  
  pinMode( btn1, INPUT_PULLUP);      // 버튼1 입력용 핀모드를  입력모드로 전환
  digitalWrite( btn1, HIGH);  // 내부 PullUp 설정, 스위치의 나머지 한선은 GND에 물리면 됩니다.(초간단)

  pinMode( btn2, INPUT);      // 이하, 위와 유사
  digitalWrite( btn2, HIGH);
  
  pinMode( btn3, INPUT);
  digitalWrite( btn3, HIGH);
  pinMode( btn4, INPUT);
  digitalWrite( btn4, HIGH);
  pinMode( btn5, INPUT);
  digitalWrite( btn5, HIGH);
  pinMode( btn6, INPUT);
  digitalWrite( btn6, HIGH);
  pinMode( btn7, INPUT);
  digitalWrite( btn7, HIGH);
  pinMode( btn8, INPUT);
  digitalWrite( btn8, HIGH);
  pinMode( btn11, INPUT);
  digitalWrite( btn11, HIGH);  
  pinMode( btn12, INPUT);
  digitalWrite( btn12, HIGH);  
  
  //cylinder
  /*for(int j = 0; j < 28; j++){
    pinMode(ledPins[j],OUTPUT);
  }
  */
}

void loop() {
    
    br1 = digitalRead(btn1);
    br2 = digitalRead(btn2);
    br3 = digitalRead(btn3);
    br4 = digitalRead(btn4);
    br5 = digitalRead(btn5);
    br6 = digitalRead(btn6);
    br7 = digitalRead(btn7);
    br8 = digitalRead(btn8);
    br11 = digitalRead(btn11);
    br12 = digitalRead(btn12);    
    //sensorValue = analogRead(btn8);

    //bool newState = digitalRead(btn1);
  // Set the last button state to the old state.
//   oldState = newState;

   if( !bs1 && !br1 ){
     noteOn(0, bn1,velocity_music);
     theaterChecker();
     //theaterChase(strip.Color(127, 0, 0), 50); 
     bs1 = true;
   }else if(bs1 && br1){
     noteOff(0, bn1,0);   
     //theaterChase(strip.Color(0, 0, 0), 50); 
     bs1 = false;
   }
   
   if( !bs2 && !br2 ){
     noteOn(0, bn2,velocity_music);
     bs2 = true;
   }else if(bs2 && br2){
     noteOff(0, bn2,0);   

     bs2 = false;
   }   
   if( !bs3 && !br3 ){
     noteOn(0, bn3,velocity_music);
     bs3 = true;
   }else if(bs3 && br3){
     noteOff(0, bn3,0);   
     bs3 = false;
   }
   if( !bs4 && !br4 ){
     noteOn(0, bn4, velocity_music);
     bs4 = true;
   }else if(bs4 && br4){
     noteOff(0, bn4,0);   
     bs4 = false;
   }
   if( !bs5 && !br5 ){
     noteOn(0, bn5,velocity_music);
     bs5 = true;
   }else if(bs5 && br5){
     noteOff(0, bn5,0);   
     bs5 = false;
   }   
   if( !bs6 && !br6 ){
     noteOn(0, bn6,velocity_music);
     bs6 = true;
   }else if(bs6 && br6){
     noteOff(0, bn6,0);   
     bs6 = false;
   }   
   if( !bs7 && !br7 ){
     noteOn(0, bn7,velocity_music);
     bs7 = true;
   }else if(bs7 && br7){
     noteOff(0, bn7,0);   
     bs7 = false;
   }   
   
   /*if(sensorValue >= 10 && sensorValue < 20){
     noteOn(0, bn8, velocity_music);
   }
   else{
     noteOff(0, bn8, 0);
   }
   */
 
     
   if( !bs8 && !br8 ){
     noteOn(0, bn8,velocity_music);
     bs8 = true;
   }else if(bs8 && br8){
     noteOff(0, bn8,0);   
     bs8 = false;
   }   
   
   char customKey = customKeypad.getKey();
   if (customKey != NO_KEY){
      if ( (customKey >= '0') && (customKey<= '9') )
      {
       patch = patch *10;
       patch = patch + customKey -'0';
      }
      if (customKey == '#' )
      {
        
        if(patch < 128 && patch >= 0)
        {
         //do stuff with value
         //for demonstration purposes, for now we'll just print it
         talkMIDI(0xc0, patch, 0);
         Serial.println(patch);
        }
        patch=0;//Now reset ready for next input
        for(int index = 0; index <= 27; index = ++index)
        // step through the LEDs, from 0 to 7
        {
          digitalWrite(ledPins[index], HIGH);  // turn LED on
        }
          delay(1000);
        for(int index = 0; index <= 27; index = ++index)
        // step through the LEDs, from 0 to 7
        {
          digitalWrite(ledPins[index], LOW);
        }
        
        
       }
        
    } 
   if( !bs11 && !br11 ){
     //bank 0x78(drum)
     talkMIDI(0xb0, 0, 0x78);
     talkMIDI(0xb0,20, 0);
     talkMIDI(0xc0, patch, 0);     
     bs11 = true;
   }else if(bs11 && br11){
     bs11 = false;
   }   
   
   if( !bs12 && !br12 ){
     //bank MSB 0, default instruments
     patch = defaultPatch;
     talkMIDI(0xb0, 0, 0);
     talkMIDI(0xb0,20, 0);
     talkMIDI(0xc0, patch, 0);       
     bs12 = true;
   }else if(bs12 && br12){
     bs12 = false;
   }   
            

  //*************** MIDI LOOPBACK ******************//
  if(Serial.available() > 0)
  {
    byteData =  Serial.read();
    mySerial.write( byteData);
  }  
  
//oneAfterAnother();  // Light up all the LEDs in turn
  //oneOnAtATime(); // Turn on one LED at a time
// pingPong();  // change direction
  //marquee(); // Chase lights3
 //randomLED();  // Blink LEDs randomly
//  midiLED();
  
     

}

//노트 온 미디 메세지를 송출합니다. 버튼이 눌린상태와 같습니다.
//채널 범위는 0-15 입니다.
void noteOn(byte channel, byte note, byte attack_velocity) {
  talkMIDI( (0x90 | channel), note, attack_velocity);
}

//노트 오프 미디 메세지를 송출합니다. 버튼이 눌리지 않은 상태와 같습니다.
void noteOff(byte channel, byte note, byte release_velocity) {
  talkMIDI( (0x80 | channel), note, release_velocity);
}


void talkMIDI(byte cmd, byte data1, byte data2) {
  digitalWrite(ledPin, HIGH);
  mySerial.write(cmd );
  mySerial.write(data1 );

  //모든 명령은 1바이트를 지니며, 모든 cmds는   0xBn보다  2 데이터 바이트를 덜 지닙니다.  
  //(sort of: http://253.ccarh.org/handout/midiprotocol/)
  if( (cmd & 0xF0) <= 0xB0)
    mySerial.write(data2 );

  digitalWrite(ledPin, LOW);
}

void theaterChecker(){
  int index, i;
  //BUTTON PRESSED
  int buttonarray[] = {11,10,9,8,7,6,5,2};
  /*for(i = 0; i < 8; i++){
    if(digitalRead(buttonarray[i]) == 0){
      theaterChase(strip.Color(127, 0, 0), 50);
    }
    */
  if(digitalRead(buttonarray[0]) == 0){
    
    theaterChase(strip.Color(127, 0, 0), 50);
  }

  /*else{
    theaterChase(strip.Color(127, 127, 127), 50);
  }
  */
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j< 7; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();
      delay(wait);
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}
