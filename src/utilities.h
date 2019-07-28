/**
 * Copyright (C) 2019  Nicolas Jeanmonod, ouilogique.com
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#include <Arduino.h>
#ifdef BOARD_M5STACK_CORE_ESP32
#include <M5Stack.h>
#endif

/**
 *
 */
void setupSerial()
{
    Serial.begin(BAUD_RATE);
    Serial.print("\n\n##########################");
    Serial.print("\nCOMPILATION DATE AND TIME:\n");
    Serial.print(COMPILATION_DATE);
    Serial.print("\n");
    Serial.print(COMPILATION_TIME);
    Serial.print("\n##########################\n\n");
}

#ifdef BOARD_M5STACK_CORE_ESP32

#define M5_SCREEN_BRIGHTNESS 10

/**
 *
 */
void setupM5Stack()
{
    // Serial port is started separately
    // for compatibility with other boards.
    M5.begin(true, false, true, true);

    // Starting message.
    delay(10);
    M5.Lcd.setBrightness(M5_SCREEN_BRIGHTNESS);
    M5.Lcd.setTextSize(3);
    M5.Lcd.setTextColor(YELLOW, BLACK);
    M5.Lcd.setCursor(70, 90);
    M5.Lcd.print("STARTING...");
}

/**
 *
 */
void printInfoM5Stack()
{
    M5.Lcd.setBrightness(M5_SCREEN_BRIGHTNESS);

    M5.Lcd.setTextSize(3);
    M5.Lcd.fillScreen(BLACK);

    M5.Lcd.setTextColor(WHITE, BLACK);
    M5.Lcd.setCursor(20, 10);
    M5.Lcd.print("COMPILATION");

    M5.Lcd.setCursor(20, 40);
    M5.Lcd.print("DATE AND TIME");

    M5.Lcd.setTextColor(WHITE, BLACK);
    M5.Lcd.setCursor(20, 90);
    M5.Lcd.print(COMPILATION_DATE);

    M5.Lcd.setCursor(20, 120);
    M5.Lcd.print(COMPILATION_TIME);

    M5.Lcd.setTextColor(YELLOW, BLACK);
    M5.Lcd.setCursor(20, 170);
    M5.Lcd.print(WiFi.localIP());

    M5.Lcd.setCursor(20, 200);
    M5.Lcd.print(WiFi.softAPIP());
}
#endif
