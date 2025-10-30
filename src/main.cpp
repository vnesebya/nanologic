#include <Arduino.h>
#include <Button.h>
#include <Display.h>


// Назначение пина частотомера
const uint8_t FREQ_PIN_2        = 2;         // Вход частотомера (прерывания по CHANGE)

// Назначение кнопок на ппины
Button btn1(4);
Button btn2(5);
Button btn3(6);
Button btn4(7);

// Таймер подсчёта
const int count_timer_ms      = 300;         // Интервал накопления подсчёта (мс)

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
    

    // bool state_pin = !digitalRead(FREQ_PIN_2);

    volatile uint8_t *pinPort = portInputRegister(digitalPinToPort(2));
    uint8_t pinMask = digitalPinToBitMask(2);
    bool state_pin = !((*pinPort) & pinMask);

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
    Serial.begin(9600);

    // Инициализируем и очищаем экран
    oled.init();              // 3 ms
    oled.clear();             // 63 ms        
    
    splashScreen();
    oled.clear();

    attachInterrupt(digitalPinToInterrupt(FREQ_PIN_2), countPulse, CHANGE);
    interrupts();
}

void loop() {

    // uint32_t now = millis();
    
    // Считаем импульсы в прерывании ----------------------------------------------
    resetCounters();
    attachInterrupt(digitalPinToInterrupt(FREQ_PIN_2), countPulse, CHANGE);
    delay (count_timer_ms);
    detachInterrupt(digitalPinToInterrupt(FREQ_PIN_2));

    // Счтаем частоту , т.к натикало в 2 раза больше по обоим фронтам (CHANGE) 
    frequency_hz = (float)pulse_count * 1000.0 / count_timer_ms / 2;

    // Печатаем данные замеров на экране ------------------------------------------
    printValues(frequency_hz, count_low_states, count_hi_states);
    
    // Рисуем график осцилограммы
    drawGraph(frequency_hz, count_low_states, count_hi_states, periods_on_screen );

    // Читаем кнопки
    btn1.tick();
    if (btn1.click()) { 
      Serial.println("click 1");
      display_mode++; 
      if (display_mode > 3) display_mode = 0;
      Serial.println(display_mode);
    }

    btn2.tick();
    if (btn2.click()) { 
      Serial.println("click 2");
      if (periods_on_screen < max_periods_on_screen) periods_on_screen++;
      Serial.println(periods_on_screen);
    }

    btn3.tick();
    if (btn3.click()) {
      Serial.println("click 3");
      if (periods_on_screen > min_periods_on_screen) periods_on_screen--;
      Serial.println(periods_on_screen);
    }


}

