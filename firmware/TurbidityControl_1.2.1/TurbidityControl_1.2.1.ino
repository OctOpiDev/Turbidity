#define VERSION "1.2.1"

#include <microDS3231.h>
#include <EEPROM.h>
#include <Wire.h>                                 //   Подключаем библиотеку для работы с аппаратной шиной I2C.
#include <GyverOLED.h>
#include <iarduino_I2C_Encoder.h>                 //   Подключаем библиотеку для работы с энкодером I2C-flash.

#define PIN_TONE 13            
#define sensorPin A7
#define SYSTEM_DATA_ADDR 0
#define MAIN_MENU_ITEM 6
#define CONTRAST_POINT 15
#define IN_PERCENT 1
#define MIN_VALUE 0
#define MAX_VALUE 100
#define TIME_SENSOR 30

iarduino_I2C_Encoder enc(0x09);                   //   Объявляем объект enc для работы с функциями и методами библиотеки iarduino_I2C_Encoder, указывая адрес модуля на шине I2C.
GyverOLED<SSH1106_128x64> oled;    

MicroDS3231 rtc;
 

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

void setup(){     
    Serial.begin(9600);  
    EEPROM.get(SYSTEM_DATA_ADDR, systemData);                                
                             
    oledInit();
    oledSplash();
   
    enc.begin();  
    rtc.setTime(BUILD_SEC, BUILD_MIN, BUILD_HOUR, BUILD_DAY, BUILD_MONTH, BUILD_YEAR);
    if (!rtc.begin()) {
        Serial.println("DS3231 not found");
        for(;;);
    }
}                                                 
                                          
void loop(){  
//    menuGUI();
timeScreen();


    if(serviceFlag){
        menuGUI();    
    }else{
       readSensor();
      }   

      if( enc.getButton(KEY_TIME_PRESSED) == 2000 ){ //   Если время удержания кнопки возвращаемое функцией getButton больше 5000 миллисекунд, то...
        serviceFlag = !serviceFlag;        //   Выводим текст.
    } 
}   


void menuGUI(){
   static int8_t pointer = 1; // Переменная указатель  

   
    int turnL=0, turnR=0;      
    
    turnL=enc.getEncoder(ENC_TURN_LEFT);          //   Считываем количество тактов поворота энкодера влево  (против часовой стрелки).
    turnR=enc.getEncoder(ENC_TURN_RIGHT);         //   Считываем количество тактов поворота энкодера вправо (по     часовой стрелке).
    
    if(turnL){
      
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
            systemData.timeP = constrain(systemData.timeP - 5000, 5000, 30000);
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
    if(turnR){
     if(mainMenu){
      pointer = constrain(pointer + 1, 0, MAIN_MENU_ITEM + 1);
      if(pointer == MAIN_MENU_ITEM + 1){
        pointer = 1;
        }
    
      }

      if(!mainMenu && pointer==1){
        systemData.percent = constrain(systemData.percent + 1, 0, 1);
        EEPROM.put(SYSTEM_DATA_ADDR,systemData);
        }
      if(!mainMenu && pointer==2){
        systemData.timeP = constrain(systemData.timeP + 5000, 5000, 30000);
        EEPROM.put(SYSTEM_DATA_ADDR,systemData);
      }
      if(!mainMenu && pointer==3){
        systemData.soundP = constrain(systemData.soundP + 1, 0, 1);
        EEPROM.put(SYSTEM_DATA_ADDR,systemData);
        }
      if(!mainMenu && pointer==4){
        systemData.contrast = constrain(systemData.contrast + CONTRAST_POINT, 5, 255);
        oled.setContrast(systemData.contrast);
        EEPROM.put(SYSTEM_DATA_ADDR,systemData);
      }
      if(!mainMenu && pointer==5){
        systemData.minP = constrain(systemData.minP + 1, 0, 2000);
        EEPROM.put(SYSTEM_DATA_ADDR,systemData);
        }
      if(!mainMenu && pointer==6){
        systemData.maxP = constrain(systemData.maxP + 1, 0, 2000);
        EEPROM.put(SYSTEM_DATA_ADDR,systemData);
      }
    }

     if( enc.getButton(KEY_PUSHED) ){              //   Если кнопка энкодера нажимается, то ...
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

void oledInit() {
    oled.init();  
    oled.clear();
    oled.setContrast(systemData.contrast);
}

void readSensor(){
   static uint32_t tmr;
   
   int sensorValue = analogRead(sensorPin);
   int turbidity = map(sensorValue, systemData.minP, systemData.maxP, MAX_VALUE, MIN_VALUE);
   
  if (millis() - tmr >= systemData.timeP) {
    tmr = millis();
    oled.clear();
    oled.home();
    oled.setScale(1);
   timeScreen();

    if(systemData.soundP == 1  && sensorValue > 0){
      Power_tone();
      }

     oled.setScale(4);
     oled.setCursorXY(38, 25); 
     if(sensorValue==0){
        oled.setScale(1);
        oled.print("Нет связи ");
        oled.setCursor(48,5);
        drawIcon8x8(2);
        drawIcon8x8(3);
        drawIcon8x8(3);
        drawIcon8x8(6);
        oled.setCursor(48,6);
        drawIcon8x8(5);
        drawIcon8x8(4);
        drawIcon8x8(4);
        drawIcon8x8(7);
      }else{
        if(systemData.percent == 0){
      oled.print(sensorValue);
     oled.setScale(1);
        }else{
          oled.print(turbidity);
     oled.setScale(1);
     oled.print("%");
          }
        }
    
     
    drawBattery();
    oled.update();
  }
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
    oled.print(F("Turbidity_sensor \n\r\n\r"" v1.0.2"));
    oled.update();
    delay(2000);
    
  }

void timeScreen(){
    DateTime now = rtc.getTime();

    if(now.hour<10) oled.print("0");
        oled.print(now.hour);
        oled.print(":");
    if(now.minute<10) oled.print("0");
        oled.print(now.minute);
}
