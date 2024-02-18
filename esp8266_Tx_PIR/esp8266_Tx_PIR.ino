#include <ESP8266WiFi.h>
#include <espnow.h>

const int PIR_OUTPUT = D5;  // define the trigger pin
int offButton = D7;

uint8_t broadcastAddress[] = { 0x08, 0xF9, 0xE0, 0x6B, 0x2A, 0xAE };

typedef struct struct_message {
  bool sw1;
  bool sw2;
} struct_message;

struct_message outgoingReadings;


String success;

// Callback when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print(F("Last Packet Send Status: "));
  if (sendStatus == 0) {
    Serial.println(F("Delivery success"));
  } else {
    Serial.println(F("Delivery fail"));
  }
}

void setup() {
  Serial.begin(9600);        // initialize serial communication
  pinMode(PIR_OUTPUT, INPUT);   // set the echo pin as input
  pinMode(offButton, INPUT_PULLUP);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println(F("Error initializing ESP-NOW"));
    return;
  }

  // Set ESP-NOW Role
  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);

  // Register peer
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_COMBO, 0, NULL, 0);

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);

}

void loop() {
  int buttonstate4 = digitalRead(offButton);
 // Serial.print(F("button state: "));
  //Serial.println(buttonstate4);

  int pir_value = digitalRead(PIR_OUTPUT);
 // Serial.print(F("pir_value: "));
  //Serial.print(pir_value);

    if (pir_value == HIGH) {
      Serial.print(F("Trigger 1: "));
      outgoingReadings.sw1 = 1;
      outgoingReadings.sw2 = (buttonstate4 == 0) ? 1 : 0;
      esp_now_send(broadcastAddress, (uint8_t *)&outgoingReadings, sizeof(outgoingReadings));
    } else {
     // Serial.print(F("Trigger 2: "));
      outgoingReadings.sw1 = 0;
      outgoingReadings.sw2 = (buttonstate4 == 0) ? 1 : 0;
      esp_now_send(broadcastAddress, (uint8_t *)&outgoingReadings, sizeof(outgoingReadings));
    }


   delay(1000); // wait for a second before taking the next reading
}