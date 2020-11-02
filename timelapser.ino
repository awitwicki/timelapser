#define CLK 2
#define DT 3
#define SW 4

#include "GyverEncoder.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//Oled display

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

Encoder enc1(CLK, DT, SW);
int timer_seconds = 15;
unsigned long started = 0;
unsigned long next = 0;
long coolDown = timer_seconds;
int taked = 0;

bool photoSignal = false;
unsigned long pinCoolDown = 0;

void menuView()
{
    display.clearDisplay();
    //logo
    display.drawLine(0, 0, display.width() - 1, 0, WHITE);
    display.drawLine(0, display.height() - 1, display.width() - 1, display.height() - 1, WHITE);
    display.drawLine(0, 0, 0, 15, WHITE);
    display.drawLine(display.width() - 1, 0, display.width() - 1, 15, WHITE);
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(4, 4);
    display.println("TimeLapser v 1.1");
    display.drawLine(0, 15, display.width() - 1, 15, WHITE);

    display.setTextSize(2);
    display.setCursor(15, 16);

    display.setTextColor(WHITE);
    display.println("set time:");
    if (timer_seconds < 10)
    {
        display.setCursor(55, 35);
    }
    else
    {
        display.setCursor(47, 35);
    }

    display.print(timer_seconds);
    display.print("s");
    display.setTextSize(1);
    display.setCursor(0, 55);
    display.println("Push encoder to start");

    display.display();
}

void menuViewUpdate()
{
    display.setTextSize(2);
    display.fillRect(47, 35, 35, 15, 0);
    if (timer_seconds < 10)
    {
        display.setCursor(55, 35);
    }
    else
    {
        display.setCursor(47, 35);
    }

    display.print(timer_seconds);
    display.print("s");

    display.display();
}

void startWorking()
{
    started = millis();
    next = started + (timer_seconds * (unsigned long)1000);
    coolDown = (next - millis()) / 1000;
}

void pinTick()
{
    if (photoSignal && millis() > pinCoolDown)
    {
        digitalWrite(13, LOW);
        photoSignal = false;
    }
}

void vorkingTick()
{
    if (millis() > next)
    {
        //high pin
        next = next + (timer_seconds * (unsigned long)1000);
        taked++;
        photoSignal = true;
        pinCoolDown = millis() + 100;
        digitalWrite(13, HIGH); // open transistor, make photo
    }

    //every second update coolDown
    {
        coolDown = (next - millis()) / 1000;
    }
}

void workingView()
{
    display.clearDisplay();

    //logo
    display.drawLine(0, 0, display.width() - 1, 0, WHITE);
    display.drawLine(0, display.height() - 1, display.width() - 1, display.height() - 1, WHITE);
    display.drawLine(0, 0, 0, 15, WHITE);
    display.drawLine(display.width() - 1, 0, display.width() - 1, 15, WHITE);
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(4, 4);
    display.print("Taked ");
    display.print(taked);
    display.print(" photos");
    display.drawLine(0, 15, display.width() - 1, 15, WHITE);

    display.setCursor(4, 16);

    display.setTextColor(WHITE);
    display.println("Next photo in:");
    display.setTextSize(2);
    display.setCursor(4, 32);
    display.print(coolDown);
    display.print("s");
    display.setTextSize(1);
    display.setCursor(4, 55);
    display.println("Push encoder to stop");

    display.display();
}

void displayBlink()
{
    display.clearDisplay();
    display.fillRect(0, 0, display.width() - 1, display.height() - 1, WHITE);
    display.display();
    display.clearDisplay();
}

void setup()
{
    pinMode(13, OUTPUT);
    enc1.setType(TYPE1);
    enc1.setFastTimeout(50);
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // initialize with the I2C addr 0x3C (for the 128x64 oled from aliexpress!)
    display.clearDisplay();

    menuView();
}

void loop()
{
    pinTick();
    enc1.tick();

    if (started == 0) //menu
    {
        if (enc1.isRight())
            timer_seconds++;
        if (enc1.isLeft() && timer_seconds > 1)
            timer_seconds--;

        if (enc1.isFastR())
            timer_seconds += 3;
        if (enc1.isFastL() && timer_seconds > 5)
            timer_seconds -= 3;

        if (enc1.isTurn()) //when encoder rotated
        {
            menuViewUpdate();
        }

        if (enc1.isPress())
        {
            //instant photo
            digitalWrite(13, HIGH);
            displayBlink();
            digitalWrite(13, LOW);

            taked = 0;
            startWorking();
            workingView();
        }
    }
    else //working
    {
        vorkingTick();
        workingView();
        if (enc1.isPress())
        {
            started = 0;
            menuView();
        }
    }
}
