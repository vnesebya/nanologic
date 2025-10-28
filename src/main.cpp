#include <Arduino.h>
#include <Display.h>


// Назначение пинов
const uint8_t FREQ_PIN_2    = 2;            // Вход частотомера
const uint8_t BUTTON_PIN_4  = 4;            // Кнопка -
const uint8_t BUTTON_PIN_5  = 5;            // Кнопка +
const uint16_t BTN_DEB      = 50;           // Антидребезг кнопки (мс)

// Таймер подсчёта
const int count_timer_ms  = 100;            // Интервал накопления подсчёта (мс)

// Переменные замеров частоты и счеттчиков состояний
unsigned long pulse_count ;                 // Счётчик фронтов (инкрементируется в ISR)
unsigned long count_hi_states ;             // Суммарное время/счёт высокого состояния (в "единицах" ISR)
unsigned long count_low_states ;            // Суммарное время/счёт низкого состояния (в "единицах" ISR)
unsigned long frequency_hz ;                // Частота
unsigned long last_time;                    // Время последнего обновления (мс)

// Переменная дисплея
bool is_dispalyed = false;
int min_periods_on_screen    = 1;           // Минимальное количество отображаемых периодов
int periods_on_screen        = 3;           // Начально количество отображаемых периодов (по умолчанию)
int max_periods_on_screen    = 7;           // Масимальное количесво отображаемых периодов

// Функция вызывается прерыванием вызванного изменением по  любому фронту на пине FREQ_PIN_2 
// Минимальная логика, только счётчики
void countPulse() {
  // Считаем по изменению фронта на FREQ_PIN_2
  pulse_count++;                              // при каждом изменении фронта увеличиваем общий счётчик
                                              // по сути это удвоенное значение частоты
  bool state_pin = !digitalRead(FREQ_PIN_2);

  // Обновляем счётчики состояний
  if (state_pin) {
    count_hi_states++;                        // счет лог 1
  } else {
    count_low_states++;                       // Счет лог 0
  }
}


void setup() {

    // Настройка пинов и прерывания
    pinMode(FREQ_PIN_2, INPUT);
    attachInterrupt(digitalPinToInterrupt(FREQ_PIN_2), countPulse, CHANGE);

    pinMode(BUTTON_PIN_4, INPUT_PULLUP);
    pinMode(BUTTON_PIN_5, INPUT_PULLUP);


    // Инициализируем и очищаем экран
    oled.init();              // 3 ms
    oled.clear();             // 63 ms        
    
    splashScreen();
    oled.clear();

    // Готовимся к началу счета 
    // сбрасываем счетчики перед началом
    is_dispalyed = false;
    pulse_count=0;
    count_hi_states=0;
    count_low_states=0;
    last_time = millis();
}

void loop() {

  // Частотомер
  if (millis() - last_time <= count_timer_ms) {
    // Если таймер частомера не истек - по спадающем фронту на пине D2 запускается функция countPulse()
  } else {
    // Если таймер истек - вычисляем частоту по накопленным импульсам в функции countPulse()
    frequency_hz = (float)pulse_count * 1000.0 / count_timer_ms / 2;

    // Рисуем данные на дисплее
    if (!is_dispalyed){
      printValues(frequency_hz, count_low_states, count_hi_states); // 95 ms
      drawGarph(frequency_hz, count_low_states, count_hi_states, periods_on_screen );
      is_dispalyed = true;
    }


    // Запускаем обрабочик кнопки повторного перезапуска
    static uint32_t tmr;
    // Кнопка -
    static bool ppin4_state = false;
    bool pin4_state = !digitalRead(BUTTON_PIN_4);
    // Состояние кнопки изменилось и вышел таймер антидребезга
    if (ppin4_state != pin4_state && millis() - tmr >= BTN_DEB) {
      tmr = millis();        // сбросить таймер
      // ppin4_state = pin4_state;   // запоминаем состояние кнопки
      if (pin4_state) {
        // Если кнопка нажата уменьшаем количество периодов на экране
        if ( min_periods_on_screen < periods_on_screen ) periods_on_screen = periods_on_screen - 1;
      }
    }
    // Кнопка +
    static bool ppin5_state = false;
    bool pin5_state = !digitalRead(BUTTON_PIN_5);
    // Состояние кнопки изменилось и вышел таймер антидребезга
    if (ppin5_state != pin5_state && millis() - tmr >= BTN_DEB) {
      tmr = millis();        // сбросить таймер
      // ppin5_state = pin5_state;   // запоминаем состояние кнопки
      if (pin5_state) {
        // Если кнопка нажата увеличиваем количество периодов на экране
        if ( max_periods_on_screen > periods_on_screen ) periods_on_screen = periods_on_screen + 1;
      }
    }

    is_dispalyed = false;
    // сбрасываем счетчики
    pulse_count=0;
    count_hi_states=0;
    count_low_states=0;
    // зануляем таймер частотомера
    last_time = millis();
  }
}

