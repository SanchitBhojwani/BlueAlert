#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <XPT2046_Touchscreen.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// ================= TFT PINS =================
#define TFT_CS   5
#define TFT_DC   2
#define TFT_RST  4
// ESP32 Hardware SPI pins:
// MOSI = 23, MISO = 19, SCK = 18

// ================= TOUCH PINS =================
#define TOUCH_CS 15
#define TOUCH_IRQ 27  // optional (can leave unconnected)

// ================= SENSOR PINS =================
#define TdsSensorPin 35       // ADC pin for TDS sensor
#define TurbiditySensorPin 34 // ADC pin for Turbidity sensor
#define ONE_WIRE_BUS 32       // DS18B20 pin

// ----------------- DS18B20 Setup -----------------
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// ----------------- TFT + Touch Objects -----------------
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);
XPT2046_Touchscreen ts(TOUCH_CS, TOUCH_IRQ);

// ----------------- Variables -----------------
float tdsValue = 0, tdsVoltage = 0;
float turbidityVoltage = 0, ntu = 0;
float baselineNTU = 1700;  // Adjust after calibration
float tempC = 0;

// ----------------- ICONS (16x16) -----------------
const unsigned char waterDrop [] PROGMEM = {
  0x00,0x18,0x00,0x00,0x3C,0x00,0x00,0x7E,0x00,0x00,0xFF,0x00,
  0x01,0xFF,0x80,0x03,0xFF,0xC0,0x07,0xFF,0xE0,0x0F,0xFF,0xF0,
  0x0F,0xFF,0xF0,0x07,0xFF,0xE0,0x03,0xFF,0xC0,0x01,0xFF,0x80,
  0x00,0xFF,0x00,0x00,0x7E,0x00,0x00,0x3C,0x00,0x00,0x18,0x00
};

const unsigned char thermometer [] PROGMEM = {
  0x0E,0x00,0x11,0x00,0x11,0x00,0x11,0x00,0x11,0x00,0x11,0x00,
  0x11,0x00,0x11,0x00,0x0E,0x00,0x04,0x00,0x04,0x00,0x04,0x00,
  0x04,0x00,0x04,0x00,0x04,0x00,0x07,0x00
};

const unsigned char cloud [] PROGMEM = {
  0x1E,0x00,0x21,0x00,0x40,0x80,0x40,0x80,0x21,0x00,0x1E,0x00,
  0x00,0x00,0x0F,0xC0,0x10,0x20,0x20,0x10,0x20,0x10,0x10,0x20,
  0x0F,0xC0,0x00,0x00
};

void setup() {
  Serial.begin(115200);

  // Init TFT
  tft.begin();
  tft.setRotation(1);   // Landscape mode (0=portrait, 1=landscape, 2=rev portrait, 3=rev landscape)
  tft.fillScreen(ILI9341_BLACK);

  // Title
  tft.setTextSize(2);
  tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
  tft.setCursor(60, 10);
  tft.println("WATER QUALITY MONITOR");

  // Init Touch
  if (ts.begin()) Serial.println("Touchscreen started");
  else Serial.println("Touchscreen not detected");

  // Init sensors
  pinMode(TdsSensorPin, INPUT);
  sensors.begin();  

  // Draw static layout with icons (landscape placement)
  tft.drawBitmap(20, 60, waterDrop, 16, 16, ILI9341_CYAN);  // TDS
  tft.setCursor(50, 60);
  tft.print("TDS:");

  tft.drawBitmap(20, 110, cloud, 16, 16, ILI9341_YELLOW);   // Turbidity
  tft.setCursor(50, 110);
  tft.print("Turb:");

  tft.drawBitmap(20, 160, thermometer, 16, 16, ILI9341_RED); // Temp
  tft.setCursor(50, 160);
  tft.print("Temp:");
}

void loop() {
  // ----------- TDS Sensor -----------
  int tdsAnalog = analogRead(TdsSensorPin);
  tdsVoltage = tdsAnalog * 3.3 / 4095.0;
  tdsValue = (tdsVoltage * 1000) / 2.3;

  // ----------- Turbidity Sensor -----------
  int turbidityAnalog = analogRead(TurbiditySensorPin);
  turbidityVoltage = turbidityAnalog * (3.3 / 4095.0);
  ntu = -1120.4 * turbidityVoltage * turbidityVoltage + 5742.3 * turbidityVoltage - 4352.9;
  float correctedNTU = ntu - baselineNTU;
  if (correctedNTU < 0) correctedNTU = 0;

  // ----------- DS18B20 Temperature Sensor -----------
  sensors.requestTemperatures();          
  tempC = sensors.getTempCByIndex(0);

  // ----------- Serial Output -----------
  Serial.print("TDS: "); Serial.print(tdsValue, 0); Serial.print(" ppm | ");
  Serial.print("Turbidity: "); Serial.print(correctedNTU, 2); Serial.print(" NTU | ");
  Serial.print("Temperature: "); Serial.print(tempC, 2); Serial.println(" C");

  // ----------- TFT Display Output (update only values) -----------
  tft.fillRect(120, 60, 140, 20, ILI9341_BLACK);
  tft.setCursor(120, 60);
  tft.setTextColor(ILI9341_CYAN, ILI9341_BLACK);
  tft.print(tdsValue, 0);
  tft.print(" ppm");

  tft.fillRect(120, 110, 140, 20, ILI9341_BLACK);
  tft.setCursor(120, 110);
  tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK);
  tft.print(correctedNTU, 1);
  tft.print(" NTU");

  tft.fillRect(120, 160, 140, 20, ILI9341_BLACK);
  tft.setCursor(120, 160);
  tft.setTextColor(ILI9341_RED, ILI9341_BLACK);
  tft.print(tempC, 1);
  tft.print(" C");

  delay(1000);
}
