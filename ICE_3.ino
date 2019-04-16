/*ICE #3.1
 * 
   DHT22 temperature and humidity sensor demo.
    
   brc 2018
*/

//URL for IO: https://io.adafruit.com/aylorob/dashboards/this-is-for-buttons

// Adafruit IO Temperature & Humidity Example
// Tutorial Link: https://learn.adafruit.com/adafruit-io-basics-temperature-and-humidity
//
// Adafruit invests time and resources providing this open source code.
// Please support Adafruit and open source hardware by purchasing
// products from Adafruit!
//
// Written by Todd Treece for Adafruit Industries
// Copyright (c) 2016-2017 Adafruit Industries
// Licensed under the MIT license.
//
// All text above must be included in any redistribution.

/************************** Configuration ***********************************/

// edit the config.h tab and enter your Adafruit IO credentials
// and any additional configuration needed for WiFi, cellular,
// or ethernet clients.
#include "config.h"

/************************ Example Starts Here *******************************/
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Wire.h>
#include <Adafruit_MPL115A2.h>    //Include these libraries
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

Adafruit_MPL115A2 mpl115a2;  //Create an instance

// pin connected to DH22 data line
#define DATA_PIN 12

// create DHT22 instance
DHT_Unified dht(DATA_PIN, DHT22);

// set up the 'temperature', 'humidity', and 'pressure' feeds
AdafruitIO_Feed *temperature = io.feed("temperature");
AdafruitIO_Feed *humidity = io.feed("humidity");
AdafruitIO_Feed *pressure = io.feed("pressure");

void setup() {

  // start the serial connection
  Serial.begin(115200);

  // wait for serial monitor to open
  while(! Serial);

  // initialize dht22
  dht.begin();
  
  // initialize MPL
  mpl115a2.begin();

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
  display.clearDisplay();                     // Clear any existing image
  display.setTextSize(1);                     // Set text size
  display.setTextColor(WHITE);                // Set text color
  display.setCursor(0, 0);                    // Put cursor back on top left
  display.println("Starting up.");            // Test and write up
  display.display();                          // Display the display

  // connect to io.adafruit.com
  Serial.print("Connecting to Adafruit IO");
  io.connect();

  // wait for a connection
  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  // we are connected
  Serial.println();
  Serial.println(io.statusText());

}

void loop() {

  // io.run(); is required for all sketches.
  // it should always be present at the top of your loop
  // function. it keeps the client connected to
  // io.adafruit.com, and processes any incoming data.
  io.run();

  sensors_event_t event;                        //An event is a grabbing of data, so prep a instance
  dht.temperature().getEvent(&event);           //Actually grab the data

  float celsius = event.temperature;            //Take temperature from the event
  float fahrenheit = (celsius * 1.8) + 32;      //Convert to Celcius

  Serial.print("celsius: ");                    //Print results
  Serial.print(celsius);                        
  Serial.println("C");

  Serial.print("fahrenheit: ");
  Serial.print(fahrenheit);
  Serial.println("F");

  float pressureKPA = 0, temperatureC = 0;                                                    // Use the MPL for temp and pressure

  pressureKPA = mpl115a2.getPressure();                                                       // Get and print pressure  
  Serial.print("Pressure (kPa): "); Serial.print(pressureKPA, 4); Serial.println(" kPa");

  temperatureC = mpl115a2.getTemperature();                                                   // Get and print temperature
  Serial.print("Temp (*C): "); Serial.print(temperatureC, 1); Serial.println(" *C");

  display.clearDisplay();                                                                     //Clear test display off

  display.setCursor(0, 0);                                                                    //Reset cursor
  display.println("Temperature in C:.");                                                      //Print that to display
  display.println(temperatureC);                                                              //Print that to display too
  display.display();                                                                          //Display the display
  
  // save fahrenheit (or celsius) to Adafruit IO
  temperature->save(temperatureC);
  pressure->save(pressureKPA);

  dht.humidity().getEvent(&event);                              //Grab humidity from event

  Serial.print("humidity: ");                                   //Print the humidity
  Serial.print(event.relative_humidity);
  Serial.println("%");

  // save humidity to Adafruit IO
  humidity->save(event.relative_humidity);

  // wait 5 seconds (5000 milliseconds == 5 seconds)
  delay(5000);

}
