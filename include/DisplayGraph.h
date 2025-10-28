#ifndef DispalyGraf
#endif

// Позиции печати на экрне для drawGarph
int diagramm_len = 64;                         // Длинна диаграммы
int diagramm_start_x_pos    = 64;              // Начальнная позиция рисования диаграммы
bool current_front_flag     = true;            // 


// Рисование осциллограммы — поддерживает дробное количество периодов (<1) ------------------------------------
void drawGarph(long frequency_hz, long count_low_states, long count_hi_states, int periods_on_screen) {

  // Калькулятор пресчета процетов скажности
  long total_states = count_low_states + count_hi_states;     // Сколько всего натикало клоков
  int hi_states_percent  = 0;
  int low_states_percent = 0;

  if (total_states > 0) {
      // hi_states_percent =  round ((float)(count_hi_states) / (float)(total_states) * 100);  // % 1
      low_states_percent = round ((float)(count_low_states) / (float)(total_states) * 100); // % 0
  }
  
  if (total_states == 0){ 
     bool state_pin = digitalRead(2);
     if (state_pin){
      hi_states_percent = 100;
     } else {
      low_states_percent = 100; 
     }
  }

  int period_len_px = diagramm_len / periods_on_screen ;
  // int hi_len_px  = round ((float)period_len_px / 100 * hi_states_percent) ;
  int low_len_px = round ((float)period_len_px / 100 * low_states_percent) ;

 
  // Рисуем каждый столбец (по X) — поддерживает частичные периоды и инверсию фронта
  for (int x = 0; x < diagramm_len; ++x) {
    float phase = fmodf((float)x, period_len_px);     // позиция внутри периода
    bool isLow = (phase < low_len_px);
    if (!current_front_flag) isLow = !isLow;

    int draw_x = diagramm_start_x_pos + x;
    // Верхняя и нижняя байты для отображения логики
    uint8_t top_byte = isLow ? 0b00000001 : 0b11100001;
    uint8_t bot_byte = isLow ? 0b10000111 : 0b10000000;

    oled.setCursorXY(draw_x, 0);
    oled.drawByte(top_byte);
    oled.setCursorXY(draw_x, 24);
    oled.drawByte(bot_byte);
  } 

  // Cетка
  oled.setCursorXY(diagramm_start_x_pos, 16);
  for (uint8_t i = 0; i < 16; ++i) {
    oled.drawByte(0b00000000);oled.drawByte(0b00000001);
    oled.drawByte(0b00000001);oled.drawByte(0b00000000);
  }

}
