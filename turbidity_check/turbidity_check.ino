#define TurbiditySensorPin 34  // ADC pin on ESP32

float voltage = 0;
float ntu = 0;
float baselineNTU = 1700;  // NTU reading when sensor is in air

void setup() {
  Serial.begin(115200);
}

void loop() {
  // Read raw analog value (12-bit ADC: 0–4095)
  int sensorValue = analogRead(TurbiditySensorPin);

  // Convert ADC value to voltage (ESP32 reference 3.3V)
  voltage = sensorValue * (3.3 / 4095.0);

  // Convert voltage to NTU using DFRobot formula
  ntu = -1120.4 * voltage * voltage + 5742.3 * voltage - 4352.9;

  // Subtract baseline NTU (air reading)
  float correctedNTU = ntu - baselineNTU;

  // Avoid negative NTU values
  if (correctedNTU < 0) correctedNTU = 0;

  Serial.print("Raw ADC: ");
  Serial.print(sensorValue);
  Serial.print(" | Voltage: ");
  Serial.print(voltage, 2);
  Serial.print(" V | Turbidity: ");
  Serial.print(correctedNTU, 2);
  Serial.println(" NTU");

  delay(1000);
}
