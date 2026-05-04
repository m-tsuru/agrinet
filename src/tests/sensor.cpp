// Sensor library validation sketch

#include <Arduino.h>
#include <Wire.h>
#include "../../lib/Sensors/sensor.h"

#define USER_LED PB5
#define SERIAL_RX PB7
#define SERIAL_TX PB6
#define I2C_SDA PA15
#define I2C_SCL PB15
#define ANALOG_PIN1 PB3
#define ANALOG_PIN2 PB4
#define SERIAL_BAUD 115200

int r1, r2, r3;
float r1_voltage, r2_voltage, r3_voltage;
HardwareSerial Serial0(SERIAL_RX, SERIAL_TX);

static const uint8_t BME280_ADDRESSES[] = {0x76, 0x77};
static const uint8_t BH1750_ADDRESSES[] = {0x23, 0x5C};

void setup() {
    Wire.setSDA(I2C_SDA);
    Wire.setSCL(I2C_SCL);
    Wire.begin();

    Serial0.begin(SERIAL_BAUD);
}

void loop() {
    while (Serial0) {
        delay(5000);
        // 温湿度・気圧計 (BME280)
        Serial0.println("Reading Temperature (BME280) ...");
        for (uint8_t address: BME280_ADDRESSES) {
            Sensor::V_BME280 data = Sensor::getBME280Value(address);
            if (data.success) {
                Serial0.print("BME280 found at 0x");
                Serial0.print(address, HEX);
                Serial0.print(": Temp = ");
                Serial0.print(data.temperature);
                Serial0.print("°C, Hum = ");
                Serial0.print(data.humidity);
                Serial0.print("%, Pres = ");
                Serial0.print(data.pressure);
                Serial0.println("hPa");
                break;
            }
            Serial0.print("No BME280 at 0x");
            Serial0.println(address, HEX);
        }

        // 照度 (BH1750)
        Serial0.println("Reading Brightness (BH1750) ...");
        for (uint8_t address: BH1750_ADDRESSES) {
            Sensor::V_BH1750 data = Sensor::getBH1750Value(address);
            if (data.success) {
                Serial0.print("BH1750 found at 0x");
                Serial0.print(address, HEX);
                Serial0.print(": Brightness = ");
                Serial0.print(data.brightness);
                Serial0.println("lx");
                break;
            }
            Serial0.print("No BH1750 at 0x");
            Serial0.println(address, HEX);
        }

        // --- アナログ入力 1 ---
        Serial0.println("Reading Analog Input 1 ...");
        r1 = analogRead(ANALOG_PIN1);
        r1_voltage = r1 * (3.3 / 1024.0);

        Serial0.print("A1: ");
        Serial0.print(r1);
        Serial0.print(", ");
        Serial0.print(r1_voltage, 2); // 小数点以下2桁まで表示
        Serial0.println(" V");

        // --- アナログ入力 2 ---
        Serial0.println("Reading Analog Input 2 ...");
        r2 = analogRead(ANALOG_PIN2);
        r2_voltage = r2 * (3.3 / 1024.0); // r2 に修正

        Serial0.print("A2: ");
        Serial0.print(r2);
        Serial0.print(", ");
        Serial0.print(r2_voltage, 2);
        Serial0.println(" V");

        // --- アナログ入力 3 ---
        Serial0.println("Reading Analog Input 3 ...");
        r3 = analogRead(PA0);
        r3_voltage = r3 * (3.3 / 1024.0);
        Serial0.print("A3: ");
        Serial0.print(r3);
        Serial0.print(", ");
        Serial0.print(r3_voltage, 2);
        Serial0.println(" V");
    }
}
