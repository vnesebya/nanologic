#ifndef DispalyGraf

// Позиции печати на экрне для drawGarph
int diagramm_len = 64;                         // Длинна диаграммы
int diagramm_start_x_pos    = 64;              // Начальнная позиция рисования диаграммы
bool current_front_flag     = true;            // 


// Рисование осциллограммы — поддерживает дробное количество периодов (<1) ------------------------------------
void drawGraph(long frequency_hz, long count_low_states, long count_hi_states, int periods_on_screen) {

  // Калькуляция процентов лог 1 и 0
  long total_states = count_low_states + count_hi_states;     // Сколько всего натикало клоков

  float hi_states_percent  = 0;
  float low_states_percent = 0;
  if (total_states > 0) {
      hi_states_percent  = (float)count_hi_states  / (float)total_states * 100.0f; // % 1
      low_states_percent = (float)count_low_states / (float)total_states * 100.0f; // % 0
  }

  // Защита от 0 тактов - если вход подключен к лог 1 или 0
  if (total_states == 0){ 
     bool state_pin = digitalRead(2);
     if (state_pin){
      hi_states_percent = 100;
     } else {
      low_states_percent = 100; 
     }
  }

  // Калькуляция точек в графике по процентам
  int period_len_px = diagramm_len / periods_on_screen ;
  float hi_len_px, low_len_px;
  if (0 < low_states_percent && low_states_percent < 1.0f){
    low_len_px = ceil((float)period_len_px / 100 * low_states_percent) ;
    hi_len_px = period_len_px - hi_len_px;
  } else if (0.0f < hi_states_percent && hi_states_percent < 1.0f){
    hi_len_px  = ceil((float)period_len_px / 100 * hi_states_percent) ;
    low_len_px = period_len_px - hi_len_px;
  } else {
    hi_len_px  = round((float)period_len_px / 100 * hi_states_percent) ;
    low_len_px = period_len_px - hi_len_px;
  }


  // Рисование осцилограммы
  oled.setCursorXY(diagramm_start_x_pos, 0);
  for ( int r = 0; r < periods_on_screen; r++) {
    for (int x = 0; x < low_len_px; ++x) oled.drawByte(0b00000001);
    for (int x = 0; x < hi_len_px;  ++x) oled.drawByte(0b00011001);
  }
  oled.setCursorXY(diagramm_start_x_pos, 24);
  for ( int r = 0; r < periods_on_screen; r++) {
    for (int x = 0; x < low_len_px; ++x) oled.drawByte(0b10011000);
    for (int x = 0; x < hi_len_px;  ++x) oled.drawByte(0b10000000);
  }

  // Рисование сетки
  oled.setCursorXY(diagramm_start_x_pos, 16);
  for (uint8_t i = 0; i < 16; ++i) {
    oled.drawByte(0b00000000);oled.drawByte(0b00000001);
    oled.drawByte(0b00000001);oled.drawByte(0b00000000);
  }

}
#endif