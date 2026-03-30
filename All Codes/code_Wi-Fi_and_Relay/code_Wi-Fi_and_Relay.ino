/******************************************************************************************/
// This is a simple room thermometer using LED Strips                                     //
// Author  : Ash Adhikari                                                                 //
// Email   : tarantula3@gmail.com                                                         //
// YouTube : https://www.youtube.com/@CrazyCoupleDIY                                      //
// Blog    : http://diy-projects4u.blogspot.com/                                          //
/******************************************************************************************/

#include <OneWire.h>
#include <Adafruit_NeoPixel.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 3  // DS18B20 data pin
#define PIN 4           // WS2812B data pin
#define RELAY_1 5       // The Relay Pin 
#define NUM_LEDS 24     // Number of LEDs

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
Adafruit_NeoPixel strip(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);

// Temperature Range
const float MIN_TEMP = -10.0;   // Minimum temperature (°C)
const float MAX_TEMP =  50.0;   // Maximum temperature (°C)
float Celcius        =  0;
int RunOnce          =  0;      // Variable that helps it to 

// Pre-computed colors for maximum speed
const uint32_t WHEEL_24_PALETTE[24] = {
    0x0000FF, 0x0128FF, 0x0264FF, 0x03B7FF, // Blue to Cyans 
    0x04DBFF, 0x04FFAD, 0x00FF55, 0x00FF55, // Cyan to Green
    0x03FF31, 0x00FF00, 0x00FF00, 0x55FF00, // Green 
    0xAAFF00, 0xFFFF00, 0xFDFF07, 0xFFC708, // Yellow to green
    0xFF8106, 0xFF5500, 0xFF5500, 0xFF2D17, // Orange to Yellow
    0xFF2D17, 0xFF0000, 0xFF0000, 0xB30000  // Red to Orange 
};
void displayRainbow( int HowMany ) {
    for(int i=0; i<HowMany; i++)
      strip.setPixelColor(i, WHEEL_24_PALETTE[i % 24]);
    strip.show();
};

// Setting up Wi-Fi parameters
#include <WiFi.h>
#include <HTTPClient.h>
const char* WIFI_SSID    = "XXXXXX";      // Your SSID
const char* WIFI_PWD     = "XXXXXX";      // Your Password
int StatusCounter        = 0;
unsigned long timerDelay = 1800000;       // Timer set to 30 minutes (1800000) or 1 Minute (60000)
unsigned long lastTime   = 0;             // Previous state

String URLUpdateStatus  = "http://XXX.XXX.XXX.XXX/UpdateStatus.php";
String URLUpdateTemp    = "http://XXX.XXX.XXX.XXX/UpdateTemperature.php";


void setup() {
    Serial.begin(9600); // Turn on serial monitor    
  
    // Connect to the Wi-Fi Network
    WiFi.begin(WIFI_SSID, WIFI_PWD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    };
    Serial.println("\nWiFi connected");
    Serial.print("IP address: "); Serial.println(WiFi.localIP());    

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
    displayRainbow(BAR);
    Serial.print("The Bar Number Is: ");
    Serial.println(BAR);

    /** Check if we need to turn on the fan **/
    if(Celcius >= 30){
      digital.write(RELAY_1, HIGH);
    } else {
      digital.write(RELAY_1, LOW);
    };

    /** Run Once **/
    if(RunOnce == 0){ SendIamAlive(); SendTemperature(); RunOnce = 1; };
  
    /** Sending Humidity and temperature every 30 minutes **/
    if((millis() - lastTime) > timerDelay){ Serial.println("Sending Temp and Humidity"); SendTemperature(); lastTime = millis(); };

    /** Sending I am alive message every minute **/
    if((millis() - StatusCounter) > 60000){ Serial.println("Sending Heartbeat"); SendIamAlive(); StatusCounter = millis(); };  

    delay(500); 
};



//// This function is used to send heartbeats to the Raspberry Pi Home Server 
void SendIamAlive(){
  WiFiConnect( URLUpdateStatus, String("?Token=32432dewfd435zft56he5d") + String("&Location=Miner") );
};

//// Function that sends Temperature and Humidity info to the server
void SendTemperature() {
  WiFiConnect( URLUpdateTemp, String("?HUM=0") + String("&TEMP=") + Celcius + String("&Location=Miner") );
};

//// Function that establishes a wireless connection and sends the data to the server
void WiFiConnect( String URL, String Querystring) {
  if(WiFi.status()== WL_CONNECTED) {
      WiFiClient client;
      HTTPClient http;
      String serverPath = URL + Querystring; 
      Serial.println(serverPath);

      http.begin(client, serverPath.c_str());
      int httpResponseCode = http.GET();      // Send HTTP GET request

      if (httpResponseCode>0) {
        Serial.print("HTTP Response code: "); Serial.println(httpResponseCode);
      } else {
        Serial.print("Error code: "); Serial.println(httpResponseCode);
        /** Reboot the device if device is unable to contact the server **/
        ESP.restart();
      };
      http.end();                            // End the connection
  } else {
    Serial.println("WiFi Disconnected");
  };
};