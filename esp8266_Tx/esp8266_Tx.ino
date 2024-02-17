#include <ESP8266WiFi.h>
#include <espnow.h>

const int trigPin = D1;  // define the trigger pin
const int echoPin = D2;  // define the echo pin
int offButton = D7;

long duration;  // variable to store the duration of sound wave travel
int distance;   // variable to store the distance calculated in centimeters
uint8_t broadcastAddress[] = { 0x08, 0xF9, 0xE0, 0x6B, 0x2A, 0xAE };

typedef struct struct_message {
  bool sw1;
  bool sw2;
} struct_message;

// Create a struct_message called outgoingReadings to hold outgoing data
struct_message outgoingReadings;


// Variable to store if sending data was successful
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
  pinMode(trigPin, OUTPUT);  // set the trigger pin as output
  pinMode(echoPin, INPUT);   // set the echo pin as input
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

  // Register for a callback function that will be called when data is received
  // esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  //digitalWrite(offButton, HIGH);
  int buttonstate4 = digitalRead(offButton);
  Serial.print(F("button state: "));
  Serial.println(buttonstate4);

  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  distance = duration * 0.034 / 2;
  // Prints the distance on the Serial Monitor
  Serial.print(F("Distance: "));
  Serial.print(distance);
  Serial.println(" cm");
    if (distance < 100) {
      Serial.print(F("Trigger 1: "));
      outgoingReadings.sw1 = 1;
      outgoingReadings.sw2 = (buttonstate4 == 0) ? 1 : 0;
      esp_now_send(broadcastAddress, (uint8_t *)&outgoingReadings, sizeof(outgoingReadings));
    } else {
      Serial.print(F("Trigger 2: "));
      outgoingReadings.sw1 = 0;
      outgoingReadings.sw2 = (buttonstate4 == 0) ? 1 : 0;
      esp_now_send(broadcastAddress, (uint8_t *)&outgoingReadings, sizeof(outgoingReadings));
    }


   delay(500); // wait for a second before taking the next reading
}