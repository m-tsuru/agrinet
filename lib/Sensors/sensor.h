#ifndef AGRINET_SENSOR_H
#define AGRINET_SENSOR_H

#include <vector>
#include <Arduino.h>

class Sensor {
public:
    struct V_BME280 {
        bool success;
        float temperature;
        float humidity;
        float pressure;
    };

    struct V_BH1750 {
        bool success;
        float brightness;
    };
    struct V_DS18B20 {
        bool success;
        float temperature;
    };

    struct V_Soil {
        bool success;
        int raw_soil;
        float soil_moisture_percent;
    };

    struct V_TDS {
        bool success;
        int raw_tds;
    };

    struct V_Battery {
        bool success;
        float voltage_battery;
    };

    static std::vector<uint8_t> getAllI2CDevice();

    static V_BME280 getBME280Value(byte address);

    static V_BH1750 getBH1750Value(byte address);

    static V_DS18B20 getDS18B20Value(uint8_t pin);

    // 12bitコンバータ実装時のために計算をする関数に変更
    static V_Soil calcSoilMoisture(int raw_adc, float cal_dry, float cal_wet);

    static V_TDS calcTDSValue(int raw_adc);

    static V_Battery calcBatteryVoltage(int raw_adc, float ref_voltage, float divider_ratio);

};

#endif //AGRINET_SENSOR_H
