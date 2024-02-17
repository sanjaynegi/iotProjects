

#include <ESP8266WiFi.h>
// #include <WiFi.h>
#include <espnow.h>


const int buzzerPin =  D1;

typedef struct struct_message {
  bool sw1;
  bool sw2;
} struct_message;



struct_message incomingReadings;

// Variable to store if sending data was successful
String success;



// Callback when data is received
// void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
void OnDataRecv(uint8_t *mac, uint8_t *incomingData, uint8_t len) {
  memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));

  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("Switch 1: ");
  Serial.println(incomingReadings.sw1);
  Serial.print("Switch 2: ");
  Serial.println(incomingReadings.sw2);

  if (incomingReadings.sw2 == 0) {
    Serial.print(F("trigger on sw2: "));
    digitalWrite(buzzerPin, LOW);
    digitalWrite(LED_BUILTIN, HIGH);
   return;
  }

  if (incomingReadings.sw1 == 1) {
    Serial.println(F("trigger on sw1: "));
    digitalWrite(buzzerPin, HIGH);
    digitalWrite(LED_BUILTIN, LOW);
  } 
  
}

void setup() {
  // Init Serial Monitor
  Serial.begin(9600);
  Serial.println("Code start");
  pinMode(buzzerPin, OUTPUT);
  //pinMode(LED_BUILTIN, OUTPUT);
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println(F("Error initializing ESP-NOW"));
    return;
  }
  //esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  // Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(OnDataRecv);
}

void buzzerTone(){
  tone(buzzerPin, 1000); // Set the buzzer to 1000 Hz
  delay(1000); // Wait for 1 second
  
  // Play a tone of 2000 Hz for 1 second
  tone(buzzerPin, 2000); // Set the buzzer to 2000 Hz
  delay(1000); // Wait for 1 second
  
  // Play a tone of 3000 Hz for 1 second
  tone(buzzerPin, 3000); // Set the buzzer to 3000 Hz
  delay(1000); // Wait for 1 second
}

void loop() {
}
