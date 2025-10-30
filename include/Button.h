#pragma once
#include <Arduino.h>

#define BTN_DEB 50      // тай-маут смены состояния, мс
#define BTN_HOLD 700    // тай-маут удержания, мс


class Button {
   public:
    Button(uint8_t pin) : _pin(pin) {
        pinMode(pin, INPUT_PULLUP);
    }

    void tick() {
        bool state = !digitalRead(_pin);
        _clickF = 0;
        _holdF = 0;

        if (_pState != state && millis() - _tmr >= BTN_DEB) {
            _pState = state;
            _hold = false;
            _tmr = millis();
            if (state) _clickF = true;
        }

        if (_pState && !_hold && millis() - _tmr >= BTN_HOLD) {
            _hold = true;
            _holdF = true;
        }
    }

    bool click() {
        return _clickF;
    }

    bool hold() {
        return _holdF;
    }

   private:
    uint32_t _tmr = 0;
    uint8_t _pin;
    bool _pState = 0;
    bool _hold = 0;
    bool _clickF = 0;
    bool _holdF = 0;
};
