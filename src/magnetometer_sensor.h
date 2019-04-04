void setup_magnetometer(Stream * serial_ref, Adafruit_BNO055 * bno) {
  if(!bno->begin()) {
    serial_ref->print("No BNO055 detected!");
    while(100);
  }
  delay(1000);
  bno->setExtCrystalUse(true);
}

String get_data(Stream * serial_ref, Adafruit_BNO055 * bno) {

  String magString = "";

  sensors_event_t event;
  bno->getEvent(&event);

  magString += "X:"+String(event.orientation.x,4);
  magString += ",Y:"+String(event.orientation.y,4);
  magString += ",Z:"+String(event.orientation.z,4);

  return magString;
}
