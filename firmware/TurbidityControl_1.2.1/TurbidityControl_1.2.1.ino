#define VERSION "1.2.1"                           //   Версия прошивки

// ================== БИБЛИОТЕКИ ===============
#include <Wire.h>                                 //   Подключаем библиотеку для работы с аппаратной шиной I2C
#include <microDS3231.h>                          //   Подключаем библиотеку для работы с RTC DS3231 (часы реального времени)
#include <EEPROM.h>                               //   Подключаем библиотеку для работы с энергонезависимая память EEPROM
#include <GyverOLED.h>                            //   Подключаем библиотеку для работы с OLED дисплеем
#include <iarduino_I2C_Encoder.h>                 //   Подключаем библиотеку для работы с энкодером I2C-flash
#include "GyverButton.h"

// ================== РАЗНОЕ ===================
#define PIN_TONE 13                               //   Объявляем пин для работы с пьезо бузером
#define PIN_SENSOR A7                             //   Объявляем пин для работы с выносным погружным датчиком
#define SYSTEM_DATA_ADDR 0
#define MAIN_MENU_ITEM 6
#define CONTRAST_POINT 15
#define IN_PERCENT 1
#define MIN_VALUE 0
#define MAX_VALUE 100
#define TIME_SENSOR 30

#define CONST_SENSOR 2      //  коректировка сенсора

#define BTN_PIN_L 3 // кнопка
#define BTN_PIN_R 4 // кнопка


// ================== ОБЪЕКТЫ ==================
iarduino_I2C_Encoder enc(0x09);                   //   Объявляем объект enc для работы с функциями и методами библиотеки iarduino_I2C_Encoder, указывая адрес модуля на шине I2C.
GyverOLED<SSH1106_128x64> oled;                   //   Объявляем объект oled для работы с OLED дисплеем
MicroDS3231 rtc;                                  //   Объявляем объект rtc для работы с RTC DS3231 (часы реального времени)

const static uint8_t icons_8x8[][8] PROGMEM = {
  {0xff, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0xff}, //false sqr
  {0xff, 0x81, 0xbd, 0xbd, 0xbd, 0xbd, 0x81, 0xff}, //true sqr
  {0x78, 0x48, 0xcf, 0x1, 0xff, 0x2, 0x2, 0x2},//1l
  {0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2}, //line
  {0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40},
  {0x1e, 0x12, 0xf3, 0x80, 0xff, 0x40, 0x40, 0x40},
  {0x2, 0x2, 0xff, 0x1, 0x7f, 0x40, 0x40, 0xc0},
  {0x40, 0x40, 0xff, 0x80, 0xfe, 0x2, 0x2, 0x3}
};

boolean serviceFlag;
bool mainMenu = true;
bool sensorMN = false;
bool infoMN = false;

bool tt = false;
bool btn = false;  

struct {
  bool percent;
  uint8_t contrast;
  int16_t timeP;
  bool soundP;
  int16_t minP;
  int16_t maxP;
} systemData;

float volt;
float ntu;

GButton btn_L(BTN_PIN_L);
GButton btn_R(BTN_PIN_R);

void setup(){     
    Serial.begin(9600);  
    EEPROM.get(SYSTEM_DATA_ADDR, systemData);                                
                       
    OledInit();        // Инициализация дисплея
    TimeInit();        // Инициализация Часов RTC
    BtnInit();         // Инициализация кнопок
    
    oledSplash();
   
    enc.begin();  

   
}                                                 
                                          
void loop(){  
    btn_L.tick();
    btn_R.tick();

    if (btn_L.isHolded()) {
        Serial.println("Click menu");  
        serviceFlag = !serviceFlag; 
    }

    if(serviceFlag){
        menuGUI();    
    }else{
        readSensor();
    }   

     
}   


void menuGUI(){
   static int8_t pointer = 1; // Переменная указатель  
    
    if(btn_R.isClick()){
      
        if(mainMenu){
          pointer = constrain(pointer - 1, 0, MAIN_MENU_ITEM);
          if(pointer == 0){
            pointer = MAIN_MENU_ITEM;
            }
        } 
        if(!mainMenu && pointer==1){
            systemData.percent = constrain(systemData.percent - 1, 0, 1);
            EEPROM.put(SYSTEM_DATA_ADDR,systemData);
        }
        if(!mainMenu && pointer==2){
            systemData.timeP = constrain(systemData.timeP - 10, 30, 30000);
            EEPROM.put(SYSTEM_DATA_ADDR,systemData);
        }
        if(!mainMenu && pointer==3){
            systemData.soundP = constrain(systemData.soundP - 1, 0, 1);
            EEPROM.put(SYSTEM_DATA_ADDR,systemData);
        }
        if(!mainMenu && pointer==4){
            systemData.contrast = constrain(systemData.contrast - CONTRAST_POINT, 5, 255);
            oled.setContrast(systemData.contrast);
            EEPROM.put(SYSTEM_DATA_ADDR,systemData);
        }
        if(!mainMenu && pointer==5){
            systemData.minP = constrain(systemData.minP - 1, 0, 2000);
            EEPROM.put(SYSTEM_DATA_ADDR,systemData);
        }
         if(!mainMenu && pointer==6){
            systemData.maxP = constrain(systemData.maxP - 1, 0, 2000);
            EEPROM.put(SYSTEM_DATA_ADDR,systemData);
        }

    }

    if(btn_L.isClick()){
     if(mainMenu){
      pointer = constrain(pointer + 1, 0, MAIN_MENU_ITEM + 1);
      if(pointer == MAIN_MENU_ITEM + 1){
        pointer = 1;
        }
      }

      //  if(turnR){
      //   if(mainMenu){
      //     pointer = constrain(pointer + 1, 0, MAIN_MENU_ITEM + 1);
      //     if(pointer == MAIN_MENU_ITEM + 1){
      //       pointer = 1;
      //       }
    
      //   }

      // if(!mainMenu && pointer==1){
      //   systemData.percent = constrain(systemData.percent + 1, 0, 1);
      //   EEPROM.put(SYSTEM_DATA_ADDR,systemData);
      //   }
      // if(!mainMenu && pointer==2){
      //   systemData.timeP = constrain(systemData.timeP + 10, 30, 30000);
      //   EEPROM.put(SYSTEM_DATA_ADDR,systemData);
      // }
      // if(!mainMenu && pointer==3){
      //   systemData.soundP = constrain(systemData.soundP + 1, 0, 1);
      //   EEPROM.put(SYSTEM_DATA_ADDR,systemData);
      //   }
      // if(!mainMenu && pointer==4){
      //   systemData.contrast = constrain(systemData.contrast + CONTRAST_POINT, 5, 255);
      //   oled.setContrast(systemData.contrast);
      //   EEPROM.put(SYSTEM_DATA_ADDR,systemData);
      // }
      // if(!mainMenu && pointer==5){
      //   systemData.minP = constrain(systemData.minP + 1, 0, 2000);
      //   EEPROM.put(SYSTEM_DATA_ADDR,systemData);
      //   }
      // if(!mainMenu && pointer==6){
      //   systemData.maxP = constrain(systemData.maxP + 1, 0, 2000);
      //   EEPROM.put(SYSTEM_DATA_ADDR,systemData);
      // }
    }

     if( btn_L.isDouble() ){              //   Если кнопка энкодера нажимается, то ...
        Serial.println("Нажали");                 //   Выводим текст.
        btn = !btn;
        mainMenu = !mainMenu;
        Serial.println(btn);     
      }                                             //
                             
      /* меню */
      oled.clear();           // Очищаем буфер
      oled.home();            // Курсор в левый верхний угол
      oled.print(F( 
          "НАСТРОЙКИ\n\r"
          "  \n\r"
          "  В процентах\n\r"
          "  Период опроса\n\r"
          "  Звук\n\r"
          "  Яркость\n\r"
          "  Минимальное \n\r"
          "  Максимальное \n\r"
          )); 
      oled.setCursor(95,2);
      if(systemData.percent == 0 ){
        oled.print("выкл");
      }else{
        oled.print("вкл");
      }
      oled.setCursor(95,3);
      oled.print(systemData.timeP);
      oled.setCursor(95,4);
      if(systemData.soundP == 0 ){
         oled.print("выкл");
      }else{
         oled.print("вкл");
      }
      oled.setCursor(95,5);
      oled.print(systemData.contrast);
      oled.setCursor(95,6);
      oled.print(systemData.minP);
      oled.setCursor(95,7);
      oled.print(systemData.maxP);
    
      printPointer(pointer);  // Вывод указателя
      
      oled.update();          // Выводим кадр на дисплей
}

void drawBattery() {
      oled.setCursor(112,0);   
      oled.drawByte(0b0011100);  // пипка
      oled.drawByte(0b0011100);
      oled.drawByte(0b1111111);  // стенка
      
      for (byte i = 0; i < 6; i++) {
         oled.drawByte(0b1000001);
      }
      for (byte i = 0; i < 4; i++) {
         oled.drawByte(0b1011101);
      }
      oled.drawByte(0b1000001);
      oled.drawByte(0b1111111);  // попка
//      oled.update();
}

void printPointer(uint8_t pointer) {
  // Указатель в начале строки
  if(mainMenu){  
      oled.setCursor(0, pointer+1);
      oled.print(">");
  } else {
      oled.setCursor(124, pointer+1);
      oled.print("<");
  }
}

void drawIcon8x8(byte index) {
  size_t s = sizeof icons_8x8[index];
  for(unsigned int i = 0; i < s; i++) {
    oled.drawByte(pgm_read_byte(&(icons_8x8[index][i])));
  }
}

void Power_tone(){
  tone(PIN_TONE, 1350, 100);
}

void oledSplash(){
    Power_tone();
    oled.clear();
    oled.setCursor(6,3);
    oled.print(F("Turbidity_sensor \n\r\n\r"" v1.2.1"));
    oled.update();
    delay(3000);    
}
