#ifndef Dispaly
#endif

#include <GyverOLED.h>

// Экран (SSD1306 128x32, без буфера)
GyverOLED<SSD1306_128x32, OLED_NO_BUFFER> oled;
// Экран (SSD1306 128x64, без буфера)
// GyverOLED<SSD1306_128x64, OLED_NO_BUFFER> oled;

#include <DisplaySplash.h>
#include <DisplayValues.h>
#include <DisplayGraph.h>