#include <OneWire.h>
#include <DallasTemperature.h>

// ----------------- Pin Definitions -----------------
#define TdsSensorPin 35       // ADC pin for TDS sensor
#define TurbiditySensorPin 34 // ADC pin for Turbidity sensor
#define ONE_WIRE_BUS 4        // GPIO for DS18B20

// ----------------- DS18B20 Setup -----------------
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// ----------------- Variables -----------------
float tdsValue = 0, tdsVoltage = 0;
float turbidityVoltage = 0, ntu = 0;
float baselineNTU = 1700;  // NTU reading when sensor is in air
float tempC = 0;

void setup() {
  Serial.begin(115200);
  pinMode(TdsSensorPin, INPUT);
  sensors.begin();  // Start DS18B20 sensor
}

void loop() {
  // ----------- TDS Sensor -----------
  int tdsAnalog = analogRead(TdsSensorPin);
  tdsVoltage = tdsAnalog * 3.3 / 4095.0;
  tdsValue = (tdsVoltage * 1000) / 2.3; // Approximate TDS in ppm

  // ----------- Turbidity Sensor -----------
  int turbidityAnalog = analogRead(TurbiditySensorPin);
  turbidityVoltage = turbidityAnalog * (3.3 / 4095.0);
  ntu = -1120.4 * turbidityVoltage * turbidityVoltage + 5742.3 * turbidityVoltage - 4352.9;
  float correctedNTU = ntu - baselineNTU;
  if (correctedNTU < 0) correctedNTU = 0;

  // ----------- DS18B20 Temperature Sensor -----------
  sensors.requestTemperatures();          
  tempC = sensors.getTempCByIndex(0);

  // ----------- Print All Values -----------
  Serial.print("TDS: "); 
  Serial.print(tdsValue, 0); 
  Serial.print(" ppm | ");

  Serial.print("Turbidity: "); 
  Serial.print(correctedNTU, 2); 
  Serial.print(" NTU | ");

  Serial.print("Temperature: "); 
  Serial.print(tempC, 2); 
  Serial.println(" °C");

  delay(1000); // 1-second delay
}
