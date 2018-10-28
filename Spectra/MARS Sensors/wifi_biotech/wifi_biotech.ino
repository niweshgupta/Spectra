#include <ESP8266WiFi.h>

//Using built in LED pin for demo
int val;
// Values from provided (eBay) code
float alpha = 0.75;
int period = 50;
float max1 = 0.0;
//---------------------------------------
int period1 = 50;

String apiWritekey = "B7T8NMOB6K0LNTOO"; // replace with your THINGSPEAK WRITEAPI key here
const char* ssid = "JioFi2_D1D9CA"; // your wifi SSID name
const char* password = "sacgi6p39t" ;// wifi pasword
 
const char* server = "api.thingspeak.com";
float resolution=3.3/1023;// 3.3 is the supply volt  & 1023 is max analog read value
WiFiClient client;
 
void setup() {
  Serial.begin(115200);
  WiFi.disconnect();
  delay(10);
  WiFi.begin(ssid, password);
 
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
 
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("NodeMcu connected to wifi...");
  Serial.println(ssid);
  Serial.println();
}
static int HRC()
{

  // Arbitrary initial value for the sensor value (0 - 1023)
  // too large and it takes a few seconds to 'lock on' to pulse
  static float oldValue = 500;

  // Time recording for BPM (beats per minute)
  static unsigned long bpmMills = millis();
  static int bpm = 0;

  // Keep track of when we had the the last pulse - ignore
  // further pulses if too soon (probably false reading)
  static unsigned long timeBetweenBeats = millis();
  int minDelayBetweenBeats = 400;

  // This is generic code provided with the board:
  // Read the sensor value (0 - 1023)
  int rawValue = analogRead((unsigned char) D3);

  // Some maths (USA: math) to determine whether we are detected a peak (pulse)
  float value = alpha * oldValue + (1 - alpha) * rawValue;
  float change = value - oldValue;
  oldValue = value;

  // if we find a new maximum value AND we haven't had a pulse lately
  if ((change >= max1) && (millis() > timeBetweenBeats + minDelayBetweenBeats)) {

    // Reset max every time we find a new peak
    max1 = change;

    // Flash LED and beep the buzzer
//    digitalWrite(ledPin, 1);
    tone(3, 2000, 50);

    // Reset the heart beat time values
    timeBetweenBeats = millis();
    bpm++;
  }
  else {
    // No pulse detected, ensure LED is off (may be off already)
    //digitalWrite(ledPin, 0);
  }
  // Slowly decay max for when sensor is moved around
  // but decay must be slower than time needed to hit
  // next pulse peak. Originally: 0.98
  max1 = max1 * 0.97;

  // Every 15 seconds extrapolate the pulse rate. Improvement would
  // be to average out BPM over last 60 seconds
  if (millis() >= bpmMills + 15000) {
                //myOLED.clrScr();
                //myOLED.print(" Heart Rate ", CENTER, 0);
                //myOLED.print("BPM (approx)  :", LEFT, 20);
    Serial.print("BPM (approx): ");
                bpm=bpm * 4;
    Serial.println(bpm);
    return bpm;
                //myOLED.printNumI(bpm, RIGHT, 20);
                //myOLED.update();
    bpm = 0;
    bpmMills = millis();
  }

  // Must delay here to give the value a chance to decay
  delay(period);
}
 
void loop() {
  float temp = (analogRead(A0) * resolution) * 100;
  float hrc = HRC();
  
  if (client.connect(server,80))
  {  
    String tsData = apiWritekey;
           tsData +="&field1=";
           tsData += String(temp);
           tsData += "\r\n\r\n";
           
           String tsData1 = apiWritekey;
           tsData1 +="&field2=";
           tsData1 += String(hrc);
           tsData1 += "\r\n\r\n";
 
     client.print("POST /update HTTP/1.1\n");
     client.print("Host: api.thingspeak.com\n");
     client.print("Connection: close\n");
     client.print("X-THINGSPEAKAPIKEY: "+apiWritekey+"\n");
     client.print("Content-Type: application/x-www-form-urlencoded\n");
     client.print("Content-Length: ");
     client.print(tsData.length());
     client.print("\n\n");  // the 2 carriage returns indicate closing of Header fields & starting of data
     client.print(tsData);

     client.print(tsData1.length());
     client.print("\n\n");  // the 2 carriage returns indicate closing of Header fields & starting of data
     client.print(tsData1);
     Serial.print("HRC = ");
                Serial.print(tsData1);

                float mv = (temp/1024.0)*5000; 
                float cel = mv/10;
                float farh = (cel*9)/5 + 32;

                //myOLED.print("TEMPRATURE", CENTER, 16);
                //myOLED.print("------------------------", CENTER, 24);
                Serial.print("TEMPRATURE = ");
                Serial.print(cel);
                //myOLED.printNumF(cel, 2, LEFT, 30);
                Serial.print("*C");
                //myOLED.print("(*C) Celsius", RIGHT, 30);
                Serial.println();
                delay(1000);

                Serial.print("TEMPRATURE = ");
                Serial.print(farh);
                //myOLED.printNumF(farh, 2, LEFT, 50);
                Serial.print("*F");
               // myOLED.print("(*F) Fahrenheit", RIGHT, 50);
               // myOLED.update();
                Serial.println();
                delay(1000);

    // Serial.print("Temperature: ");
    // Serial.print(temp);
     Serial.println("uploaded to Thingspeak server....");
  }
  client.stop();
 
  Serial.println("Waiting to upload next reading...");
  Serial.println();
  // thingspeak needs minimum 15 sec delay between updates
  delay(15000);
}
