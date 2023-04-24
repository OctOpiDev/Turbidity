void oledInit() {
    oled.init();  
    oled.clear();
    oled.setContrast(systemData.contrast);
}


void timeInit(){
    rtc.setTime(BUILD_SEC, BUILD_MIN, BUILD_HOUR, BUILD_DAY, BUILD_MONTH, BUILD_YEAR);
    if (!rtc.begin()) {
        Serial.println("DS3231 not found");
        for(;;);
    }
}
