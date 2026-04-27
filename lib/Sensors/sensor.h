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

        static std::vector<uint8_t> getAllI2CDevice();
        static V_BME280 getBME280Value(byte address);
        static V_BH1750 getBH1750Value(byte address);
};

#endif //AGRINET_SENSOR_H