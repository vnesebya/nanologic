#ifndef DispalySplash
#endif


// Заствка ----------------------------------------------------------------------------------------------------
void splashScreen(){
    // Рисуем заставку
    oled.setScale(2);
    oled.setCursorXY(0, 16);
    oled.print("_NanoLOGic_");
    oled.setScale(1);
    oled.setCursorXY(64, 0);
    oled.print("Ver: 0.1.1");
    
    // Полоска загрузки
    oled.setCursorXY(0,8);
    for (int x = 0; x < 128; ++x) {
        oled.drawByte(0b01000000);
        delay (8);
    }
    
}
