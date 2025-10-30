#ifndef Display

#include <GyverOLED.h>

// Экран (SSD1306 128x32, без буфера)
// GyverOLED<SSD1306_128x32, OLED_NO_BUFFER> oled;
// Экран (SSD1306 128x64, без буфера)
GyverOLED<SSD1306_128x64, OLED_NO_BUFFER> oled;



void debug0(float v1, float v2, float v3){
  oled.setCursorXY(0,40);
  oled.print("1:");
  oled.print (v1);
  oled.setCursorXY(0,48);
  oled.print("2:");
  oled.print (v2);
  oled.setCursorXY(0,56);
  oled.print("3:");
  oled.print (v3);
}
void debug64(float v1, float v2, float v3){
  oled.setCursorXY(64,40);
  oled.print("4:");
  oled.print (v1, 3);
  oled.print ("     ");
  oled.setCursorXY(64,48);
  oled.print("5:");
  oled.print (v2, 3);
  oled.print ("     ");
  oled.setCursorXY(64,56);
  oled.print("6:");
  oled.print (v3, 3);
  oled.print ("     ");
}

#include <DisplaySplash.h>
#include <DisplayValues.h>
#include <DisplayGraph.h>

#endif