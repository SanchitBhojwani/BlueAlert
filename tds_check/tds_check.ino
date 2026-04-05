#define TdsSensorPin 35   // ESP32 ADC pin where TDS sensor is connected

float tdsValue = 0;
float voltage = 0;

void setup() {
  Serial.begin(115200);
  pinMode(TdsSensorPin,INPUT);
}

void loop() {
  // Read analog value (12-bit ADC, range 0-4095 for 0-3.3V)
  int analogValue = analogRead(TdsSensorPin);

  // Convert ADC value to voltage
  voltage = analogValue * 3.3 / 4095.0;

  // Convert voltage to TDS (approx formula, calibration needed)
  // This assumes the TDS sensor outputs ~0–2.3V for 0–1000 ppm
  tdsValue = (voltage * 1000) / 2.3;

  Serial.print("Analog Value: ");
  Serial.print(analogValue);
  Serial.print(" | Voltage: ");
  Serial.print(voltage, 2);
  Serial.print(" V | TDS: ");
  Serial.print(tdsValue, 0);
  Serial.println(" ppm");

  delay(1000);
}
