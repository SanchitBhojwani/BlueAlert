#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <XPT2046_Touchscreen.h>

// ================= TFT PINS =================
#define TFT_CS   5
#define TFT_DC   2
#define TFT_RST  4
#define TFT_MOSI 23
#define TFT_CLK  18
#define TFT_MISO 19

// ================= TOUCH PINS =================
#define TOUCH_CS 15
#define TOUCH_IRQ 27  // optional (can leave unconnected)

// TFT object
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

// Touch object
XPT2046_Touchscreen ts(TOUCH_CS, TOUCH_IRQ);

void setup() {
  Serial.begin(115200);

  // Init TFT
  tft.begin();
  tft.fillScreen(ILI9341_BLACK);
  tft.setCursor(20, 20);
  tft.setTextColor(ILI9341_GREEN);
  tft.setTextSize(2);
  tft.println("ESP32 + 2.8\" TFT");
  delay(1000);

  // Draw shapes
  tft.fillCircle(160, 120, 40, ILI9341_RED);
  tft.drawRect(10, 200, 100, 40, ILI9341_YELLOW);

  // Init Touch
  if (ts.begin()) {
    Serial.println("Touchscreen started");
  } else {
    Serial.println("Touchscreen not detected");
  }
}

void loop() {
  // Check touch
  if (ts.touched()) {
    TS_Point p = ts.getPoint();
    Serial.print("Touch at X=");
    Serial.print(p.x);
    Serial.print("  Y=");
    Serial.println(p.y);

    // Simple visual feedback
    tft.fillCircle(map(p.x, 200, 3900, 0, 240),  // adjust calibration
                   map(p.y, 200, 3900, 0, 320), 
                   3, ILI9341_BLUE);
  }
  delay(50);
}
