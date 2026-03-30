#include <OneWire.h>
#include <Adafruit_NeoPixel.h>
#include <DallasTemperature.h>

#define PIN 4           // WS2812B data pin
#define ONE_WIRE_BUS 3  // DS18B20 data pin
#define NUM_LEDS 24     // Number of LEDs

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
Adafruit_NeoPixel strip(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);

// Temperature Range
const float MIN_TEMP = -10.0;   // Minimum temperature (°C)
const float MAX_TEMP =  50.0;   // Maximum temperature (°C)
float Celcius        =  0;

// Pre-computed colors for maximum speed
const uint32_t _24_PALETTE[24] = {
    0x0000FF, 0x0128FF, 0x0264FF, 0x03B7FF, // Blue to Cyans 
    0x04DBFF, 0x04FFAD, 0x00FF55, 0x00FF55, // Cyan to Green
    0x03FF31, 0x00FF00, 0x00FF00, 0x55FF00, // Green 
    0xAAFF00, 0xFFFF00, 0xFDFF07, 0xFFC708, // Green to Yellow
    0xFF8106, 0xFF5500, 0xFF5500, 0xFF2D17, // Yellow to Orange
    0xFF2D17, 0xFF0000, 0xFF0000, 0xB30000  // Orange to Red 
};


void displayRainbowFast( int HowMany ) {
    for(int i=0; i<HowMany; i++)
      strip.setPixelColor(i, _24_PALETTE[i % 24]);
    strip.show();
};


void setup() {
    Serial.begin(9600); //turn on serial monitor
    strip.begin();
    strip.clear();
    strip.show();
    strip.setBrightness(10);
    sensors.begin();
};


void loop() {
    strip.clear();
    sensors.requestTemperatures(); 
    Celcius = sensors.getTempCByIndex(0);
    Serial.print("The Temperature is: ");
    Serial.println(Celcius);
      
    int BAR = map(Celcius, MIN_TEMP, MAX_TEMP, 0, NUM_LEDS);
    displayRainbowFast(BAR);
    Serial.print("The Bar Number Is: ");
    Serial.println(BAR);

    delay(500); 
};