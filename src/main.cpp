#include <PubSubClient.h>
#include <Wire.h>
#include <WiFi.h>
#include <UIPEthernet.h>
#include <HardwareSerial.h>
// // Magnetometer
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

#include "motion_control_up_down.h"
#include "motion_control_expand_retract.h"
#include "move_axis.h"
//#include "magnetometer_sensor.h"

#include "pins_definitions.h"

/* SENSORS VARIABLES */
long lastMsg = 0;
HardwareSerial SerialATMEGA(2);

Adafruit_BNO055 bno = Adafruit_BNO055(55);



/* CONNECTION AND MQTT FUNCTIONS */
void setup_wifi(char* ssid, char* password);
void callback(char* topic, byte* message, unsigned int length);


WiFiClient espClient;
//EthernetClient espClient;
PubSubClient client(espClient);



void setup() {
  Serial.begin(115200);
  SerialATMEGA.begin(115200, SERIAL_8N1, 16, 17);

  setup_wifi("girassol", "0987654321");
  //setup_wifi("termofluidos", "fg@t&rm0flwydos");

  // uint8_t mac[6] = {0x00,0x01,0x02,0x03,0x04,0x05};
  // Ethernet.begin(mac,IPAddress(192,168,1,140));

  //const char* mqtt_server = "192.168.43.68";
  const char* mqtt_server = "192.168.0.101";
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);


  setup_up_down_movement();
  setup_expand_retract_movement();
  //setup_magnetometer(&Serial, &bno);

  pinMode(pin_reset, OUTPUT);
  digitalWrite(pin_reset, HIGH);
}


void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;

  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }

  Serial.println();

  // If a message is received on the topic movement/up_down
  if (String(topic) == "movement/up_down") {
    Serial.print("Movement up_down: ");
    if(messageTemp == "up"){
      go_up(&Serial, &client, &SerialATMEGA);
    }
    else if(messageTemp == "down"){
      go_down(&Serial, &client, &SerialATMEGA);
    }
    else if(messageTemp == "stop"){
      stop_up_down(&Serial);
    }
  }

  // If a message is received on the topic movement/expand_retract
  if (String(topic) == "movement/expand_retract") {
    Serial.print("Movement expand_retract: ");
    if(messageTemp == "expand"){
      expand(&Serial, &client, &SerialATMEGA);
    }
    else if(messageTemp == "retract"){
      retract(&Serial, &client, &SerialATMEGA);
    }
    else if(messageTemp == "stop"){
      stop_expand_retract(&Serial);
    }
  }

  if (String(topic) == "movement/axis") {
    Serial.print("Moving axis: ");

    if (digitalRead(pin_down_stop) == HIGH) {
      if(messageTemp == "go_home"){
        go_home(&SerialATMEGA, &Serial);
      }
      else if (messageTemp == "unlock"){
        unlock(&SerialATMEGA, &Serial);
      }
      else if (messageTemp == "reset_axis"){

          digitalWrite(pin_reset, LOW);
          delay(100);
          digitalWrite(pin_reset, HIGH);
          delay(1000);

          SerialATMEGA.println("$X");
          Serial.println("$X");
          delay(100);

          SerialATMEGA.println("$H");
          Serial.println("$H");
          delay(100);

      }
      else {
        move_axis(messageTemp, &SerialATMEGA, &Serial);
      }
    } else {
      Serial.println("Invalid Movement");
    }

  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("movement/up_down");
      client.subscribe("movement/expand_retract");
      client.subscribe("movement/axis");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;


    const char* aliveString = "alive";
    Serial.print("status: ");
    Serial.println(aliveString);
    client.publish("status", aliveString);

    //String magnetString = get_data(&Serial, &bno);
    //char magnetStringChar [40];
    //magnetString.toCharArray(magnetStringChar, 40);
    //Serial.print("Magnetometer: ");
    //Serial.println(magnetStringChar);
    //client.publish("sensor/magnetometer", magnetStringChar);

  }
}




void setup_wifi(char* ssid, char* password) {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
