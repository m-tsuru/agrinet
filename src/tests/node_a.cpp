#include <Arduino.h>
#include <Wire.h>
#include <STM32LowPower.h>
#include "../../lib/Sensors/sensor.h"

#define PIN_MOSFET  PC0
#define PIN_DS18B20 PA0
#define PIN_SOIL    PB3
#define PIN_TDS     PB4
#define PIN_BAT     PB14

// センサーのキャリブレーション値
const float SOIL_CAL_DRY = 660.0f;
const float SOIL_CAL_WET = 200.0f;
const float VREF_VOLTAGE = 3.3f;
const float BAT_DIVIDER_RATIO = 2.0f;
const float AnalogMaxValue = 1023.0f; // 10-bit ADC

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
    Serial.println("System Initialized. Ready to sleep.");
}

void loop() {
    Serial.println("\n--- Waking up Node ---");

    // 電源供給開始
    digitalWrite(PIN_MOSFET, HIGH);
    delay(1000); 
    Wire.begin(); 

    // I2C/OneWire Sensors
    Sensor::V_BH1750  lux  = Sensor::getBH1750Value(0x23);
    Sensor::V_DS18B20 temp = Sensor::getDS18B20Value(PIN_DS18B20);

    // Analog Sensors (ADCの読み取りはメイン側で行う)
    int soil_raw = analogRead(PIN_SOIL);
    int tds_raw  = analogRead(PIN_TDS);
    int bat_raw  = analogRead(PIN_BAT);

    Sensor::V_Soil    soil = Sensor::calcSoilMoisture(soil_raw, SOIL_CAL_DRY, SOIL_CAL_WET);
    Sensor::V_TDS     tds  = Sensor::getTDSValue(tds_raw);
    Sensor::V_Battery bat  = Sensor::calcBatteryVoltage(bat_raw, VREF_VOLTAGE, BAT_DIVIDER_RATIO, AnalogMaxValue);

    // ログ出力
    if (lux.success)  { Serial.print("LUX:  "); Serial.println(lux.brightness); }
    if (temp.success) { Serial.print("TEMP: "); Serial.println(temp.temperature); }
    if (soil.success) { 
        Serial.print("SOIL: "); Serial.print(soil.soil_moisture_percent);
        Serial.print(" % (Raw: "); Serial.print(soil.raw_soil); Serial.println(")");
    }
    if (tds.success)  { Serial.print("TDS:  "); Serial.println(tds.raw_tds); }
    if (bat.success)  { Serial.print("BATT: "); Serial.print(bat.voltage_battery, 2); Serial.println(" V"); }

    // 通信の終了処理
    Wire.end(); 

    // 漏れ電流（回り込み電流）防止のためのピン保護
    pinMode(PA15, INPUT_ANALOG);
    pinMode(PB15, INPUT_ANALOG);
    pinMode(PIN_DS18B20, INPUT_ANALOG);
    pinMode(PIN_SOIL, INPUT_ANALOG);
    pinMode(PIN_TDS, INPUT_ANALOG);
    pinMode(PIN_BAT, INPUT_ANALOG);

    // センサー電源遮断
    digitalWrite(PIN_MOSFET, LOW);
    Serial.println("--- Node Sleeping ---");
    
    Serial.flush(); 
    LowPower.deepSleep(10000); 
}
