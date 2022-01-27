/*
  ───────────────────────────────────────────────────────────────────────────────────────────────────
  ─██████████████────────██████──██████────────██████──────────██████────────██████──████████────────
  ─██░░░░░░░░░░██────────██░░██──██░░██────────██░░██████████████░░██────────██░░██──██░░░░██────────
  ─██░░██████████────────██░░██──██░░██────────██░░░░░░░░░░░░░░░░░░██────────██░░██──██░░████────────
  ─██░░██────────────────██░░██──██░░██────────██░░██████░░██████░░██────────██░░██──██░░██──────────
  ─██░░██████████────────██░░██──██░░██────────██░░██──██░░██──██░░██────────██░░██████░░██──────────
  ─██░░░░░░░░░░██────────██░░██──██░░██────────██░░██──██░░██──██░░██────────██░░░░░░░░░░██──────────
  ─██████████░░██────────██░░██──██░░██────────██░░██──██████──██░░██────────██░░██████░░██──────────
  ─────────██░░██────────██░░░░██░░░░██────────██░░██──────────██░░██────────██░░██──██░░██──────────
  ─██████████░░██─██████─████░░░░░░████─██████─██░░██──────────██░░██─██████─██░░██──██░░████─██████─
  ─██░░░░░░░░░░██─██░░██───████░░████───██░░██─██░░██──────────██░░██─██░░██─██░░██──██░░░░██─██░░██─
  ─██████████████─██████─────██████─────██████─██████──────────██████─██████─██████──████████─██████─
  ───────────────────────────────────────────────────────────────────────────────────────────────────
  Дозиметр на базе Atmega328p и СМБ-20/СТС-5
  Дисплей: Oled 128x32
  Функции:
   - Смена единиц измерения (Зв/ч, Р/ч);
   - Изминение яркости экрана;
   - Выключение звука
   - Смена уровня оповещения
   - Сброс Максимума/Накопленной дозы
   - Удоброе переключение режимов отображения
*/

//-----------------------Настройки----------------------------------
#define interval  600  //Период для датчика
#define volt_min  3.2    //Минимальное напряжение на аккумуляторе 
#define volt_max  4.2    //Максимальное напряжение на аккумуляторе
#define volt_5v   5.2    //Рельное напряжение на плате по линии 5в
//----------------------------------------------------------------


#include <Wire.h>
#include <GyverOLED.h>
#include "EEPROM.h"
#include <GyverButton.h>
GyverOLED<SSD1306_128x32, OLED_BUFFER> oled;
#define ITEMS 8               // Общее кол во пунктов

const char i0[] PROGMEM = "  Change unit";  // Каждое имя в своей строке, символ переноса не нужен
const char i1[] PROGMEM = "  Reset Max";
const char i2[] PROGMEM = "  Reset Dose";
const char i3[] PROGMEM = "  Alert level";
const char i4[] PROGMEM = "  Mute clicks";
const char i5[] PROGMEM = "  Brightness";
const char i6[] PROGMEM = "  Settings Reset";
const char i7[] PROGMEM = "  Quit";

const char* const names[] PROGMEM =           // Создаем массив ссылок, обращение к названию пунктов по их номеру
{
  i0, i1, i2, i3,
  i4, i5, i6, i7,
};

const uint8_t logo[] PROGMEM = {
  0x00, 0x80, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0x00, 0xC0, 0xC0, 0xC0, 0xC0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0xC0, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0xC0, 0xC0, 0xC0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xC0, 0xC0, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0x80, 0xE0, 0xF0, 0xF8, 0xFC, 0xFC, 0xF8, 0xE0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xE0, 0xF8, 0xFC, 0xF8, 0xF8, 0xF0, 0xE0, 0x80, 0x00, 0x00, 0x00, 
  0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x0F, 0x1F, 0x7F, 0xFE, 0xF8, 0xE0, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0xF0, 0xFC, 0xFF, 0x7F, 0x1F, 0x07, 0x01, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFE, 0xFC, 0xF8, 0xF0, 0xE0, 0xC0, 0xC0, 0x80, 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE, 0xFE, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFE, 0xBF, 0x1F, 0x0F, 0x07, 0x01, 0x00, 0x00, 0x00, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x1F, 0x06, 0xF0, 0xF0, 0xF8, 0xF8, 0xF0, 0x60, 0x07, 0x1F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3E, 0x00, 0x00, 
  0x00, 0x80, 0xE0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xFF, 0xFF, 0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x07, 0x1F, 0x7F, 0xFF, 0xFC, 0xF0, 0xF8, 0xFE, 0xFF, 0x3F, 0x0F, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7F, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x01, 0x03, 0x07, 0x0F, 0x0F, 0x0F, 0x07, 0x03, 0x01, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xF8, 0xFC, 0x7E, 0x3F, 0x0F, 0x07, 0x03, 0x07, 0x1F, 0x3F, 0x7E, 0xFC, 0xF8, 0xE0, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xC0, 0xF0, 0xFC, 0xFC, 0xFD, 0xFD, 0xFC, 0xF8, 0xF0, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x07, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x07, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
};




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
GButton up(8,  HIGH_PULL, NORM_OPEN);
GButton ok(9,  HIGH_PULL, NORM_OPEN);
GButton down(10, HIGH_PULL, NORM_OPEN);

uint32_t countimer;
uint32_t disptimer;
uint32_t sound;
uint32_t silence;
int oldbt;
byte scr = 0;
byte bright = 0;
int alarm;
bool mute = 0;
bool menu = 0;
int transition = 20;
uint32_t data[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                       Sum = 0, Sum_filt = 0;
volatile uint32_t bt = 0;
float input_voltage = 3.0, line1 = 0.0, doza = 0, doz_max = 0, FILTER_COEF = 0.2;
bool s1 = 0;
bool unit = 1, flag_doza = 0, flag_max = 0, meter = 0, raspad = 0;
//-----------------SETUP----------------------------------
void setup() {
  pinMode (7, OUTPUT); // buzzer
  EEPROM.get(0, unit);
  EEPROM.get(5, doza);
  EEPROM.get(10, doz_max);
  EEPROM.get(15, alarm);
  EEPROM.get(25, bright);
  pinMode(11, OUTPUT);
  digitalWrite(11, 0);
  oled.init();
  oled.setContrast(bright);
  Wire.setClock(400000L); 
  attachInterrupt(0, schet, FALLING);
  attachInterrupt(1, schet, FALLING);
  oled.clear();
  oled.drawBitmap(0, 0, logo, 128, 32);
  oled.update();
  delay(500);
  oled.clear();
  batt();
pinMode(8, INPUT_PULLUP);

  
}
//--------------------LOOP-------------------------------
void loop() {
  static int8_t pointer = 0;
  up.tick();
  ok.tick();
  down.tick();
  //------------------Заходим в меню-----------------------//
  if (ok.isHold() and menu == 0) {
    menu = 1;
    oled.setScale(1);
    pointer = 0;
  }
  //------------------Переключаем экраны---------------------//
  if (up.isClick() and menu == 0) {
    scr++;
    if (scr == 4)scr = 0;
    if (scr == 0) {
      raspad = 0;
      flag_max = 0;
      flag_doza = 0;
      meter = 0;
    }
    else if (scr == 1) {
      flag_doza = 1;
    }
    else if (scr == 2) {
      flag_max = 1;
      flag_doza = 0;
    }
    else if (scr == 3) {
      meter = 1;
      flag_max = 0;
    }
  }

  if (!menu) {
    if (down.isClick()) {
      res(0);
    }
  }
  //------------------Отправляем данные на подсчет---------------------//
  if (millis() - countimer >= interval) {
    countimer = millis();
    math(bt);
    bt = 0;
    oldbt=0;
  }
  if (millis() - disptimer >= 200) {
    disptimer = millis();
    disp();
  }
  //------------------------------------Звук---------------------------------------//
    if (Sum >= alarm) {
      if (millis() - sound >= 100) {
        sound = millis();
        noTone(7);
      }
      if (millis() - silence >= 1000) {
        silence = millis();
        tone(7, 4000);
      }
      else if (Sum < alarm) {
        noTone(7);
      }
    }
    if (mute == 0) {
    if (oldbt != bt){
   digitalWrite (13, HIGH);
     tone(7, 5000);
   digitalWrite (13, LOW); 
    noTone(7);
  oldbt = bt;
  }

  }
  
  //--------------------------------Менюшка----------------------------------------

  if (menu == 1) {
    oled.clear();
    oled.home(); 
  up.tick();                 // Опрос кнопок
  down.tick();
  ok.tick();
  
  if (down.isClick() or down.isHold()) {
    pointer = constrain(pointer + 1, 0, ITEMS - 1);
  }
  if (!digitalRead(8)) {                // Если кнопку нажали или удерживают
    pointer = constrain(pointer - 1, 0, ITEMS - 1); // Двигаем указатель в пределах дисплея
 delay(100);
  }

    if (ok.isClick()) {
      switch (pointer) {
        case 0: cunit(); break;
        case 1: resmax(); break;
        case 2: resdose(); break;
        case 3: setalarm(); break;
        case 4: onmute(); break;
        case 5: brig(); break;
        case 6: resetall(); break;
        case 7: menu = 0; break;
      }
    }

   for (uint8_t i = 0; i < 4; i++) { // Цикл, выводящий 8 пунктов на дисплей
    printMenuItem(pointer > 3 ? i + (pointer - 3) : i); // Выводим пункты + скроллинг при достижении последней строки указателем
  }                                                     // Если указатель > 7, то сдвигаем список пунктов на pointer - 7
  printPointer(constrain(pointer, 0, 3));               // Вывод указателя, в пределах 0-7 (не даем переполняться)
  oled.update();                    // Выводим кадр на дисплей
  }
}
void printPointer(uint8_t pointer) {
 oled.setCursor(0, pointer);
 oled.print(">");
}


//---------------------------Подсчет импульсов-----------------------------------------------------------------------------
void schet()
{
  bt++;
  s1 = 1;
}
void printMenuItem(uint8_t num) {
  char buffer[21];                                // Буфер на полную строку
  uint16_t ptr = pgm_read_word(&(names[num]));    // Получаем указатель на первый символ строки
  uint8_t i = 0;                                  // Переменная - счетчик

  do {                                            // Начало цикла
    buffer[i] = (char)(pgm_read_byte(ptr++));     // Прочитать в буфер один символ из PGM и подвинуть указатель на 1
  } while (buffer[i++] != NULL);                  // Если это не конец строки - вернуться в начало цикла

  oled.println(buffer);                           // Вывод готовой строки с переносом на следующую
}
void resetall(void) {
  oled.clear();
  oled.home();
   oled.print(F("Уверены?"));
  oled.update();
  while (1) {
    ok.tick();
    if (ok.isClick()) {
      oled.clear();
      oled.home();
      oled.print(F("В процессе..."));
      oled.update();
     unit = 1;
     doza = 0;
     doz_max = 0;
     bright = 255;
     oled.setContrast(255);
     alarm = 30;
      EEPROM.put(15, alarm);
      EEPROM.put(10, doz_max);
      delay(150);
      EEPROM.put(5, doza);
      delay(150);
      EEPROM.put(0, unit);
      delay(150);
      EEPROM.put(25, bright);
      delay(150);
      oled.clear();
      oled.home();
      oled.print(F("Готово"));
      oled.update();
      delay(500);
      return;
    }
    if (ok.isHold()) {
      return;
    }
  }
}
//-----------------------------Смена единиц измерения------------------------------------------
void cunit(void) {
  oled.clear();
  oled.home();
  oled.print("Сменить ед.измерения?");
  oled.update();
  while (1) {
    ok.tick();
    if (ok.isClick()) {
      unit = !unit;
      EEPROM.put(0, unit);
      oled.clear();
      oled.home();
      oled.print(F("Готово"));
      oled.update();
      delay(500);
      return;
    }
    if (ok.isHold()) {
      return;
    }
  }
}
void brig(void) {
  byte br = bright;
  while (1) {
    ok.tick();
    up.tick();
    down.tick();
    oled.clear();
    oled.home();
    oled.print("Яркость: (0 - 255):");
    oled.setCursor(50, 2);
    oled.print(br);
    oled.update();

    if (up.isClick() or up.isHold()){
      if(br<=250)
      br += 5; 
    }
    if (down.isClick() or down.isHold())br -= 5; 
br = constrain(br, 5, 255);
     oled.setContrast(br);
    if (ok.isClick()) {
      oled.clear();
      oled.home();
      bright = br;
      EEPROM.put(25, bright);
      oled.print(F("Готово"));
      oled.update();
      delay(500);
      return;
    }
    if (ok.isHold()) {
      return;
    }
  }
}

//------------------------------Обнуление максимума----------------------------------------------------------
void resmax(void) {
  oled.clear();
  oled.home();
  oled.print(F("Уверены?"));
  oled.update();
  while (1) {
    ok.tick();
    if (ok.isClick()) {
      doz_max = 0;
      EEPROM.put(10, doz_max);
      oled.clear();
      oled.home();
      oled.print(F("Готово"));
      oled.update();
      delay(500);
      return;
    }
    if (ok.isHold()) {
      return;
    }
  }
}
//------------------------------Обнуление дозы----------------------------------------------------------
void resdose(void) {
  oled.clear();
  oled.home();
  oled.print(F("Уверены?"));
  oled.update();
  while (1) {
    ok.tick();
    if (ok.isClick()) {
      doza = 0;
      EEPROM.put(5, doza);
      oled.clear();
      oled.home();
      oled.print(F("Готово"));
      oled.update();
      delay(500);
      return;
    }
    if (ok.isHold()) {
      return;
    }
  }
}
//----------------------------Выбор фона оповещения------------------------------------------------------------
void setalarm(void) {
  int al = alarm;
  while (1) {
    ok.tick();
    up.tick();
    down.tick();
    oled.clear();
    oled.home();
    oled.print("Оповещение от: (мкР)");
    oled.setCursor(50, 2);
    oled.print(al);
    oled.update();

    if (up.isClick() or up.isHold())al += 10;
    if (down.isClick() or down.isHold())al -= 10;
 al = constrain(al, 30, 10000);
    
    if (ok.isClick()) {
      oled.clear();
      oled.home();
      alarm = al;
      EEPROM.put(15, alarm);
      oled.print(F("Готово"));
      oled.update();
      delay(500);
      return;
    }
    if (ok.isHold()) {
      return;
    }
  }
}
//---------------------------------Выбор яркости экрана-------------------------------------------------------

//--------------------------------Выключиение звука--------------------------------------------------------
void onmute(void) {
  oled.clear();
  oled.home();
  if (!mute) {
    oled.print("Выключить щелчки?");
  }
  if (mute) {
    oled.print("Включить щелчки?");
  }
  oled.update();
  while (1) {
    ok.tick();
    if (ok.isClick()) {
      mute = !mute;
      oled.clear();
      oled.home();
      oled.print(F("Готово"));
      oled.update();
      delay(500);
      return;
    }
    if (ok.isHold()) {
      return;
    }
  }
}

//-------------------------------------------------------------------------------------

void math(unsigned long kol)
{
  static uint32_t writedelay;
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

  if (millis() - writedelay >= 60000) {
    writedelay = millis();
    EEPROM.put(5, doza);
    EEPROM.put(10, doz_max);
  }
}
//-------------------------------------------------------------------------------------
void disp() //ТЕКСТ
{
  if(!menu){
batt();
oled.clear();
  oled.setScale(2);
  oled.setCursorXY(10, 1);
  
  if (flag_doza) {
     oled.clear();
    oled.setScale(1);
    oled.home();
    oled.print(F("Доза:"));
    oled.setScale(2);
    if (unit) {
      oled.setCursorXY(5, 15);
      float doza_zv = doza / 114.025;
      if (doza == 0) {
        oled.print("---");
       oled.setCursorXY(70, 15);
        oled.setScale(2);
        oled.print("мкЗв");
      }
      else if (doza_zv < 10) {
        oled.print(doza_zv);
        oled.setCursorXY(75, 15);
        oled.print("мкЗв");
      }
      else if (doza_zv < 100) {
        oled.print(doza_zv, 1);
        oled.setCursorXY(75, 15);
        oled.print("мкЗв");
      }
      else if (doza_zv < 1000) {
        oled.print(round(doza_zv));
       oled.setCursorXY(75, 15);
        oled.print("мкЗв");
      }
      else {
        oled.print(round(doza_zv / 1000));
        oled.setCursorXY(75, 15);
        oled.print("мЗв");
      }
    }
    else {
      oled.setCursorXY(5, 15);
      if (round(doza) == 0) {
        oled.print("---");
       oled.setCursorXY(75, 15);
        oled.print("мкР");
      }
      else if (doza < 1000) {
        oled.print(round(doza));
        if (doza < 10) oled.clear();
        else if (doza < 100);
        else oled.clear();
        oled.setCursorXY(75, 15);
        oled.print("мкР");
      }
      else if (doza < 1000000) {
        oled.print(round((float)doza / 1000));
        oled.setCursorXY(75, 9);
        oled.print("мР"); oled.clear();
      }
      else {
        oled.print((float)doza / 1000000);
        oled.setCursorXY(75, 15);
        oled.print("Р");
      }
    }
    oled.update();
 
  } else if (flag_max) {
      oled.clear();
    oled.setScale(1);
    oled.home();
    oled.print("Макс:");
    oled.setScale(2);
    if (unit) {
      oled.setCursorXY(5, 15);
      float dozm_zv = doz_max / 100.0;
      if (dozm_zv == 0) {
        oled.print("---");
        oled.setCursorXY(75, 15);
        oled.print("мкЗв");
      }
      else if (dozm_zv < 10) {
        oled.print(dozm_zv);
        oled.setCursorXY(75, 15);
        oled.print("мкЗв");
      }
      else if (dozm_zv < 100) {
        oled.print(dozm_zv, 1);
        oled.setCursorXY(75, 15);
        oled.print("мкЗв");
      }
      else if (dozm_zv < 1000) {
        oled.print(round(dozm_zv));
        oled.setCursorXY(75, 15);
        oled.print("мкЗв");
      }
      else {
        oled.print(round(dozm_zv / 1000));
        oled.setCursorXY(75, 15);
        oled.print("мЗв");
      }
    }
    else {
      oled.setCursorXY(5, 15);
      if (doz_max == 0) {
        oled.print("---");
        oled.setCursorXY(75, 15);
        oled.print("мкР");
      }
      else if (doz_max < 1000) {
        oled.print(doz_max);
        if (doz_max < 10) oled.clear(20, 16, 56, 32);
        else if (doz_max < 100) oled.clear(32, 16, 56, 32);
        else oled.clear(45, 16, 56, 32);
        oled.setCursorXY(75, 15);
        oled.print("мкР");
      }
      else if (doz_max < 100000) {
        if (doz_max < 10000) oled.print((float)doz_max / 1000);
        else oled.print(((float)doz_max / 1000), 1);
        oled.setCursorXY(75, 15);
        oled.print("мР"); oled.clear(108, 16, 120, 32);
      }
      else if (doz_max < 1000000) {
        oled.print(round((float)doz_max / 1000)); oled.clear(45, 16, 56, 32);
        oled.setCursorXY(75, 15);
        oled.print("мР"); oled.clear(108, 16, 120, 32);;
      }
      else {
        oled.print((float)doz_max / 1000000);
        oled.setCursorXY(75, 15);
        oled.print("Р"); oled.clear(98, 16, 120, 32);
      }
    }
    oled.update();


  } else {
    if (unit) {
      float Sum_zv = Sum_filt / 114.025;
      if (Sum_zv == 0) {
        oled.print("---");
        oled.setCursorXY(14, 16);
        oled.print("мкЗв/ч");
      }
      else if (Sum_zv < 10) {
        oled.print(Sum_zv);
        oled.setCursorXY(14, 16);
        oled.print("мкЗв/ч");
      }
      else if (Sum_zv < 100) {
        oled.print(Sum_zv, 1);
        oled.setCursorXY(14, 16);
        oled.print("мкЗв/ч");
      }
      else if (Sum_zv < 1000) {
        oled.print(round(Sum_zv)); oled.clear(45, 0, 56, 14);
        oled.setCursorXY(14, 16);
        oled.print("мкЗв/ч");
      }
      else {
        oled.print(round(Sum_zv / 1000));
        oled.setCursorXY(14, 16);
        oled.print("мЗв/ч"); oled.clear(58, 16, 68, 32);
      }
    }
    else {
      if (Sum_filt == 0) {
        oled.print("---"); oled.clear(45, 0, 56, 14);
        oled.setCursorXY(14, 16);
        oled.print("мкР/ч");
      }
      else if (Sum_filt < 1000) {
        if (Sum_filt < 10) oled.clear(20, 0, 56, 14);
        else if (Sum_filt < 100) oled.clear(32, 0, 56, 14);
        else oled.clear(45, 0, 56, 14);
        oled.setCursorXY(10, 0);
        oled.print(Sum_filt);
        oled.setCursorXY(14, 16);
        oled.print("мкР/ч");
      }
      else if (Sum_filt < 100000) {
        if (Sum_filt < 10000) oled.print((float)Sum_filt / 1000);
        else oled.print(((float)Sum_filt / 1000), 1);
        oled.setCursorXY(14, 16);
        oled.print("мР/ч"); oled.clear(58, 16, 68, 32);
      }
      else if (Sum_filt < 1000000) {
        oled.print(round((float)Sum_filt / 1000)); oled.clear(45, 0, 56, 14);
        oled.setCursorXY(14, 16);
        oled.print("мР/ч");
      } else {
        oled.print(Sum_filt / 1000000);
        oled.setCursorXY(14, 16);
        oled.print("Р/ч"); oled.clear(48, 16, 68, 32);
      }
    }
    oled.fastLineV(0, 32 - round(line1), 32);
    oled.fastLineV(1, 32 - round(line1), 32);
    if (meter) {
      oled.drawBitmap(60, 1, met, 25, 8);
      //oled.clear(68, 15, 81, 18);
    }
    else oled.drawBitmap(68, 0, lupa, 13, 16);

    if (s1 == 1) oled.drawBitmap(87, 0, radiation, 31, 31);
    else oled.clear(87, 1, 118, 32);

      batt();
    

    //ВЫВОД ИЗОБРАЖЕНИЯ
    s1 = 0;
    oled.update();
  }
}
}
void res(byte n) //обнуление значений
{
  if (n) {
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
//--------------------------------------------------------------------------------------------------------
void batt() {//ИНДИКАТОР ЗАРЯДА
  input_voltage = (analogRead(A3) * volt_5v) / 1023.0; //вольты
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
    delay(2000);
    oled.clear(3, 0, 120, 32);
    }
}
//--------------------------------------------------------------------------------------------------------
