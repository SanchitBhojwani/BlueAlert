#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <XPT2046_Touchscreen.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFi.h>
#include <HTTPClient.h>

// ================= TFT PINS =================
#define TFT_CS   5
#define TFT_DC   2
#define TFT_RST  4
// ESP32 Hardware SPI pins: MOSI=23, MISO=19, SCK=18

// ================= TOUCH PINS =================
#define TOUCH_CS 15
#define TOUCH_IRQ 27  // optional

// ================= SENSOR PINS =================
#define TdsSensorPin 35
#define TurbiditySensorPin 34
#define ONE_WIRE_BUS 32

int recordId = 1;

// ================= WIFI =================
const char* ssid = "A.T.O.M robotics";
const char* password = "atom281121";
String serverUrl = "http://192.168.0.101:8000/device_data";

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
  0x04,0x00,0x04,0x00,0x07,0x00
};

const unsigned char cloud [] PROGMEM = {
  0x1E,0x00,0x21,0x00,0x40,0x80,0x40,0x80,0x21,0x00,0x1E,0x00,
  0x00,0x00,0x0F,0xC0,0x10,0x20,0x20,0x10,0x20,0x10,0x10,0x20,
  0x0F,0xC0,0x00,0x00
};

void setup() {
  Serial.begin(115200);

  // ---------- TFT Setup ----------
  tft.begin();
  tft.setRotation(1);   // Landscape
  tft.fillScreen(ILI9341_BLACK);

  tft.setTextSize(2);
  tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK); // text=green background_behind_text=black
  tft.setCursor(40, 10);
  tft.println("WATER QUALITY MONITOR");

  // ---------- Touch Setup ----------
  if (ts.begin()) Serial.println("Touchscreen started");
  else Serial.println("Touchscreen not detected");

  // ---------- Sensors ----------
  pinMode(TdsSensorPin, INPUT);
  sensors.begin();  

  // ---------- Draw static layout ----------
  tft.drawBitmap(20, 60, waterDrop, 16, 16, ILI9341_CYAN); // (x,y,---,width,height,colour)
  tft.setCursor(50, 60); tft.print("TDS:");

  tft.drawBitmap(20, 110, cloud, 16, 16, ILI9341_YELLOW);
  tft.setCursor(50, 110); tft.print("Turb:");

  tft.drawBitmap(20, 160, thermometer, 16, 16, ILI9341_RED);
  tft.setCursor(50, 160); tft.print("Temp:");

  // ---------- WiFi ----------
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
}

void loop() {
  // ----------- TDS Sensor -----------
  int tdsAnalog = analogRead(TdsSensorPin);
  tdsVoltage = tdsAnalog * 3.3 / 4095.0;
  tdsValue = (tdsVoltage * 1000) / 2.3; // ppm approx

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
  Serial.print("Turbidity: "); Serial.print(correctedNTU, 1); Serial.print(" NTU | ");
  Serial.print("Temperature: "); Serial.print(tempC, 1); Serial.println(" C");

  // ----------- TFT Display Output -----------
  // TDS
  tft.fillRect(120, 60, 140, 20, ILI9341_BLACK);
  tft.setCursor(120, 60);
  tft.setTextColor(ILI9341_CYAN, ILI9341_BLACK);
  tft.print(tdsValue, 0); tft.print(" ppm");

  // Turbidity
  tft.fillRect(120, 110, 140, 20, ILI9341_BLACK);
  tft.setCursor(120, 110);
  tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK);
  tft.print(correctedNTU, 1); tft.print(" NTU");

  // Temperature
  tft.fillRect(120, 160, 140, 20, ILI9341_BLACK);
  tft.setCursor(120, 160);
  tft.setTextColor(ILI9341_RED, ILI9341_BLACK);
  tft.print(tempC, 1); tft.print(" C");

  // ----------- Send Data to Server -----------
  if (WiFi.status() == WL_CONNECTED) {
  HTTPClient http;
  http.begin(serverUrl);
  http.addHeader("Content-Type", "application/json");

String payload = "{";
payload += "\"device_id\": 1,";
payload += "\"date\": \"2025-09-16\",";   // still hardcoded date
payload += "\"id\": " + String(recordId) + ",";   // auto-increment ID
payload += "\"temp_data\": " + String(tempC,1) + ",";
payload += "\"turbidity_data\": " + String(correctedNTU,1) + ",";
payload += "\"conductivity_data\": " + String(tdsValue,0);
payload += "}";


  Serial.println("Sending: " + payload);
  int httpResponseCode = http.POST(payload);
  if (httpResponseCode > 0) {
    Serial.println("Response code: " + String(httpResponseCode));
    Serial.println("Response: " + http.getString());
  } else {
    Serial.println("Error sending POST: " + String(httpResponseCode));
  }
  http.end();   // <-- prevent future API calls
  }
// } else if (dataSen) {
//   Serial.println("Data already sent, skipping POST...");
// }

  delay(5000); // update every 5 sec
  recordId++;

}
