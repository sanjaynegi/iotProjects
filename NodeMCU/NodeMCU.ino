
#define BLYNK_PRINT Serial


#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "AgDMFvuqmiPPHuseOefVkJzLfIK4z9vG";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Negi";
char pass[] = "24071986";
int pinState = 0;

void setup()
{
  // Debug console
  pinMode(D7, OUTPUT);
  Serial.begin(9600);
  digitalWrite(D7, HIGH);
  Blynk.begin(auth, ssid, pass);
  Serial.println ("setup");
  // You can also specify server:
  //Blynk.begin(auth, ssid, pass, "blynk-cloud.com");
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080);
}
BLYNK_CONNECTED() {
  Serial.println ("setting low initially on internet connect");
   digitalWrite(D8, HIGH);
  Blynk.syncVirtual(V7);
}

BLYNK_WRITE(V8)
{
  int pinValue = param.asInt();
  Serial.println (pinValue);

  if (pinValue == 1) {
    Serial.println ("high input");
    //digitalWrite(LED_BUILTIN, LOW);
    digitalWrite(D7, LOW);
    Blynk.virtualWrite(V8, HIGH);
  } else if (pinValue == 0){
    Serial.println ("low input");
    //digitalWrite(LED_BUILTIN, HIGH);
    digitalWrite(D7, HIGH);
    Blynk.virtualWrite(V8, LOW);
  }
}
BLYNK_APP_DISCONNECTED() {
Serial.println ("setting low if internet disconnected");
   digitalWrite(D7, HIGH);
}

void loop()
{
  Blynk.run(); 
}
