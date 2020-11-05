#define USE_ULTRASONIC 0
#define USE_TEMP 1

#if USE_ULTRASONIC
#include <HCSR04.h>
#endif

#if USE_TEMP
#include <DallasTemperature.h>
#endif

//Mandatory for device simulation
#include <OneWireHub.h>
#include <OneWireHub_config.h>
#include <OneWireItem.h>
#include <platform.h>
#include <PoolReaderDevice.h>

//ATINY85
//#define pin_onewire 4
//#define pin_temp_1W 1
//#define pin_trigger 5
//#define pin_echo 0
//#define pin_PH A3
//#define pin_ORP A1

//Uno
#define pin_onewire 9
#define pin_temp_1W 10
#define pin_trigger 11
#define pin_echo 12
#define pin_PH A3
#define pin_ORP A1

OneWireHub hub(pin_onewire);
PoolReaderDevice board(PoolReaderDevice::family_code, 0x00, 0x00, 0xB2, 0xDA, 0x18, 0x00);

#if USE_ULTRASONIC
UltraSonicDistanceSensor distanceSensor(pin_trigger, pin_echo);
#endif

#if USE_TEMP
OneWire oneWire(pin_temp_1W);
DallasTemperature sensors(&oneWire);
DeviceAddress insideThermometer;
#endif

unsigned long interval = 0;
unsigned long previousCall = 0;

void setup() {
  hub.attach(board);

#if USE_TEMP
  sensors.begin();
  sensors.getAddress(insideThermometer, 0);
  sensors.setResolution(insideThermometer, 12);
#endif

  interval = board.state.interval * 1000;
  board.setTemperature(0);
  board.setOrpRaw(0);
  board.setPhRaw(0);
  board.setWaterLevel(0);
  
  updatedTriggered();
}

void updatedTriggered() {

#if USE_TEMP
  sensors.requestTemperatures();
  float T = sensors.getTempC(insideThermometer);
  board.setTemperature(T);
#endif

  board.setOrpRaw(analogRead(pin_ORP));
  board.setPhRaw(analogRead(pin_PH));

#if USE_ULTRASONIC
  double distance = distanceSensor.measureDistanceCm(T);
  board.setWaterLevel((distance > 0) ? distance : 25.5f);
#endif

  //update timer
  interval = board.state.interval * 1000;
}

void loop() {

  // put your main code here, to run repeatedly:
  // following function must be called periodically
  hub.poll();
  //  this part is just for debugging (USE_SERIAL_DEBUG in OneWire.h must be enabled for output)
  //  if (hub.hasError()) {
  //    auto _error = hub.getError();
  //    if (_error == Error::NO_ERROR) return;
  //    Serial.print("Error: ");
  //    if (_error == Error::READ_TIMESLOT_TIMEOUT) Serial.print("read timeslot timeout");
  //    else if (_error == Error::WRITE_TIMESLOT_TIMEOUT) Serial.print("write timeslot timeout");
  //    else if (_error == Error::WAIT_RESET_TIMEOUT) Serial.print("reset wait timeout");
  //    else if (_error == Error::VERY_LONG_RESET) Serial.print("very long reset");
  //    else if (_error == Error::VERY_SHORT_RESET) Serial.print("very short reset");
  //    else if (_error == Error::PRESENCE_LOW_ON_LINE) Serial.print("presence low on line");
  //    else if (_error == Error::READ_TIMESLOT_TIMEOUT_LOW) Serial.print("read timeout low");
  //    else if (_error == Error::AWAIT_TIMESLOT_TIMEOUT_HIGH) Serial.print("await timeout high");
  //    else if (_error == Error::PRESENCE_HIGH_ON_LINE) Serial.print("presence high on line");
  //    else if (_error == Error::INCORRECT_ONEWIRE_CMD) Serial.print("incorrect onewire command");
  //    else if (_error == Error::INCORRECT_SLAVE_USAGE) Serial.print("slave was used in incorrect way");
  //    else if (_error == Error::TRIED_INCORRECT_WRITE) Serial.print("tried to write in read-slot");
  //    else if (_error == Error::FIRST_TIMESLOT_TIMEOUT) Serial.print("found no timeslot after reset / presence (is OK)");
  //    else if (_error == Error::FIRST_BIT_OF_BYTE_TIMEOUT) Serial.print("first bit of byte timeout");
  //
  //    if ((_error == Error::INCORRECT_ONEWIRE_CMD) || (_error == Error::INCORRECT_SLAVE_USAGE))
  //    {
  //      Serial.println("INCORRECT_ONEWIRE_CMD or INCORRECT_SLAVE_USAGE");
  //    } else
  //    {
  //      Serial.println("");
  //    }

  //}

  unsigned long currentTime = millis();
  if (currentTime - previousCall >= interval)
  {
    previousCall = currentTime;
    updatedTriggered();
  }

}
