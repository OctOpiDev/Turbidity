void OledInit() {
    oled.init();  
    oled.clear();
    oled.setContrast(systemData.contrast);
}


void TimeInit(){
    rtc.setTime(BUILD_SEC, BUILD_MIN, BUILD_HOUR, BUILD_DAY, BUILD_MONTH, BUILD_YEAR);
    if (!rtc.begin()) {
        Serial.println("DS3231 not found");
        for(;;);
    }
}

void BtnInit(){
    btn_L.setDebounce(50);        // настройка антидребезга (по умолчанию 80 мс)
    btn_L.setTimeout(800);        // настройка таймаута на удержание (по умолчанию 500 мс)
    btn_L.setClickTimeout(600);   // настройка таймаута между кликами (по умолчанию 300 мс)
    btn_L.setType(LOW_PULL);
    btn_L.setDirection(NORM_OPEN);

    btn_R.setDebounce(50);        // настройка антидребезга (по умолчанию 80 мс)
    btn_R.setTimeout(800);        // настройка таймаута на удержание (по умолчанию 500 мс)
    btn_R.setClickTimeout(600);   // настройка таймаута между кликами (по умолчанию 300 мс)
    btn_R.setType(LOW_PULL);
    btn_R.setDirection(NORM_OPEN);

}
