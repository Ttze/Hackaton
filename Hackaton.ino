//Wifi library
#include <SPI.h>
#include <WiFi.h>

//LCD library
#include <Wire.h>
#include "rgb_lcd.h"

WiFiClient client;
char ssid[] = "wireless-uem";     // your network SSID (name) 
int status = WL_IDLE_STATUS;
char FIWARE_SERVER[] = "10.34.81.33";
int FIWARE_PORT = 3000;
//url = "10.34.81.33:3000/sendData"


rgb_lcd lcd;

int colorR = 255;
int colorG = 0;
int colorB = 0;



// Define the pin to which the temperature sensor is connected.
const int pinTemp = A0;
// Define the B-value of the thermistor.
// This value is a property of the thermistor used in the Grove - Temperature Sensor,
// and used to convert from the analog value it measures and a temperature value.
const int B = 3975;

// Defines the pins to which the light sensor and LED are connected.
const int pinLight = A0;

void setup()
{
	while (status != WL_CONNECTED) {
		Serial.print("Attempting to connect to SSID: ");
		Serial.println(ssid);
		// Connect to WPA/WPA2 network. Change this line if using open or WEP network:    
		status = WiFi.begin(ssid);

		// Wait 10 seconds for connection:
		delay(10000);
	}
	
	Serial.println("Connected to wifi");

	// set up the LCD's number of columns and rows:
	lcd.begin(16, 2);
	lcd.setRGB(colorR, colorG, colorB);

	 //Print a message to the LCD.
	lcd.print("Temperatura:");

	Serial.begin(115200);

	delay(1000);
}

void loop()
{
	// Read the value of the light sensor. The light sensor is an analog sensor.
	int light = analogRead(pinLight);

	// Get the (raw) value of the temperature sensor.
	int val = analogRead(pinTemp);
	// Determine the current resistance of the thermistor based on the sensor value.
	float resistance = (float)(1023 - val) * 10000 / val;
	// Calculate the temperature based on the resistance value.
	float temperature = 1 / (log(resistance / 10000) / B + 1 / 298.15) - 273.15;

	// set the cursor to column 0, line 1
	// (note: line 1 is the second row, since counting begins with 0):
	lcd.setCursor(0, 1);
	// print the number of seconds since reset:
	lcd.print(temperature);

	postMeasures(light,temperature);
	
	//Serial.println(temperature);

	delay(10000);
}

void postMeasures(long light, long temp)
{
	Serial.println("\nStarting connection to server...");

	String body = "{\"light\":";
	body += light;
	body += ",\"temperature\":";
	body += temp;
	body += ",\"deviceName\":\"Ciudad Real\",\"lat\":";
	body += "38.988641";
	body += ",\"lng\":";
	body += "-3.933538";
	body += ",\"timeInstant\":\"21/04/2016 16:00:45\"}";

	while (!client.connected())
	{
		if (client.connect(FIWARE_SERVER, FIWARE_PORT))
			Serial.println("connected to server");

		else
			Serial.println("ERROR");

		delay(1000);
	}

	// Make a HTTP request:
	client.println("POST /SendData HTTP/1.1");
	client.println("Host:" + String(FIWARE_SERVER) + ":" + String(FIWARE_PORT));
	client.println("Content-Type: application/json");
	client.println("Content-Length: " + String(body.length()));
	client.println("Connection: close");
	client.println();
	client.println(body);
	Serial.println(body);


	client.stop();
	
	Serial.println("Done");
}