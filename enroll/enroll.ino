#include <Adafruit_Fingerprint.h>

#if (defined(__AVR__) || defined(ESP8266)) && !defined(__AVR_ATmega2560__)
// For UNO and others without hardware serial, we must use software serial...
// pin #2 is IN from sensor (GREEN wire)
// pin #4 is OUT from arduino  (WHITE wire)
// Set up the serial port to use softwareserial..
SoftwareSerial mySerial(2, 4); // RX, TX
SoftwareSerial mySerial2(7, 8); // RX, TX
#else
// On Leonardo/M0/etc, others with hardware serial, use hardware serial!
// #0 is green wire, #1 is white
#define mySerial Serial1
#endif

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
uint8_t id = 0;

#define BBSENSOR 12
uint8_t motor_pin_array[6] = {3, 5, 6, 9, 10, 11};
bool turn_motors = true;
bool motorstate1 = true;
uint8_t sensorState, lastState = 0;
uint32_t prev_millis = 0;

void setup(){
  Serial.begin(9600);
  while (!Serial);  // For Yun/Leo/Micro/Zero/...
  mySerial2.begin(9600);
  while(!mySerial2);

  // Initialize GPIO
  for (uint8_t i; i < 6; i++) {
    pinMode(motor_pin_array[i], OUTPUT);
  }
  pinMode(BBSENSOR, INPUT);
  digitalWrite(BBSENSOR, HIGH);

  delay(100);
  Serial.println("\n\nAdafruit Fingerprint sensor enrollment");

  // set the data rate for the sensor serial port
  finger.begin(57600);

  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  }

  finger.emptyDatabase();
}


uint8_t readnumber(void) {
  uint8_t num = 0;

  while (num == 0) {
    while (! Serial.available());
    num = Serial.parseInt();
  }
  return num;
}


void defaultIdle() {
  // mySerial.listen();
  // Serial.println(mySerial.isListening());
  // Serial.println(mySerial2.isListening());
  // Serial.println("---1");
  // mySerial2.listen();
  // Serial.println(mySerial.isListening());
  // Serial.println(mySerial2.isListening());
  // Serial.println("---2");
  // delay(500);
  
  // while( mySerial2.available() ) {
  //   c = mySerial2.read();
  //   if (c == 'e') {
  //     doEnroll();
  //   } else {

  //   }
  // }
}
uint8_t c;
uint8_t val;
void loop()                     // run over and over again
{
  mySerial2.listen();
  // while (mySerial2.available()) {
  //   c = mySerial2.read();
    c = listen_for_packet();

    /// c == 130 means that myserial2 enrolls 
    if (c == 130) {
      mySerial.listen();
      while(!getFingerprintEnroll());
      mySerial2.write(val);
      mySerial2.write(0xFF);
    }
    if (c != 0 && c < 129) {  //gets fingerprint and compares 
      mySerial.listen();
      while(!test(c));
      mySerial2.write(val);
      mySerial2.write(0xFF);
    } 
  // }
}

uint8_t listen_for_packet() {
  uint8_t stored_packet;
  uint8_t packet;
  bool check = false;
  mySerial2.listen();
  while (mySerial2.available()) {
    packet = mySerial2.read(); 
    if (packet != 0xFF) {
      stored_packet = packet;
      packet = mySerial2.read();
      if (packet == 0xFF) {
        return stored_packet;
      }
    }
  }
  return 0xFF;
}

uint8_t tries = 0;
uint8_t cselec = 0;
int test(uint8_t sid) {
  int p = -1;
  
  while (p != FINGERPRINT_OK) {
    Serial.println("Place finger on sensor...");
    p = finger.getImage();
  }
  
  
  p = finger.image2Tz();
  p = finger.fingerSearch(sid);
  
  if (finger.confidence < 10){
    Serial.println("Not found");
    tries ++;
    while (tries < 2) {
      Serial.println("Place finger again");
      p = finger.getImage();
      p = finger.image2Tz();
      p = finger.fingerSearch(sid);
      if (finger.confidence > 10) {
        tries = 0;
        break;
      }
      tries ++;
    }
    if (tries == 2){
      tries = 0 ;
      val = 129;//failure
      return -1;
    }
  }


  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  val = 131; //sucessful
  mySerial2.write(val);

  turn_motors = true;
  mySerial2.listen();
  //242 chute 1  249 chute 2 
  while(cselec == 0) {
    if(mySerial2.available()){
      cselec = mySerial2.read();
      Serial.println(cselec);
    }
  }

  while(turn_motors){

    if (cselec == 242){
      cselec = 201;
    }
    if (cselec == 249){
      cselec = 202;
    }
    turnMotor(motor_pin_array[cselec-201]);
  
    
  }
  val = 133; //successfull pill dispensing
  mySerial2.write(val); 
  return finger.fingerID;
  
}


uint8_t getFingerprintEnroll() {
  id ++;
  int p = -1;
  Serial.print("Waiting for valid finger to enroll as #"); Serial.println(id);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println(".");
      break;
    default:
      Serial.println("Unknown error1");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error2");
      return p;
  }

  Serial.println("Remove finger");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID "); Serial.println(id);
  p = -1;
  Serial.println("Place same finger again");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.print(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error3");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error4");
      return p;
  }

  // OK converted!
  Serial.print("Creating model for #");  Serial.println(id);

  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Fingerprints did not match");
    id --;
    val = 132;
    return p;
  } else {
    Serial.println("Unknown error5");
    return p;
  }

  Serial.print("ID "); Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("Stored!");
  } 
  else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not store in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    return p;
  } 
    else {
      Serial.println("Unknown error6");
      id --;
      val = 132;
      return p;
  }

  val = id;  
  return true;
}

void turnMotor(uint8_t motor_pin) {
  if (turn_motors) {
    if (motorstate1) {
      if (millis() > prev_millis + 50) {
        motorstate1 = false;
        digitalWrite(motor_pin, 0);
      } else {
        digitalWrite(motor_pin, 1);
      }
    }
    if (millis() > prev_millis + 500) {
      prev_millis = millis();
      motorstate1 = true;
    }
  }

  if (digitalRead(BBSENSOR) == LOW) {
    digitalWrite(motor_pin, 0);
    turn_motors = false;
    Serial.println("Broken");
  }
}
