#include"Arduino.h"
#include <Adafruit_Fingerprint.h>
#include <HardwareSerial.h>
#include "WiFi.h"
#include <HTTPClient.h>
const char* ssid = "TP-Link_Guest_EFDC";         // change SSID
const char* password = "Fwciith@2022";    // change password
const int led = 19;
// Google script ID and required credentials

String GOOGLE_SCRIPT_ID = "AKfycbwAV85c1KkQ0eXvcJEekNSppgUFi--MvRZ3xaN8iu6OuKrwwW0wXSUNENzmO4ylbTiO";    // change Gscript ID

HardwareSerial hw(2);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&hw);


void wifi_init()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) 
  {
    delay(5000);
   // ESP.restart();
  }
}

void Buzzer()
{
  digitalWrite(led, HIGH);
  delay(3000);        
  digitalWrite(led, LOW);   
  
}

int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;
  
  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID); 
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  return finger.fingerID; 
}


void setup()
{
	pinMode(led, OUTPUT);
   	Serial.begin(115200);
   	while (!Serial); 
   	delay(100);
   	Serial.println("\n\nAdafruit finger detect test");


   finger.begin(57600); 
  
    if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
    } 
    else 
    {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
    }
    finger.getTemplateCount();
    Serial.print("Sensor contains ");     
    Serial.print(finger.templateCount); Serial.println(" templates");
    Serial.println("Waiting for valid finger...");
  
     wifi_init();
}

void loop()
  {
  
    int  fp = getFingerprintIDez();
    Serial.print("fp Id is :");
    
    Serial.println(fp);

    delay(50);
    
    if(fp != -1)
  {
    String urlFinal = "https://script.google.com/macros/s/"+GOOGLE_SCRIPT_ID+"/exec?"+ "&sensor=" + fp;
    Serial.print("POST data to spreadsheet:");
    Serial.println(urlFinal);
    Buzzer();
    HTTPClient http;
    http.begin(urlFinal.c_str());
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    int httpCode = http.GET(); 
    Serial.print("HTTP Status Code: ");
    Serial.println(httpCode);
    
    //---------------------------------------------------------------------
    //getting response from google sheet
    String payload;
    
    if (httpCode > 0) {
        payload = http.getString();
        Serial.println("Payload: "+payload);    
    }    
    //---------------------------------------------------------------------
   
 http.end();
  }
  }
