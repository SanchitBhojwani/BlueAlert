#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is connected to GPIO 4 (change as needed)
#define ONE_WIRE_BUS 4

// Setup a oneWire instance
OneWire oneWire(ONE_WIRE_BUS);

// Pass oneWire to Dallas Temperature library
DallasTemperature sensors(&oneWire);

void setup() {
  Serial.begin(115200);
  sensors.begin(); // Start the sensor
}

void loop() {
  sensors.requestTemperatures();          // Send command to get temperatures
  float tempC = sensors.getTempCByIndex(0); // Get temperature of first sensor

  Serial.print("Temperature: ");
  Serial.print(tempC);
  Serial.println(" °C");

  delay(1000); // Wait 1 second
}
