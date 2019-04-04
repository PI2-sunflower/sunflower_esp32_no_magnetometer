#include <Wire.h>
#include "pins_definitions.h"


/* MOVEMENT FUNCTIONS */
void setup_up_down_movement();
void go_up(Stream * serial_ref, PubSubClient * client, Stream * serial_atmega);
void go_down(Stream * serial_ref, PubSubClient * client, Stream * serial_atmega);
void stop_up_down(Stream * serial_ref);

// UP AND DOWN MOVEMENT FUNCTIONS
void setup_up_down_movement() {

  pinMode(pin_up, OUTPUT);
  pinMode(pin_down, OUTPUT);
  pinMode(pin_up_stop, INPUT);
  pinMode(pin_down_stop, INPUT);

  digitalWrite(pin_up, LOW);
  digitalWrite(pin_down, LOW);
}

void go_up(Stream * serial_ref, PubSubClient * client, Stream * serial_atmega) {
  serial_ref->println("** up **");
  long lastMsgTime = millis();
  long currentTime = millis();


  while(digitalRead(pin_up_stop) == HIGH) {
    digitalWrite(pin_down, LOW);
    digitalWrite(pin_up, HIGH);

    currentTime = millis();
    if (currentTime - lastMsgTime > 5000) {
      lastMsgTime = currentTime;

      const char* busyString = "busy";
      serial_ref->print("status: ");
      serial_ref->println(busyString);
      client->publish("status", busyString);
    }
    delay(10);
  }
  digitalWrite(pin_down, LOW);
  digitalWrite(pin_up, LOW);
  delay(300);

  digitalWrite(pin_reset, LOW);
  delay(100);
  digitalWrite(pin_reset, HIGH);
  delay(1000);


  serial_atmega->println("$X");
  serial_ref->println("$X");
  delay(100);


  serial_atmega->println("$H");
  serial_ref->println("$H");
  delay(100);


  serial_atmega->println("G1 X-90 Y-5 Z-5 F600");
  serial_ref->println("G1 X-90 Y-5 Z-5 F600");
  delay(100);

  /********* DELAY 20000 WITH MQTT *********/
  lastMsgTime = millis();
  currentTime = millis();
  int i;
  for(i=0; i<10; i++) {
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

}

void go_down(Stream * serial_ref, PubSubClient * client, Stream * serial_atmega) {
  serial_ref->println("** down **");

  serial_atmega->println("G1 X0 Y0 Z0 F1000");
  serial_ref->println("G1 X0 Y0 Z0 F1000");
  //delay(15000);
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


  lastMsgTime = millis();
  currentTime = millis();

  while(digitalRead(pin_down_stop) == HIGH && digitalRead(pin_stop_retract) == HIGH) {
    digitalWrite(pin_up, LOW);
    digitalWrite(pin_down, HIGH);

    currentTime = millis();
    if (currentTime - lastMsgTime > 5000) {
      lastMsgTime = currentTime;

      const char* busyString = "busy";
      serial_ref->print("status: ");
      serial_ref->println(busyString);
      client->publish("status", busyString);
    }
    delay(50);
  }
  serial_ref->println("*** end down ***");

  digitalWrite(pin_down, LOW);
  digitalWrite(pin_up, LOW);
  delay(300);

}

void stop_up_down(Stream * serial_ref) {
  serial_ref->println("** stop up and down **");

  digitalWrite(pin_down, LOW);
  digitalWrite(pin_up, LOW);
}
