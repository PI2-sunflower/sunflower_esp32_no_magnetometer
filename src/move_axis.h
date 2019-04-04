#include <Wire.h>

void go_home(Stream * serial_atmega, Stream * serial_ref);
void move_axis(String messageTemp, Stream * serial_atmega, Stream * serial_ref);
void clean_arduino_read_buffer(Stream * serial_atmega);


void clean_arduino_read_buffer(Stream * serial_atmega) {
  while(serial_atmega->available() > 0){
    serial_atmega->read();
  }
}

void go_home(Stream * serial_atmega, Stream * serial_ref){
  serial_atmega->println("$H");
  serial_ref->println("$H");
  delay(200);
}

void unlock(Stream * serial_atmega, Stream * serial_ref){
  serial_atmega->println("$X");
  serial_ref->println("$X");
  delay(200);
}


void move_axis(String messageTemp, Stream * serial_atmega, Stream * serial_ref) {
  clean_arduino_read_buffer(serial_atmega);

  serial_ref->println(messageTemp);
  serial_atmega->println(messageTemp);

  delay(50);
  if (serial_atmega->read() == 'ok') {
    serial_ref->println('ok');
  }

}
