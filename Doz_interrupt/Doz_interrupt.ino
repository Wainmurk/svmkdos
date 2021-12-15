
#include <Wire.h>
#include <GyverOLED.h>
#include <GyverButton.h>
#include "EEPROMex.h"
#include <TimerMs.h>
TimerMs tmr(1);
#define interval  1800  //Период для датчика
#define volt_min  3.2    //Минимальное напряжение на аккумуляторе
#define volt_max  4.2    //Максимальное напряжение на аккумуляторе
#define volt_5v   5.2    //Рельное напряжение на плате по линии 5в

#define DSP       0      //Если дисплей 128*64
GyverOLED<SSD1306_128x32, OLED_NO_BUFFER> oled;

const uint8_t logo[] PROGMEM = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x1C, 0x7E, 0xFE, 0xFE, 0xFE, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFE, 0xFE, 0x7E, 0x1C, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0xF8, 0xFE, 0xFE, 0xFF, 0xCF, 0xC7, 0x87, 0x87, 0x8F,
0x3F, 0x3F, 0x3F, 0x3E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00,
0xC0, 0xF8, 0xFF, 0xFF, 0x7F, 0x0F, 0x03, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xF8, 0xC0, 0x80, 0xC0, 0xF0, 0xF8, 0xFE, 0xFF, 0xFF,
0xFF, 0xFF, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0xF8, 0xFF, 0xFF, 0xFF, 0x8F, 0x80, 0xC0, 0xF0,
0xFC, 0xFE, 0x3F, 0x1F, 0x07, 0x03, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x01, 0x07, 0x1F, 0x9F, 0xCF, 0xCF, 0xCF,
0xCF, 0x9F, 0x1F, 0x07, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0xF8,
0xF9, 0xF9, 0xE3, 0xC3, 0xC3, 0xC7, 0xE7, 0xFF, 0xFF, 0xFF,
0x7E, 0x00, 0x00, 0xE0, 0xE0, 0xE0, 0xE0, 0x00, 0x00, 0x00,
0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0xF0, 0xFE, 0xFF, 0x7F,
0x0F, 0x03, 0x00, 0x00, 0xE0, 0xE0, 0xE0, 0xE0, 0x00, 0x00,
0x00, 0xC0, 0xFE, 0xFF, 0xFF, 0x7F, 0x03, 0x01, 0x1F, 0x1F,
0x1F, 0x1F, 0x0F, 0x03, 0xE1, 0xFF, 0xFF, 0xFF, 0x3F, 0x01,
0x00, 0xC0, 0xE0, 0xE0, 0xE0, 0xE0, 0x00, 0x00, 0xC0, 0xFE,
0xFF, 0xFF, 0x7F, 0x07, 0x07, 0x07, 0x7F, 0xFF, 0xFF, 0xFC,
0xE0, 0x00, 0x00, 0x00, 0xE0, 0xE0, 0xE0, 0xE0, 0x00, 0x00,
0x1E, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE,
0xFE, 0xFE, 0xF0, 0xE3, 0x4F, 0x0F, 0x1F, 0x1F, 0x1F, 0xCF,
0xE7, 0xF8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0x1F, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00,
0x00, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00,
0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01,
0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01,
0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01,
0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01,
0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00,
0x03, 0x0F, 0x1F, 0x3F, 0x7F, 0x7F, 0xFF, 0x7F, 0x1F, 0x0F,
0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x07,
0x1F, 0x3F, 0x7F, 0xFF, 0xFF, 0x7F, 0x3F, 0x1F, 0x0F, 0x03,
0x00, 0x00};
const uint8_t radiation[] PROGMEM = {
  0x00, 0x00, 0x80, 0x40, 0x20, 0x90, 0xC8, 0xE4, 0xF4, 0xC2, 0x02, 0x02, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x02, 0xC2, 0xF4, 0xE4, 0xC8, 0x90, 0x20, 0x40, 0x80, 0x00, 0x00, 
  0xF0, 0x0E, 0xE1, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0x10, 0xC0, 0xE0, 0xE0, 0xE0, 0xC0, 0x10, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xE1, 0x0E, 0xF0, 
  0x07, 0x38, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xE0, 0xF9, 0xFB, 0xFB, 0xFB, 0xF9, 0xE0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x38, 0x07, 
  0x00, 0x00, 0x00, 0x01, 0x02, 0x04, 0x08, 0x10, 0x10, 0x20, 0x2E, 0x2F, 0x4F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x4F, 0x2F, 0x2E, 0x20, 0x10, 0x10, 0x08, 0x04, 0x02, 0x01, 0x00, 0x00, 0x00, 
};
const uint8_t met[] PROGMEM = {
  0xFF, 0x81, 0xE1, 0x81, 0xE1, 0x81, 0xF9, 0x81, 0xE1, 0x81, 0xE1, 0x81, 0xF9, 0x81, 0xE1, 0x81, 0xE1, 0x81, 0xF9, 0x81, 0xE1, 0x81, 0xE1, 0x81, 0xFF,
};
const uint8_t lupa[] PROGMEM = {
  0x38, 0xEE, 0x82, 0x83, 0x09, 0x9B, 0x82, 0xEE, 0xB8, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x00, 0x01, 0x03, 0x07, 0x0E, 0x1C, 0x18,
};
GButton butt1(10, HIGH_PULL, NORM_OPEN);
GButton butt2(9,  HIGH_PULL, NORM_OPEN);
unsigned long previousMillis = 0, pm = 0,pm1 =0,pm2=0,pm3=0, previousMillis1 = 0;
long bat_timer = -10000;
long interval1 = 200;
int alarm = 100;
int mute = 0;
int oldbt;
int transition = 20;
unsigned long data[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
              Sum = 0, Sum_filt = 0, doz_max = 0;
volatile unsigned long  bt = 0;
volatile bool flag;
float input_voltage = 3.0, line1 = 0.0, doza = 0, FILTER_COEF = 0.2;
bool s1 = 0, unit = 0, flag_doza = 0, flag_max = 0, meter = 0, clr = 0, raspad = 0;
void setup() {
   Serial.begin(9600);
  butt1.setTimeout(1000);    // настройка таймаута на удержание (по умолчанию 500 мс)
  //pinMode(2 , INPUT_PULLUP); // вход на пине 2
pinMode (7, OUTPUT); // buzzer
pinMode (3, OUTPUT); // buzzer
digitalWrite(3, LOW);
tmr.start();
 doza=EEPROM.readFloat(4);
  doz_max=EEPROM.readFloat(2);
  unit = EEPROM.readInt(3);
  attachInterrupt(0, schet, FALLING);
  oled.init();              // инициализация
  Wire.setClock(400000L);   // макс. 800'000
  //oled.setPower(0); для будущего :)
  //oled.setContrast(255);
  if (DSP) {
    oled.clear(0, 0, 128, 64);
    oled.drawBitmap(0, 16, logo, 128, 32);
  }
  else {
    oled.clear(0, 0, 128, 32);
    oled.drawBitmap(0, 0, logo, 128, 32);
  }
  oled.update();
  delay(1000);
  if (DSP) oled.clear(0, 0, 128, 64);
  else oled.clear(0, 0, 128, 32);
  //Serial.begin(9600);
batt();
}

void loop() {
  butt1.tick();
  butt2.tick();
  if (butt1.isSingle()) {
    flag_doza = !(flag_doza);
    flag_max = 0;
    clr = 1;
  }
  if (butt1.isTriple()) {
  doza=0;
  }
   if (butt2.isTriple()) {
     doz_max = 0;
    EEPROM.writeFloat (2, 0);
  }
  if (butt1.isDouble() && (!raspad)) {
    unit = !unit;
    EEPROM.writeInt(3, unit);
    clr = 1;
  }
  if (butt1.isHolded()) {
    if (flag_max) res(1);
    else res(0);
  }
  if (butt2.isSingle()) {
    flag_max = !(flag_max);
    flag_doza = 0;
    clr = 1;
  }
  if (butt2.isDouble() && (!raspad)) {
    meter = !meter;
    res(0);
    clr = 1;
  }
  if (butt2.isHolded()) {
    raspad = !raspad;
    flag_doza = 0;
    flag_max = 0;
    clr = 1;
  } 
if (oldbt != bt){
   digitalWrite (13, HIGH);
     tone(7, 5000);
   digitalWrite (13, LOW); 
    noTone(7);
  oldbt = bt;
  }
if (Serial.available() > 0 ){
  String buf = Serial.readString();
if (buf.startsWith("part.set")) {
  int newrad = buf.substring(8).toInt();
    bt = newrad;
    }   
else if (buf.startsWith("mute")) {
    int mut = buf.substring(5).toInt();
    if(mut == 1){mute = 1;}else{mute = 0;}
    } 
 else if (buf.startsWith("dose.reset")) {
    doza = 0;
  } 
else if (buf.startsWith("dose.set")) {
  int dos = buf.substring(9).toInt();
    doza = dos;
  } 
  else if (buf.startsWith("alarm.set")) {
  int al = buf.substring(9).toInt();
    alarm = al;
  } 
   else if (buf.startsWith("max.reset")) {
    doz_max = 0;
    EEPROM.writeFloat (2, 0);
  }
}
  if(millis() - pm1 > 2000) {
    pm1 = millis(); 
Serial.print("Фон: ");
Serial.print(Sum / 114.02509);
Serial.print(" µSv/h");
Serial.println("");
Serial.print("Частиц: ");
Serial.print(bt);
Serial.println("");
Serial.print("Накоплено:");
Serial.print(doza / 114.02509);
Serial.println(" µSv");
Serial.println("");
}
  if (millis() - previousMillis >= interval) {
    previousMillis = millis();
    math(bt);
    bt = 0;
    oldbt = 0;
    if (DSP) disp64();
  }

  if (millis() - previousMillis1 >= interval1) {
    previousMillis1 = millis();
    disp();
  }
  if(mute == 0){
if (Sum >= alarm){
  if(millis() - pm2 > 100) {
    pm2 = millis(); 
 noTone(7);
  }
    if(millis() - pm3 > 1000) {
    pm3 = millis(); 
  tone(7, 4000);
}
else if(Sum < alarm){noTone(7);}
}}}
void schet() //подсчет срабатываний
{   
   if (tmr.tick()){
  detachInterrupt(0);
  bt++;
  s1 = 1;
  attachInterrupt(0, schet, FALLING);
  //_delay_ms(10);
}
}
void math(unsigned long kol) //математические вычисления
{
  if (raspad)
    transition = 20;
  else if (meter)
    transition = 40;
  else {
    if ((transition != 20) && (kol < 5) && (data[1] < 5))
    {
      transition = 20;
      res(0);
    }
    if ((transition != 5) && (data[1] > 10) && (kol > 10) && (kol < 50))
    {
      if (transition == 1) res(0);
      transition = 5;
    }
    if ((transition != 1) && (data[1] > 50) && (kol > 50))
    {
      transition = 1;
      res(0);
    }
  }
  Sum = 0;
  for (int i = 39; i > 0; i--) data[i] = data[i - 1];
  data[0] = kol;

  if (transition != 1)
    for (int j = 0; j < transition; j++) {
      Sum = Sum + data[j];
    }
  if (transition == 20) {
    if (abs(Sum - Sum_filt) > 10) FILTER_COEF = 0.5;
    else FILTER_COEF = 0.2;
    if (line1 < 33) line1 += 1.6;
  }
  else if (transition == 5) {
    FILTER_COEF = 0.5;
    if (line1 < 33) line1 += 6.4;
    Sum = Sum * 4;
  }
  else if (transition == 1) {
    FILTER_COEF = 0.7;
    line1 = 32;
    Sum = kol * 20;
  }
  else {
    if (line1 < 33) line1 += 0.8;
    Sum = round(Sum / 2);
  }

  if (meter) Sum_filt = Sum;
  else Sum_filt = Sum * FILTER_COEF + Sum_filt * (1 - FILTER_COEF);

  if (doz_max < Sum) doz_max = Sum;
  doza = doza + (float)kol * 20 * interval / 3600000;
  
EEPROM.writeFloat(4, doza);
EEPROM.writeFloat(2, doz_max);
}
void res(byte n) //обнуление значений
{
  if (n){
    doz_max = 0;
    doza = 0;
  }
  else {
    for (int i = 0; i < 40; i++) data[i] = 0;
    line1 = 0;
    Sum = 0;
    Sum_filt = 0;
  }
  oled.clear(0, 0, 2, 32);
}
void disp() //ТЕКСТ
{
  if (clr) {
    if (DSP) oled.clear(0, 0, 128, 64);
    else oled.clear(0, 0, 128, 32);
    batt();
    clr = 0;
  }
  oled.setScale(2);
  oled.setCursorXY(10, 0);
  if (flag_doza) {
    oled.print("Доза:");
    oled.setCursorXY(10, 18);
    if (unit) {
      float doza_zv = doza / 114.02509;
      if (doza == 0) {
        oled.print("---");
        oled.setCursorXY(50, 18);
        oled.print("мкЗв");
      }
      else if (doza_zv < 10) {
        oled.print(doza_zv);
        oled.setCursorXY(60, 18);
        oled.print("мкЗв");
      }
      else if (doza_zv < 100) {
        oled.print(doza_zv, 1);
        oled.setCursorXY(60, 18);
        oled.print("мкЗв");
      }
      else if (doza_zv < 1000) {
        oled.print(round(doza_zv)); oled.clear(45, 0, 56, 14);
        oled.setCursorXY(60, 18);
        oled.print("мкЗв");
      }
      else {
        oled.print(round(doza_zv / 1000));
        oled.setCursorXY(50, 18);
        oled.print("мЗв"); oled.clear(48, 16, 68, 32);
      }
    }
    else {
      if (round(doza) == 0) {
        oled.print("---"); oled.clear(45, 16, 56, 32);
        oled.setCursorXY(60, 18);
        oled.print("мкР");
      }
      else if (doza < 1000) {
        oled.print(round(doza));
        if (doza < 10) oled.clear(20, 16, 56, 32);
        else if (doza < 100) oled.clear(32, 16, 56, 32);
        else oled.clear(45, 16, 56, 32);
        oled.setCursorXY(60, 18);
        oled.print("мкР");
      }
      else if (doza < 1000000) {
        oled.print(round((float)doza / 1000)); oled.clear(45, 16, 56, 32);
        oled.setCursorXY(60, 18);
        oled.print("мР"); oled.clear(98, 16, 110, 32);
      }
      else {
        oled.print((float)doza / 1000000);
        oled.setCursorXY(60, 18);
        oled.print("Р"); oled.clear(88, 16, 110, 32);
      }
    }
  } else if (flag_max) {
    oled.print("Макс:");
    oled.setCursorXY(10, 18);
    if (unit) {
      float dozm_zv = doz_max / 114.02509;
      if (dozm_zv == 0) {
        oled.print("---");
        oled.setCursorXY(60, 18);
        oled.print("мкЗвч");
      }
      else if (dozm_zv < 10) {
        oled.print(dozm_zv);
        oled.setCursorXY(60, 18);
        oled.print("мкЗвч");
      }
      else if (dozm_zv < 100) {
        oled.print(dozm_zv, 1);
        oled.setCursorXY(60, 18);
        oled.print("мкЗвч");
      }
      else if (dozm_zv < 1000) {
        oled.print(round(dozm_zv)); oled.clear(45, 16, 56, 32);
        oled.setCursorXY(60, 18);
        oled.print("мкЗвч");
      }
      else {
        oled.print(round(dozm_zv / 1000));
        oled.setCursorXY(50, 18);
        oled.print("мЗв/ч"); oled.clear(108, 16, 120, 32);
      }
    }
    else {
      if (doz_max == 0) {
        oled.print("---"); oled.clear(45, 16, 56, 32);
        oled.setCursorXY(60, 18);
        oled.print("мкР/ч");
      }
      else if (doz_max < 1000) {
        oled.print(doz_max);
        if (doz_max < 10) oled.clear(20, 16, 56, 32);
        else if (doz_max < 100) oled.clear(32, 16, 56, 32);
        else oled.clear(45, 16, 56, 32);
        oled.setCursorXY(60, 18);
        oled.print("мкР/ч");
      }
      else if (doz_max < 100000) {
        if (doz_max < 10000) oled.print((float)doz_max / 1000);
        else oled.print(((float)doz_max / 1000), 1);
        oled.setCursorXY(60, 18);
        oled.print("мР/ч"); oled.clear(108, 16, 120, 32);
      }
      else if (doz_max < 1000000) {
        oled.print(round((float)doz_max / 1000)); oled.clear(45, 16, 56, 32);
        oled.setCursorXY(60, 18);
        oled.print("мР/ч"); oled.clear(108, 16, 120, 32);;
      }
      else {
        oled.print((float)doz_max / 1000000);
        oled.setCursorXY(60, 18);
        oled.print("Р/ч"); oled.clear(98, 16, 120, 32);
      }
    }
  } else if (raspad) {
    if (Sum_filt == 0) {
      oled.print("---"); oled.clear(45, 16, 56, 32);
    }
    else {
      float raspadi_sec = (float)Sum_filt * 0.0133;
      if (raspadi_sec < 10) oled.print(raspadi_sec, 2);
      else if (raspadi_sec < 100) oled.print(raspadi_sec, 1);
      else {
        oled.print(round(raspadi_sec));
        oled.clear(45, 0, 56, 14);
      }
    }
    oled.setCursorXY(10, 18);
    oled.print("имп/см2*с");
  } else {
    if (unit) {
      float Sum_zv = Sum_filt / 114.02509;
      if (Sum_zv == 0) {
        oled.print("---"); oled.clear(44, 0, 55, 14);
        oled.setCursorXY(10, 18);
        oled.print("мкЗв/ч");
      }
      else if (Sum_zv < 10) {
        oled.print(Sum_zv);
        oled.setCursorXY(10, 18);
        oled.print("мкЗв/ч");
      }
      else if (Sum_zv < 100) {
        oled.print(Sum_zv, 1);
        oled.setCursorXY(10, 18);
        oled.print("мкЗв/ч");
      }
      else if (Sum_zv < 1000) {
        oled.print(round(Sum_zv)); oled.clear(45, 0, 56, 14);
        oled.setCursorXY(10, 18);
        oled.print("мкЗв/ч");
      }
      else {
        oled.print(round(Sum_zv / 1000));
        oled.setCursorXY(10, 18);
        oled.print("мЗв/ч"); oled.clear(58, 16, 68, 32);
      }
    }
    else {
      if (Sum_filt == 0) {
        oled.print("---"); oled.clear(45, 0, 56, 14);
        oled.setCursorXY(10, 18);
        oled.print("мкР/ч");
      }
      else if (Sum_filt < 1000) {
        if (Sum_filt < 10) oled.clear(20, 0, 56, 14);
        else if (Sum_filt < 100) oled.clear(32, 0, 56, 14);
        else oled.clear(45, 0, 56, 14);
        oled.setCursorXY(10, 0);
        oled.print(Sum_filt);
        oled.setCursorXY(10, 18);
        oled.print("мкР/ч");
      }
      else if (Sum_filt < 100000) {
        if (Sum_filt < 10000) oled.print((float)Sum_filt / 1000);
        else oled.print(((float)Sum_filt / 1000), 1);
        oled.setCursorXY(10, 18);
        oled.print("мР/ч"); oled.clear(58, 16, 68, 32);
      }
      else if (Sum_filt < 1000000) {
        oled.print(round((float)Sum_filt / 1000)); oled.clear(45, 0, 56, 14);
        oled.setCursorXY(10, 18);
        oled.print("мР/ч");
      } else {
        oled.print(Sum_filt / 1000000);
        oled.setCursorXY(10, 18);
        oled.print("Р/ч"); oled.clear(48, 16, 68, 32);
      }
    }
    oled.fastLineV(0, 32 - round(line1), 32);
    oled.fastLineV(1, 32 - round(line1), 32);
    if (meter) {
      oled.drawBitmap(60, 1, met, 25, 8);
      oled.clear(68, 15, 81, 18);
    }
    else oled.drawBitmap(68, 0, lupa, 13, 16);

    if (s1 == 1){  
      oled.drawBitmap(87, 0, radiation, 31, 31);}
    else oled.clear(87, 1, 118, 32);

    if (millis() - bat_timer > 120000) {
      bat_timer = millis();
      batt();
    }

    //ВЫВОД ИЗОБРАЖЕНИЯ
    s1 = 0;
    oled.update();
  }
}
void disp64() {
  long int maxx = 0;
  for (int i = 0; i < 40; i++)
    if (data[i] > maxx) maxx = data[i];
  static float adj;
  if (maxx > 0) adj = 23.0 / maxx;
  for (int i = 0; i < 40; i++) {
    oled.clear(map(i, 0, 40, 123, 2)-1 , 40, map(i, 0, 40, 123, 2), 63);
    oled.fastLineV(map(i, 0, 40, 123, 2), 63 - data[i]*adj, 63);
    oled.fastLineV(map(i, 0, 40, 123, 2) - 1, 63 - data[i]*adj, 63);
  }
}
void batt() {//ИНДИКАТОР ЗАРЯДА
  input_voltage = (analogRead(A3) * volt_5v) / 1023.0; //вольты
  oled.setScale(2);
  oled.clear(124, 3, 125, 31);
  oled.rect(122, 2, 127, 31, OLED_STROKE);
  oled.rect(123, 0, 126, 1, OLED_FILL);
  if (input_voltage > volt_min)
    oled.rect(124, 31 - map(input_voltage*10, volt_min*10, volt_max*10, 0, 31), 125, 31, OLED_FILL);
  else {
    oled.clear(3, 0, 120, 32);
    oled.setCursorXY(32, 0);
    oled.print("Аккум.");
    oled.setCursorXY(12, 18);
    oled.print("разряжен!");
tone(7, 3000);
delay(150);
noTone(7);
tone(7, 2000);
delay(300);
noTone(7);
delay(1500);
    oled.clear(3, 0, 120, 32);
    }
}
