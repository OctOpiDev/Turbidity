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

        oled.setScale(2);
        oled.setCursor(0,3); 
        oled.print(ntu);
//        oled.print(" NTU");

        oled.setScale(1);
        oled.setCursorXY(0, 56);
//      oled.print(sensorValue);
        oled.print(volt);
        oled.print(" V");
     
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

void sens(){
    volt = 0;
    
    for(int i=0; i<800; i++)
    {
        volt += ((float)analogRead(PIN_SENSOR)/1023)*4*2.5;
    }
    volt = volt/800;
    volt = round_to_dp(volt,1);
    if(volt < 2.5){
      ntu = 3000;
    }else{
      ntu = -1120.4*square(volt)+5742.3*volt-4353.8; 
    }
    if(volt == 4.1)  ntu=0;
    if(volt > 4.2)   ntu=0;
}

float round_to_dp( float in_value, int decimal_place ){
  float multiplier = powf( 10.0f, decimal_place );
  in_value = roundf( in_value * multiplier ) / multiplier;
  return in_value;
}
