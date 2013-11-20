/*
Network enabled thermostat code.

for now:

you will get temp
you will get humidity
you will get 'is the heat on'

Use the Ethernet shield (Luke)

11/10/2013 SDC 

considerations
DHCP
Security
etc

11/13/2013 SDC
simplest interface imaginable

11/14/2013 SDC
It's live up in the Bloominglabs.
It's on github

*/
#include <SPI.h>
#include <Ethernet.h>
#include "DHT.h"

byte mac[] = { 
  0x90, 0xA2, 0xDA, 0x00, 0x98, 0x28 };
byte ip[] = { 192, 168, 1, 3 }; // ip in lan
byte gateway[] = { 192, 168, 1, 1 }; // internet access via router
byte subnet[] = { 255, 255, 255, 0 }; //subnet mask
EthernetServer server(80); //server port
String readString;
boolean heat = false;
float temp, humidity;
int heatPin = 4;
unsigned long lastTempRead = 0;
unsigned int tempReadInterval = 1000; // every second is good enough. C'mon man.
// to-do - show IP addr on LCD
#define DHTPIN 2     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
// Connect pin 1 (on the left) of the sensor to +5V
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600); 
  pinMode(heatPin, OUTPUT);
  turnHeatOff();
  dht.begin();
  Ethernet.begin(mac); //, ip, gateway, subnet);
  server.begin();
  readTempAndHumidity();  
  Serial.print("IP be:");
  for (byte thisByte = 0; thisByte < 4; thisByte++) {
    // print the value of each byte of the IP address:
    Serial.print(Ethernet.localIP()[thisByte], DEC);
    Serial.print("."); 
  }
  Serial.println();
}

void turnHeatOn() {
   digitalWrite(heatPin, HIGH);
   heat = true; 
}

void turnHeatOff() {
   digitalWrite(heatPin, LOW);
   heat = false; 
}

void readTempAndHumidity() {
// only read if somebody asks? or read every second <-this one.
  humidity = dht.readHumidity();
  temp = dht.readTemperature();
  int retries = 0;
  // check if returns are valid, if they are NaN (not a number) then something went wrong!
  while (retries++ < 3 & (isnan(temp) || isnan(humidity))) {
    delay(50);
    humidity = dht.readHumidity();
    temp = dht.readTemperature();
  }

  if (isnan(temp) || isnan(humidity)) {
    Serial.println("Failed to read from DHT");
    temp = -100;
    humidity = -100; // obvious bad results!
  } else {
    temp = temp * 9.0/5.0 + 32; // note, readings seem 3-4 degrees off
    Serial.print("Humidity: "); 
    Serial.print(humidity);
    Serial.print(" %\t");
    Serial.print("Temperature: "); 
    Serial.print(temp);
    Serial.println(" *C");
  }
}

void loop() {
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  if (millis() - lastTempRead > tempReadInterval) {
    readTempAndHumidity();
    lastTempRead = millis();
  } 

  // Create a client connection
  EthernetClient client = server.available();
  if (client) {
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
 
        //read char by char HTTP request
        if (readString.length() < 100) {
 
          //store characters to string
          readString += c;
          //Serial.print(c);
        }
 
        //if HTTP request has ended
        if (c == '\n') {
           ///////////////////// control arduino pin
          if(readString.indexOf("?heaton") >0)//checks for on
          {
            turnHeatOn();    // set pin 4 high
            Serial.println("Heat On");
          }
          else{
            if(readString.indexOf("?heatoff") >0)//checks for off
            {
              turnHeatOff();
              Serial.println("Heat Off");
            }
          }
          ///////////////
          Serial.println(readString); //print to serial monitor for debuging
 
          client.println("HTTP/1.1 200 OK"); //send new page
          client.println("Content-Type: text/html");
          client.println();
 
          client.println("<HTML>");
          client.println("<HEAD>");
          client.println("<meta name='apple-mobile-web-app-capable' content='yes' />");
          client.println("<meta name='apple-mobile-web-app-status-bar-style' content='black-translucent' />");
          client.println("<link rel='stylesheet' type='text/css' href='http://homeautocss.net84.net/a.css' />");
          client.println("<TITLE>Home Automation</TITLE>");
          client.println("</HEAD>");
          client.println("<BODY>");
          client.println("<br />");
       
          client.print("<h1>Humidity: "); 
          client.print(humidity);
          client.print(" %\t");
          client.print("Temperature: "); 
          client.print(temp);
          client.println(" F");
          client.println("</h1>");
          client.println("<p>The Heat is:");
          if(heat == true) {
            client.print("ON");
          } else {
            client.print("OFF");
          }          
          client.println("</p><a href=\"/?heaton\"\">Turn On Heat</a>");
          client.println("<a href=\"/?heatoff\"\">Turn Off Heat</a><br />"); 
          client.println("</BODY>");
          client.println("</HTML>");
          delay(1);
          client.stop();
          //clearing string for next read
          readString=""; 
        }
      }
    }
  }
}
