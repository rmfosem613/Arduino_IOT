#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 19
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress insideThermometer;
#include "Dust.h"
Dust dust;

HardwareSerial dustport(1);  //(통신속도, UART모드, RX핀번호 5)

#include <WiFi.h>
#include <HTTPClient.h>
const char* ssid     = "산학WiFi_208_2.4G";
const char* password = "";
//const char* host = "api.thingspeak.com";
String url = "http://api.thingspeak.com/update?api_key=your_own_api_key";
static unsigned long mark;
int dpm25,dpm10;

void got_dust(int _pm25, int _pm10) {
   Serial.printf("pm25,pm10=");
   Serial.println(String(_pm25) +","+ String(_pm10));
   dpm25 = _pm25;
   dpm10 = _pm10;
}

void send(float temp) {
	HTTPClient http;
	
    Serial.print("\nRequesting URL: ");
	String s = url+"&field1="+String(temp) +"&field2="+ String(dpm25) +"&field3="+ String(dpm10);
    Serial.println(s);
	http.begin(s);

	int httpCode = http.GET();
	if(httpCode == HTTP_CODE_OK) {
		String payload = http.getString();
		Serial.println(payload);
	}
	http.end();
	
	
/*
    WiFiClient client;
    const int httpPort = 80;
    if (!client.connect(host, httpPort)) {
        Serial.println("connection failed");
        return;
    }

    Serial.print("\nRequesting URL: ");
    Serial.println(url+String(temp));

    // This will send the request to the server
    client.print(String("GET ") + url+String(temp) + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n\r\n");
    unsigned long timeout = millis();
    while (client.available() == 0) {
        if (millis() - timeout > 5000) {
            Serial.println(">>> Client Timeout !");
            client.stop();
            return;
        }
    }

    // Read all the lines of the reply from server and print them to Serial
    while(client.available()) {
        String line = client.readStringUntil('\r');
        Serial.print(line);
    }
*/	
}

void ticker() {
  sensors.requestTemperatures();
  float tempC = sensors.getTempC(insideThermometer);
  Serial.print(tempC);
  send(tempC);
}

void setup() {
	Serial.begin(115200);
	dustport.begin(9600, SERIAL_8N1, 5, 4);
	sensors.begin();
	Serial.print("Found ");
	Serial.print(sensors.getDeviceCount(), DEC);
	Serial.print("Device 0 Resolution: ");
	Serial.print(sensors.getResolution(insideThermometer), DEC); 
	Serial.println();
	if (!sensors.getAddress(insideThermometer, 0)) 
		Serial.println("Unable to find address for Device 0"); 

	WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    mark = millis() + 2000;
}

void loop() {
    if (millis() > mark) {
        mark = millis() + 20000;
		ticker();
	}
	  while (dustport.available() > 0) {
		dust.do_char(dustport.read(), got_dust);
		yield();
	  }
}
