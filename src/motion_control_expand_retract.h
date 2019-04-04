#include <Wire.h>
#include "pins_definitions.h"

// UP AND DOWN VARIABLES
int vel_expand_retract = 1024;


/* MOVEMENT FUNCTIONS */
void setup_expand_retract_movement();
void expand(Stream * serial_ref, PubSubClient * client, Stream * serial_atmega);
void retract(Stream * serial_ref, PubSubClient * client, Stream * serial_atmega);
void stop_expand_retract(Stream * serial_ref);


// UP AND DOWN MOVEMENT FUNCTIONS
void setup_expand_retract_movement() {

  pinMode(pin_expand, OUTPUT);
  pinMode(pin_retract, OUTPUT);
  pinMode(pin_stop_retract, INPUT);

  // COMPRESSOR
  digitalWrite(pin_expand, LOW);

  // VALVE
  digitalWrite(pin_retract, LOW);
}

void expand(Stream * serial_ref, PubSubClient * client, Stream * serial_atmega) {

  if (digitalRead(pin_up_stop) == LOW) {
    serial_ref->println("** expand **");
    long lastMsgTime = millis();
    long expandStartTime = millis();
    long currentTime = millis();

    digitalWrite(pin_retract, LOW);
    digitalWrite(pin_expand,  HIGH);
    while(true) {
      currentTime = millis();

      if (currentTime - expandStartTime >= 45000) {
        break;
      }

      if (currentTime - lastMsgTime > 5000) {
        lastMsgTime = currentTime;
        const char* busyString = "busy";
        serial_ref->print("status: ");
        serial_ref->println(busyString);
        client->publish("status", busyString);
      }
    }

    delay(50);


    digitalWrite(pin_retract, LOW);
    digitalWrite(pin_expand,  LOW);
    delay(100);

    serial_atmega->println("$X");
    serial_ref->println("$X");
    delay(100);
    serial_atmega->println("$H");
    serial_ref->println("$H");
    //delay(20000);
    /********* DELAY 20000 WITH MQTT *********/
    lastMsgTime = millis();
    currentTime = millis();
    int i;
    for(i=0; i<12; i++) {
      currentTime = millis();
      if (currentTime - lastMsgTime >= 5000) {
        lastMsgTime = currentTime;
      }
      const char* busyString = "busy";
      serial_ref->print("status: ");
      serial_ref->println(busyString);
      client->publish("status", busyString);
      delay(5000);
    }
    /***************************************/
  } else {
    serial_ref->println("** cannot expand **");
  }
}

void retract(Stream * serial_ref, PubSubClient * client, Stream * serial_atmega) {
  serial_atmega->println("G1 X-90 Y-5 Z-5 F600");
  serial_ref->println("G1 X-90 Y-5 Z-5 F600");

  //delay(20000);
  /********* DELAY 20000 WITH MQTT *********/
  long lastMsgTime = millis();
  long currentTime = millis();
  int i;
  for(i=0; i<4; i++) {
    currentTime = millis();
    if (currentTime - lastMsgTime >= 5000) {
      lastMsgTime = currentTime;
    }
    const char* busyString = "busy";
    serial_ref->print("status: ");
    serial_ref->println(busyString);
    client->publish("status", busyString);
    delay(5000);
  }
  /***************************************/

  serial_ref->println("** retract **");
  lastMsgTime = millis();
  currentTime = millis();



  while(digitalRead(pin_stop_retract) == LOW) {
    digitalWrite(pin_expand,  LOW);
    digitalWrite(pin_retract, HIGH);
    currentTime = millis();
    if (currentTime - lastMsgTime >= 5000) {
      lastMsgTime = currentTime;

      const char* busyString = "busy";
      serial_ref->print("status: ");
      serial_ref->println(busyString);
      client->publish("status", busyString);
    }
    delay(50);
  }
  delay(4000);
  digitalWrite(pin_expand,  LOW);
  digitalWrite(pin_retract, LOW);
}

void stop_expand_retract(Stream * serial_ref) {
  serial_ref->println("** stop up and down **");

  digitalWrite(pin_retract, LOW);
  digitalWrite(pin_expand,  LOW);
}
