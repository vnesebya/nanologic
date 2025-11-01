#ifndef DisplayValues_h
#define DisplayValues


// Позиции печати на экрне для printValues
const int vales_x_pos = 12;                         // Позиция X для значений частоты скважности и длительности
const int units_x_pos = 48;                         // Позиция X для единиц измерения
const int freq_y_pos = 0;                           // Позиция Y для частоты
const int duty_y_pos = 8;                           // Позиция Y для скважности
const int len1_y_pos = 16;                          // Позиция Y для длительности лог 1
const int len0_y_pos = 24;                          // Позиция Y для длятельности лог 0


// Печать частоты ---------------------------------------------------------------------------------------------
void printFreq(long frequency_hz, int freq_y_pos ){

    // Калькулятор пересчета единиц частоты
    oled.setCursorXY(0, freq_y_pos);
    oled.print("F: ");
    oled.print("      ");
    oled.setCursorXY(12,freq_y_pos);
    // 0 HZ
    if (frequency_hz < 1.0f) {
      float hz = frequency_hz / 1.00f;
      oled.print ("< 1 ");
      oled.setCursorXY(units_x_pos, freq_y_pos);
      oled.print("Hz");
    // 0.999 - 9.99 HZ
    } else if (frequency_hz < 10.0f) {
      float hz = frequency_hz / 1.00f;
      oled.print (hz,3);
      oled.setCursorXY(units_x_pos, freq_y_pos);
      oled.print("Hz");
    // 10.00 - 99.99 HZ
    } else if (frequency_hz < 100.0f) {
      float hz = frequency_hz / 1.00f;
      oled.print (hz,2);
      oled.setCursorXY(units_x_pos, freq_y_pos);
      oled.print("Hz");
    // 100.0 - 999.9 HZ
    } else if (frequency_hz < 1000.0f) {
      float hz = frequency_hz / 1.00f;
      oled.print (hz,1);
      oled.setCursorXY(units_x_pos, freq_y_pos);
      oled.print("Hz");

    // 1.000 - 9.999 KHz
    } else if (frequency_hz < 10000.0f) {
      float khz = frequency_hz / 1000.00f;
      oled.print (khz,3);
      oled.setCursorXY(units_x_pos, freq_y_pos);
      oled.print("K.");
    // 10.00 - 99.99 KHz
    } else if (frequency_hz < 100000.0f) {
      float khz = frequency_hz / 1000.00f;
      oled.print (khz,2);
      oled.setCursorXY(units_x_pos, freq_y_pos);
      oled.print("K.");
    // 100.1 - 999.9 KHz
    } else if (frequency_hz < 1000000.0f) {
      float khz = frequency_hz / 1000.00f;
      oled.print (khz,1);
      oled.setCursorXY(units_x_pos, freq_y_pos);
      oled.print("K.");

    // 1.000 - 9.999 MHz
    } else if (frequency_hz < 10000000.0f) {
      float mhz = frequency_hz / 1000000.0f;
      oled.print (mhz,3);
      oled.setCursorXY(units_x_pos, freq_y_pos);
      oled.print("M.");
    // 10.00 - 99.99 MHz
    } else if (frequency_hz < 100000000.0f) {
      float mhz = frequency_hz / 1000000.0f;
      oled.print (mhz,2);
      oled.setCursorXY(units_x_pos, freq_y_pos);
      oled.print("M.");
    // 100.1 - 999.9 MHz
    } else if (frequency_hz < 1000000000.0f) {
      float mhz = frequency_hz / 1000000.0f;
      oled.print (mhz,1);
      oled.setCursorXY(units_x_pos, freq_y_pos);
      oled.print("M.");

    // > 1 GHz 
    } else {
      float ghz = frequency_hz / 1000000000.0f;
      oled.print ("> 1");
      oled.setCursorXY(units_x_pos, freq_y_pos);
      oled.print("G.");
    }
}

// Печать Скважноссти -----------------------------------------------------------------------------------------
void printDuty (float hi_states_percent, float low_states_percent, int duty_y_pos){

    // Печатаем скважность
    oled.setCursorXY(0, duty_y_pos);
    oled.print("D: ");
    oled.print("      ");
    oled.setCursorXY(12,duty_y_pos);
    oled.print(hi_states_percent,0);
    oled.print("/");
    oled.print(low_states_percent,0);
    oled.print(" ");
    oled.setCursorXY(units_x_pos, duty_y_pos);
    oled.print("%");
}


// Печать длительности ----------------------------------------------------------------------------------------
void printLen (long frequency_hz, int states_percent, bool len_front, int len_y_pos){

    // Калькулятор длительностей лог состояний
    // Считаем длительностей в пикосекундах
    float long_one_period_ns = 0;
    if (frequency_hz > 0){
        long_one_period_ns =  (1 / (float)(frequency_hz) * 1000000000);
    } else {
        long_one_period_ns = 0;
        long_one_period_ns =  (1 / (float)(frequency_hz) * 1000000000);
    }
    

    float long_halfperiod = 0;
    long_halfperiod =  long_one_period_ns / 100 * states_percent;

    // Иконнка Измерения длительности лог 1
    oled.setCursorXY(0, len_y_pos);
    if (len_front){
    // Иконка 1
      oled.drawByte(0b10000000); oled.drawByte(0b11111110); oled.drawByte(0b00000010); oled.drawByte(0b00000010); 
      oled.drawByte(0b11111110); oled.drawByte(0b10000000); oled.print(": ");
    } else {
    // Иконка 0
      oled.drawByte(0b00000010); oled.drawByte(0b11111110); oled.drawByte(0b10000000); oled.drawByte(0b10000000); 
      oled.drawByte(0b11111110); oled.drawByte(0b00000010); oled.print(": ");
    }
    
    oled.print("      ");
    oled.setCursorXY(12, len_y_pos);

    // Значение длительности
    // < 1 nS
    if (long_halfperiod < 1.0f) {
      oled.print("< 1");
      oled.setCursorXY(units_x_pos, len_y_pos);
      oled.print("nS");
    // 1.00 - 999.99 nS
    } else if (long_halfperiod < 1000.0f) {
      float nsec = long_halfperiod / 1.0f;
      oled.print(nsec, 2);
      oled.setCursorXY(units_x_pos, len_y_pos);
      oled.print("nS");
    // 1.0 - 999.99 uS
    } else if (long_halfperiod < 1000000.0f) {
      float usec = long_halfperiod / 1000.0f;
      oled.print(usec, 2);
      oled.setCursorXY(units_x_pos, len_y_pos);
      oled.print("uS");
    // 1.0 - 999.99 mS
    } else if (long_halfperiod < 1000000000.0f) {
      float msec = long_halfperiod / 1000000.0f;
      oled.print(msec, 2);
      oled.setCursorXY(units_x_pos, len_y_pos);
      oled.print("mS");
    // > 1.0 S
    } else {
      oled.print ("> 1");
      oled.setCursorXY(units_x_pos + 1, len_y_pos);
      oled.print("S ");
    }
}


// Печать значений --------------------------------------------------------------------------------------------
void printValues (long frequency_hz, long count_low_states, long count_hi_states){

    oled.setScale(1);
    oled.setCursorXY (0,0);

    printFreq(frequency_hz, freq_y_pos);

    // Калькуляция процентов лог 1 и 0
    long total_states = count_low_states + count_hi_states;     // Сколько всего натикало клоков
    float hi_states_percent  = 0;
    float low_states_percent = 0;
    if (total_states > 0) {
        hi_states_percent  = (float)count_hi_states  / (float)total_states * 100.0f; // % 1
        low_states_percent = (float)count_low_states / (float)total_states * 100.0f; // % 0
    }
    if (0 < low_states_percent && low_states_percent < 1.0f){
      low_states_percent = 1;
      hi_states_percent = 100 - low_states_percent;
    } else if (0.0f < hi_states_percent && hi_states_percent < 1.0f){
      hi_states_percent = 1;
      low_states_percent = 100 - hi_states_percent;
    } else {
      hi_states_percent  = round(hi_states_percent) ;
      low_states_percent = round(low_states_percent);
    }

    printDuty(hi_states_percent, low_states_percent, duty_y_pos);
    printLen(frequency_hz, hi_states_percent,  1, len1_y_pos);
    printLen(frequency_hz, low_states_percent, 0, len0_y_pos);

}
#endif
