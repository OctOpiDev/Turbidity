void timeScreen(){
    DateTime now = rtc.getTime();

    oled.home();
    if(now.hour<10) oled.print("0");
        oled.print(now.hour);
        oled.print(":");
    if(now.minute<10) oled.print("0");
      oled.print(now.minute);
}
