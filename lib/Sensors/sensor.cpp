#include "sensor.h"

#include <BH1750.h>
#include <DallasTemperature.h>
#include <OneWire.h>
#include <Wire.h>

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
    
    // 光を蓄積するための待機時間
    delay(150); 
    return {true, lightMeter.readLightLevel()};
}

Sensor::V_DS18B20 Sensor::getDS18B20Value(uint8_t pin) {
    OneWire oneWire(pin);
    DallasTemperature ds18b20(&oneWire);
    
    ds18b20.begin();
    // 変換待ち(WaitForConversion)はデフォルトでtrueのため、完了までブロックされる（delay不要）
    ds18b20.requestTemperatures();

    float tempC = ds18b20.getTempCByIndex(0);
    
    // -127.0f: DEVICE_DISCONNECTED_C (未接続エラー)
    //  85.0f: パワーオンリセット直後の初期値（変換完了前の読み出しエラー）
    if (tempC <= -127.0f || tempC == 85.0f) {
        return {false, 0.0f};
    }
    return {true, tempC};
}

Sensor::V_Soil Sensor::calcSoilMoisture(int raw_adc, float cal_dry, float cal_wet) {
    // 浮動小数点での割合計算に変更（map関数の整数丸め誤差を回避）
    float percent = (cal_dry - (float)raw_adc) / (cal_dry - cal_wet) * 100.0f;
    if (percent < 0.0f) percent = 0.0f;
    if (percent > 100.0f) percent = 100.0f;

    if (cal_dry == cal_wet) { // キャリブレーションエラーの回避
        return {false, raw_adc, 0.0f};
    }
    
    return {true, raw_adc, percent};
}

Sensor::V_TDS Sensor::getTDSValue(int raw_adc) {
    return {true, raw_adc};
}

Sensor::V_Battery Sensor::calcBatteryVoltage(int raw_adc, float ref_voltage, float divider_ratio, float AnalogMaxValue) {
    float pin_voltage = (float)raw_adc * (ref_voltage / AnalogMaxValue);
    float battery_voltage = pin_voltage * divider_ratio;
    
    return {true, battery_voltage};
}
