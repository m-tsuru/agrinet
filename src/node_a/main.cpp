// 2025-05-04 agrinet-programs (Node A Main)

#include <Arduino.h>
#include <Wire.h>
#include <STM32LowPower.h>
#include "../../lib/Sensors/sensor.h" // チームのライブラリを読み込む

// ピン定義
#define PIN_MOSFET  PC0
#define PIN_DS18B20 PA0
#define PIN_SOIL    PB3
#define PIN_TDS     PB4
#define PIN_BAT     PB14

void setup() {
  Serial.setTx(PB6);
  Serial.setRx(PB7);
  Serial.begin(115200);
  delay(2000); 

  Wire.setSDA(PA15);
  Wire.setSCL(PB15);

  pinMode(PIN_MOSFET, OUTPUT);
  digitalWrite(PIN_MOSFET, LOW);

  LowPower.begin();
  Serial.println("System Initialized! Ready to deep sleep.");
}

void loop() {
  Serial.println("\n--- Waking up Node A ---");

  // 1. センサー電源ON
  digitalWrite(PIN_MOSFET, HIGH);
  delay(1000); 
  Wire.begin(); 

  // 2. データの取得（ライブラリに丸投げ！）
  Sensor::V_BH1750  lux  = Sensor::getBH1750Value(0x23);
  Sensor::V_DS18B20 temp = Sensor::getDS18B20Value(PIN_DS18B20);
  Sensor::V_Soil    soil = Sensor::getSoilMoisture(PIN_SOIL);
  Sensor::V_TDS     tds  = Sensor::getTDSValue(PIN_TDS);
  Sensor::V_Battery bat  = Sensor::getBatteryValue(PIN_BAT);

  // 3. データ表示
  if (lux.success)  { Serial.print("LUX:  "); Serial.println(lux.brightness); }
  if (temp.success) { Serial.print("TEMP: "); Serial.println(temp.temperature); }
  if (soil.success) { Serial.print("SOIL: "); Serial.print(soil.percent); Serial.println(" % (Raw: " + String(soil.raw) + ")"); }
  if (tds.success)  { Serial.print("TDS:  "); Serial.println(tds.raw); }
  if (bat.success)  { 
      Serial.print("BATTERY: "); Serial.print(bat.voltage, 2); Serial.println(" V"); 
      Serial.print("BATTERY: "); Serial.print(bat.percent); Serial.println(" %"); 
  }

  // 4. 終了処理とピン解放
  Wire.end(); 
  pinMode(PA15, INPUT);
  pinMode(PB15, INPUT);

  // 5. センサー電源OFF
  digitalWrite(PIN_MOSFET, LOW);
  Serial.println("--- Node A Sleeping ---");

  Serial.flush(); 
  LowPower.deepSleep(10000); 
}