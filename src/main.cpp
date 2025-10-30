#include <Arduino.h>
#include <Display.h>


// Назначение пинов
const uint8_t FREQ_PIN_2    = 2;   // Вход частотомера (прерывания по CHANGE)
const uint8_t BUTTON_PIN_4  = 4;   // Кнопка "-" Уменьшает количетво периодов на осцилограмме
const uint8_t BUTTON_PIN_5  = 5;   // Кнопка "+" Увеличивает количетво периодов на осцилограмме
const uint8_t BUTTON_PIN_6  = 6;   // Кнопка "DisplayMode" Меняет режим отображения данных 
const uint8_t BUTTON_PIN_7  = 7;   // Кнопка "Reset" Перезапуск счета и оттрисовки осцилограммы
const uint8_t BUTTON_PIN_8  = 8;   // Кнопка Up   (зарезервирована для будущего использования)
const uint8_t BUTTON_PIN_9  = 9;   // Кнопка Down (зарезервирована для будущего использования)
const uint16_t BTN_DEB     = 25;   // Антидребезг кнопки (мс)

const uint8_t BUTTON_PINS[] = {
  BUTTON_PIN_4, BUTTON_PIN_5, BUTTON_PIN_6,
  BUTTON_PIN_7 //, BUTTON_PIN_8, BUTTON_PIN_9
};
const uint8_t BUTTON_COUNT = sizeof(BUTTON_PINS) / sizeof(BUTTON_PINS[0]);

volatile uint8_t *buttonPorts[BUTTON_COUNT]; // указатели на регистры портов
uint8_t buttonMasks[BUTTON_COUNT];           // маски битов для каждого пина
bool buttonPrevState[BUTTON_COUNT];          // предыдущее состояние (true = нажат)
uint32_t buttonLastDebounce[BUTTON_COUNT];   // время последнего события для debounce


// Таймер подсчёта
const int count_timer_ms  = 300;             // Интервал накопления подсчёта (мс)

// Переменные замеров частоты и счетчиков состояний
unsigned long pulse_count       = 0;         // Счётчик фронтов (инкрементируется в ISR)
unsigned long count_hi_states   = 0;         // Суммарное время/счёт высокого состояния (в "единицах" ISR)
unsigned long count_low_states  = 0;         // Суммарное время/счёт низкого состояния (в "единицах" ISR)
unsigned long frequency_hz      = 0;         // Частота
unsigned long p_frequency_hz    = 0;         // Пердыдущий замер частоты
unsigned long last_time         = 0;         // Время последнего обновления (мс)

int analog_value_A0             = 0;         // Напряжение на A0 в попугаях Ардуины
float voltage                = 0.0f;         // Пересчитанное напряжение на A0

// Переменная дисплея
bool is_dispalyed = false;
int min_periods_on_screen       = 1;         // Минимальное количество отображаемых периодов
int periods_on_screen           = 3;         // Начально количество отображаемых периодов (по умолчанию)
int max_periods_on_screen       = 8;         // Масимальное количесво отображаемых периодов
int display_mode                = 0;         // Режим дисплея (TODO)


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

void resetCounters(){
    is_dispalyed = false;
    pulse_count=0;
    count_hi_states=0;
    count_low_states=0;
    last_time = millis();
}

void setup() {

    // инициализация всех кнопок как INPUT_PULLUP
    for (uint8_t i = 0; i < BUTTON_COUNT; ++i) {
      pinMode(BUTTON_PINS[i], INPUT_PULLUP);
    }

    // Предвычисляем порты и маски, читаем начальные состояния
    for (uint8_t i = 0; i < BUTTON_COUNT; ++i) {
      buttonPorts[i] = portInputRegister(digitalPinToPort(BUTTON_PINS[i]));
      buttonMasks[i] = digitalPinToBitMask(BUTTON_PINS[i]);
      // INPUT_PULLUP: нажатие = 0 на линии -> инвертируем чтение для логичности (true = нажата)
      buttonPrevState[i] = !((*buttonPorts[i]) & buttonMasks[i]);
      buttonLastDebounce[i] = 0;
    }

    // Инициализируем и очищаем экран
    oled.init();              // 3 ms
    oled.clear();             // 63 ms        
    
    // splashScreen();
    oled.clear();

    attachInterrupt(digitalPinToInterrupt(FREQ_PIN_2), countPulse, CHANGE);
    interrupts();
}

void loop() {
  uint32_t now = millis();
  // analogValue = analogRead(A0);
  // voltage = analogValue * (5.0 / 1023.0);
  // debug0(voltage, display_mode,periods_on_screen);


  // Частотомер
  if (millis() - last_time <= count_timer_ms) {
    // Если таймер частомера не истек - по изменению фронта на пине D2 запускается функция countPulse()
  } else {
    detachInterrupt(digitalPinToInterrupt(FREQ_PIN_2));
    // Если таймер истек - вычисляем частоту по накопленным импульсам в функции countPulse()
    // Т.К мы дергаем функцию countPulse() по изменению фронта, то частота будет в 2 раза ниже чем pulse_count
    frequency_hz = (float)pulse_count * 1000.0 / count_timer_ms / 2;

    // Рисуем данные на дисплее
    if (!is_dispalyed ){
      p_frequency_hz = frequency_hz;
      is_dispalyed = true;
      printValues(frequency_hz, count_low_states, count_hi_states);
      drawGraph(frequency_hz, count_low_states, count_hi_states, periods_on_screen );
    }
  }


    // ------------------- Оптимизированное чтение кнопок -------------------
    // читаем каждый пин один раз через предвычисленные порты/маски
    for (uint8_t i = 0; i < BUTTON_COUNT; ++i) {
      bool state = !((*buttonPorts[i]) & buttonMasks[i]); // true = нажата

      if (state != buttonPrevState[i]) {
        // изменение состояния — проверяем дебаунс
        if ((now - buttonLastDebounce[i]) >= BTN_DEB) {
          buttonLastDebounce[i] = now;
          buttonPrevState[i] = state;
          // реагируем только на фронт нажатия (state == true)
          if (state) {
            // i == 0 => BUTTON_PIN_4 ("Режим отображенния") 
            // i == 1 => BUTTON_PIN_5 ("- Периодов")
            // i == 2 => BUTTON_PIN_6 ("+ Периодов")
            // i == 3 => BUTTON_PIN_7 ("Рестарт счета")
            if (i == 0) {
              display_mode++;
              if (display_mode > 3)display_mode = 0;
            } else if (i == 1 && periods_on_screen > min_periods_on_screen) {
              periods_on_screen--;
            } else if (i == 2 && periods_on_screen < max_periods_on_screen) {
              periods_on_screen++;
            } else if (i == 3) {
              // просто просходит рестарт счета
              // noInterrupts();
              resetCounters();
              // interrupts();
              attachInterrupt(digitalPinToInterrupt(FREQ_PIN_2), countPulse, CHANGE);

            }
            // Зарезервированные кнопки (i >= 4) — пока без действия
          }
        }

      }
    }


  // }
}

