#include <Wire.h>
#include <BH1750.h>
#include "sensor.h"
#include <OneWire.h>
#include <DallasTemperature.h>

#include "Adafruit_BME280.h"

std::vector<uint8_t> Sensor::getAllI2CDevice() {
    // Sensor:: を追加
    std::vector<uint8_t> addresses;
    byte error;

    for (byte address = 1; address < 127; address++) {
        Wire.beginTransmission(address);
        error = Wire.endTransmission();
        if (error == 0) {
            addresses.push_back(address);
        }
    }
    return addresses;
}

Sensor::V_BME280 Sensor::getBME280Value(byte address) {
    Adafruit_BME280 bme;

    if (!bme.begin(address)) {
        return {false, 0.0f, 0.0f, 0.0f};
    }

    return {
        true,
        bme.readTemperature(),
        bme.readHumidity(),
        bme.readPressure() / 100.0F // hPaに変換
    };
}

Sensor::V_BH1750 Sensor::getBH1750Value(byte address) {
    BH1750 lightMeter;

    if (!lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE, address)) {
        return {false, 0.0f};
    }
    
    // ★追加：光を蓄積するための待ち時間（これがないとディープスリープ復帰時に0.00になります）
    delay(150); 

    return {true, lightMeter.readLightLevel()};
}

Sensor::V_DS18B20 Sensor::getDS18B20Value(uint8_t pin) {
    OneWire oneWire(pin);
    DallasTemperature ds18b20(&oneWire);
    
    ds18b20.begin();
    ds18b20.requestTemperatures();
    delay(800); // 変換待ち

    float tempC = ds18b20.getTempCByIndex(0);
    if (tempC <= -127.0 || tempC == 85.0) {
        return {false, 0.0f};
    }
    return {true, tempC};
}

Sensor::V_Soil Sensor::getSoilMoisture(uint8_t pin) {
    int raw = analogRead(pin);
    int percent = map(raw, 660, 200, 0, 100);
    percent = constrain(percent, 0, 100);
    return {true, raw, percent};
}

Sensor::V_TDS Sensor::getTDSValue(uint8_t pin) {
    int raw = analogRead(pin);
    return {true, raw};
}

Sensor::V_Battery Sensor::getBatteryValue(uint8_t pin) {
    int raw = analogRead(pin);
    float pinVoltage = 3.3 / 1023.0; 
    float voltage = raw * pinVoltage * 2.0; 
    int percent = map(voltage * 100, 330, 450, 0, 100);
    percent = constrain(percent, 0, 100);
    return {true, voltage, percent};
}