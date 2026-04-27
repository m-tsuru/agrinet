#include <Wire.h>
#include <BH1750.h>
#include "sensor.h"

#include "Adafruit_BME280.h"

std::vector<uint8_t> Sensor::getAllI2CDevice() {  // Sensor:: を追加
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

    return {true, lightMeter.readLightLevel()};
}





