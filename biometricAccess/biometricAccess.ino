#include <Adafruit_Fingerprint.h>
#include <SPI.h>
#include <SD.h>
#include <RtcDS1302.h>

SoftwareSerial mySerial(10, 11); //Connect Tx, Rx of FP sensor


Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

ThreeWire myWire(6,7,5); // IO, SCLK, CE (Data, clock, reset)
RtcDS1302<ThreeWire> Rtc(myWire);

// const int chipSelect = 4;

//File logFile;

uint8_t id;
void setup()
{
  Serial.begin(9600);
  Serial1.begin(9600);
  while (!Serial);  // For Yun/Leo/Micro/Zero/...
  delay(1);

  Serial.println(F("Running setup......"));
   Serial1.println(F("Running setup......"));

  // set the data rate for the sensor serial port
  finger.begin(57600);

  if (finger.verifyPassword()) {
    Serial.println(F("Found fingerprint sensor!"));
  } else {
    Serial.println(F("Did not find fingerprint sensor :("));
    while (1) { delay(1); }
  }

  // Serial.println(F("Reading sensor parameters"));
  //finger.getParameters();

  // Serial.print("---------------------------------------------------------");

  //Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(4)) {
    Serial.println(F("Card failed, or not present"));
    // don't do anything more:
    while (1);
  }
  Serial.println(F("\ncard initialized."));
  // Serial.print("---------------------------------------------------------");
  
    Serial.println(F("Place finger to authenticate"));
  
   Rtc.Begin();
}

void loop() {
  // Serial.println("Running loop...");

  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    if (input == "enroll") {
      // Enroll fingerprint
      enrollFingerprint();
    }
  }
  if (Serial1.available()) {
    int blInput = Serial1.read();
    Serial.println(blInput);
    if (blInput == "e") {
      // Enroll fingerprint
      enrollFingerprint();
    }
  }

  // Verification loop
  getFingerprintIDez();
  // delay(3000);
}

uint8_t readnumber(void) {
  uint8_t num = 0;

  while (num == 0) {
    while (! Serial.available());
    num = Serial.parseInt();
  }
  return num;
}

void enrollFingerprint() {
  Serial.println(F("Type in the ID # "));
  //Serial.println("Please type in the ID # (from 1 to 127) you want to save this finger as...");
  id = readnumber();
  if (id == 0) {// ID #0 not allowed, try again!
     return;
  }
  Serial.print(F("Enrolling ID #"));
  Serial.println(id);

  while (!  getFingerprintEnroll() ){
    delay(1000);
  };
}

uint8_t getFingerprintEnroll() {

  int p = -1;
  Serial.print(F("Waiting for valid finger to enroll as #")); Serial.println(id);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println(F("Image taken"));
      break;
    case FINGERPRINT_NOFINGER:
      //Serial.println(".");
      break;
    // case FINGERPRINT_PACKETRECIEVEERR:
    //   Serial.println("Communication error");
    //   break;
    // case FINGERPRINT_IMAGEFAIL:
    //   Serial.println("Imaging error");
    //   break;
    default:
      Serial.println(F("Error"));
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println(F("Image converted"));
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println(F("Image too messy"));
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println(F("Communication error"));
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println(F("Could not find fingerprint features"));
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println(F("Could not find fingerprint features"));
      return p;
    default:
      Serial.println(F("Unknown error"));
      return p;
  }

  Serial.println(F("Remove finger"));
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID "); Serial.println(id);
  p = -1;
  Serial.println(F("Place same finger again"));
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println(F("Image taken"));
      break;
    case FINGERPRINT_NOFINGER:
      //Serial.print(".");
      break;
    // case FINGERPRINT_PACKETRECIEVEERR:
    //   Serial.println("Communication error");
    //   break;
    // case FINGERPRINT_IMAGEFAIL:
    //   Serial.println("Imaging error");
    //   break;
    default:
      Serial.println(F("Error"));
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println(F("Image converted"));
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println(F("Image too messy"));
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println(F("Communication error"));
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println(F("Could not find fingerprint features"));
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println(F("Could not find fingerprint features"));
      return p;
    default:
      Serial.println(F("Unknown error"));
      return p;
  }

  // OK converted!
  Serial.print(F("Creating model for #"));  Serial.println(id);

  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println(F("Prints matched!"));
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println(F("Communication error"));
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println(F("Fingerprints did not match"));
    return p;
  } else {
    Serial.println(F("Unknown error"));
    return p;
  }

  Serial.print("ID "); Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println(F("Stored!"));
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println(F("Communication error"));
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println(F("Could not store in that location"));
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println(F("Error writing to flash"));
    return p;
  } else {
    Serial.println(F("Unknown error"));
    return p;
  }

  return true;
}



int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;

  // found a match!
  Serial.print(F("Found ID #")); Serial.print(finger.fingerID);
  Serial.print(F(" with confidence of ")); Serial.println(finger.confidence);
  RtcDateTime now = Rtc.GetDateTime();

  File logFile;

  char filename[20];
  //filename[0] = '\0';
  sprintf(filename, "%04d%02d%02d.txt", now.Year(), now.Month(), now.Day());
  delay(1000);
  
  logFile = SD.open(filename, FILE_WRITE);
   //delay(1000);
  if (logFile) {
    Serial.print(F("\nwriting to file: ")); Serial.print(filename);
    int id = finger.fingerID;
     logFile.print(now.Year(), DEC);
     logFile.print('/');
     logFile.print(now.Month(), DEC);
     logFile.print('/');
     logFile.print(now.Day());
     logFile.print(" ");
     logFile.print(now.Hour(), DEC);
     logFile.print(':');
     logFile.print(now.Minute(), DEC);
     logFile.print(':');
     logFile.print(now.Second(), DEC);
    logFile.print(" | ");
    logFile.print(id);
    logFile.print("\n");
    logFile.close();
    Serial.println(F("\nDone writing"));
    }else {
    Serial.print(F("error opening file: "));Serial.print(filename);
  }
  Serial.println(F("\nPlace finger to authenticate"));
  return finger.fingerID;
}


