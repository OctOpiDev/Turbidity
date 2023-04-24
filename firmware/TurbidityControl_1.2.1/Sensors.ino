void readSensor(){
   static uint32_t tmr;
   
   int sensorValue = analogRead(PIN_SENSOR);
   int turbidity = map(sensorValue, systemData.minP, systemData.maxP, MAX_VALUE, MIN_VALUE);

   
   
  if (millis() - tmr >= systemData.timeP) {
    tmr = millis();
    oled.clear();
    oled.home();
    oled.setScale(1);
   timeScreen();
   sens();

    if(systemData.soundP == 1  && sensorValue > 0){
      Power_tone();
      }

     oled.setScale(1);
     oled.setCursorXY(38, 55); 
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
//      oled.print(sensorValue);
    oled.print(volt);
    oled.print(" V");

//oled.setScale(2);
     oled.setCursorXY(0, 25); 
    oled.print(ntu);
    oled.print(" ntu");
     
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
