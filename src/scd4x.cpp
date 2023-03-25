/*
 * Copyright (c) 2021, Sensirion AG
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of Sensirion AG nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "scd4x.h"

uint8_t scd4x_Class::begin(TwoWire& port, uint8_t addr) {
	_i2cPort = &port;
    _address = addr;
    _i2cPort->beginTransmission(_address);
    return _i2cPort->endTransmission();
}

uint8_t scd4x_Class::startPeriodicMeasurement() {
	_i2cPort->beginTransmission(_address);
    _i2cPort->write(0x21);
    _i2cPort->write(0xb1);
    return _i2cPort->endTransmission();
}

// uint16_t SensirionI2CScd4x::readMeasurementTicks(uint16_t& co2,
//                                                  uint16_t& temperature,
//                                                  uint16_t& humidity) {
//     uint16_t error;
//     uint8_t buffer[9];
//     SensirionI2CTxFrame txFrame(buffer, 9);

//     error = txFrame.addCommand(0xEC05);
//     if (error) {
//         return error;
//     }

//     error = SensirionI2CCommunication::sendFrame(SCD4X_I2C_ADDRESS, txFrame,
//                                                  *_i2cBus);
//     if (error) {
//         return error;
//     }

//     delay(1);

//     SensirionI2CRxFrame rxFrame(buffer, 9);
//     error = SensirionI2CCommunication::receiveFrame(SCD4X_I2C_ADDRESS, 9,
//                                                     rxFrame, *_i2cBus);
//     if (error) {
//         return error;
//     }

//     error |= rxFrame.getUInt16(co2);
//     error |= rxFrame.getUInt16(temperature);
//     error |= rxFrame.getUInt16(humidity);
//     return error;
// }

uint8_t scd4x_Class::readMeasurement(double& co2, double& temperature, double& humidity) {
	const int bytesRequested = 12;

    _i2cPort->beginTransmission(_address);
    _i2cPort->write(0xec);
    _i2cPort->write(0x05);

    if (_i2cPort->endTransmission(true) == 0) {
        // read measurement data: 2 bytes co2, 1 byte CRC,
        // 2 bytes T, 1 byte CRC, 2 bytes RH, 1 byte CRC,
        // 2 bytes sensor status, 1 byte CRC
        // stop reading after bytesRequested (12 bytes)

        uint8_t bytesReceived = Wire.requestFrom(_address, bytesRequested);
        if (bytesReceived == bytesRequested) {  // If received requested amount of bytes
            uint8_t data[bytesReceived];
            Wire.readBytes(data, bytesReceived);

            // floating point conversion
            co2 = (double)((uint16_t)data[0] << 8 | data[1]);
            // convert T in degC
            temperature = -45 + 175 * (double)((uint16_t)data[3] << 8 | data[4]) / 65536;
            // convert RH in %
            humidity = 100 * (double)((uint16_t)data[6] << 8 | data[7]) / 65536;

            if(inRange(co2, 40000, 0) && inRange(temperature, 60, -10) &&
                   inRange(humidity, 100, 0)){
                    return false;
            } else {
                ESP_LOGE("measurement", "out of range");
                return true;
            }

        } else {
            ESP_LOGE("Wire.requestFrom","bytesReceived(%i) != bytesRequested(%i)",
                     bytesReceived, bytesRequested);
            return true;
        }
    } else {
        ESP_LOGE("Wire.endTransmission", "Returned ERROR");
        return true;
    }
}

// uint16_t SensirionI2CScd4x::stopPeriodicMeasurement() {
//     uint16_t error;
//     uint8_t buffer[2];
//     SensirionI2CTxFrame txFrame(buffer, 2);

//     error = txFrame.addCommand(0x3F86);
//     if (error) {
//         return error;
//     }
//     error = SensirionI2CCommunication::sendFrame(SCD4X_I2C_ADDRESS, txFrame,
//                                                  *_i2cBus);
//     delay(500);
//     return error;
// }

// uint16_t SensirionI2CScd4x::getTemperatureOffsetTicks(uint16_t& tOffset) {
//     uint16_t error;
//     uint8_t buffer[3];
//     SensirionI2CTxFrame txFrame(buffer, 3);

//     error = txFrame.addCommand(0x2318);
//     if (error) {
//         return error;
//     }

//     error = SensirionI2CCommunication::sendFrame(SCD4X_I2C_ADDRESS, txFrame,
//                                                  *_i2cBus);
//     if (error) {
//         return error;
//     }

//     delay(1);

//     SensirionI2CRxFrame rxFrame(buffer, 3);
//     error = SensirionI2CCommunication::receiveFrame(SCD4X_I2C_ADDRESS, 3,
//                                                     rxFrame, *_i2cBus);
//     if (error) {
//         return error;
//     }

//     error |= rxFrame.getUInt16(tOffset);
//     return error;
// }

// uint16_t SensirionI2CScd4x::getTemperatureOffset(float& tOffset) {
//     uint16_t error;
//     uint16_t tOffsetTicks;

//     error = getTemperatureOffsetTicks(tOffsetTicks);
//     if (error) {
//         return error;
//     }

//     tOffset = static_cast<float>(tOffsetTicks * 175.0 / 65536.0);
//     return NoError;
// }

// uint16_t SensirionI2CScd4x::setTemperatureOffsetTicks(uint16_t tOffset) {
//     uint16_t error;
//     uint8_t buffer[5];
//     SensirionI2CTxFrame txFrame(buffer, 5);

//     error = txFrame.addCommand(0x241D);
//     error |= txFrame.addUInt16(tOffset);
//     if (error) {
//         return error;
//     }

//     error = SensirionI2CCommunication::sendFrame(SCD4X_I2C_ADDRESS, txFrame,
//                                                  *_i2cBus);
//     delay(1);
//     return error;
// }

// uint16_t SensirionI2CScd4x::setTemperatureOffset(float tOffset) {
//     uint16_t tOffsetTicks =
//         static_cast<uint16_t>(tOffset * 65536.0 / 175.0 + 0.5f);
//     return setTemperatureOffsetTicks(tOffsetTicks);
// }

// uint16_t SensirionI2CScd4x::getSensorAltitude(uint16_t& sensorAltitude) {
//     uint16_t error;
//     uint8_t buffer[3];
//     SensirionI2CTxFrame txFrame(buffer, 3);

//     error = txFrame.addCommand(0x2322);
//     if (error) {
//         return error;
//     }

//     error = SensirionI2CCommunication::sendFrame(SCD4X_I2C_ADDRESS, txFrame,
//                                                  *_i2cBus);
//     if (error) {
//         return error;
//     }

//     delay(1);

//     SensirionI2CRxFrame rxFrame(buffer, 3);
//     error = SensirionI2CCommunication::receiveFrame(SCD4X_I2C_ADDRESS, 3,
//                                                     rxFrame, *_i2cBus);
//     if (error) {
//         return error;
//     }

//     error |= rxFrame.getUInt16(sensorAltitude);
//     return error;
// }

// uint16_t SensirionI2CScd4x::setSensorAltitude(uint16_t sensorAltitude) {
//     uint16_t error;
//     uint8_t buffer[5];
//     SensirionI2CTxFrame txFrame(buffer, 5);

//     error = txFrame.addCommand(0x2427);
//     error |= txFrame.addUInt16(sensorAltitude);
//     if (error) {
//         return error;
//     }

//     error = SensirionI2CCommunication::sendFrame(SCD4X_I2C_ADDRESS, txFrame,
//                                                  *_i2cBus);
//     delay(1);
//     return error;
// }

// uint16_t SensirionI2CScd4x::setAmbientPressure(uint16_t ambientPressure) {
//     uint16_t error;
//     uint8_t buffer[5];
//     SensirionI2CTxFrame txFrame(buffer, 5);

//     error = txFrame.addCommand(0xE000);
//     error |= txFrame.addUInt16(ambientPressure);
//     if (error) {
//         return error;
//     }

//     error = SensirionI2CCommunication::sendFrame(SCD4X_I2C_ADDRESS, txFrame,
//                                                  *_i2cBus);
//     delay(1);
//     return error;
// }

// uint16_t
// SensirionI2CScd4x::performForcedRecalibration(uint16_t targetCo2Concentration,
//                                               uint16_t& frcCorrection) {
//     uint16_t error;
//     uint8_t buffer[5];
//     SensirionI2CTxFrame txFrame(buffer, 5);

//     error = txFrame.addCommand(0x362F);
//     error |= txFrame.addUInt16(targetCo2Concentration);
//     if (error) {
//         return error;
//     }

//     error = SensirionI2CCommunication::sendFrame(SCD4X_I2C_ADDRESS, txFrame,
//                                                  *_i2cBus);
//     if (error) {
//         return error;
//     }

//     delay(400);

//     SensirionI2CRxFrame rxFrame(buffer, 5);
//     error = SensirionI2CCommunication::receiveFrame(SCD4X_I2C_ADDRESS, 3,
//                                                     rxFrame, *_i2cBus);
//     if (error) {
//         return error;
//     }

//     error |= rxFrame.getUInt16(frcCorrection);
//     return error;
// }

// uint16_t SensirionI2CScd4x::getAutomaticSelfCalibration(uint16_t& ascEnabled) {
//     uint16_t error;
//     uint8_t buffer[3];
//     SensirionI2CTxFrame txFrame(buffer, 3);

//     error = txFrame.addCommand(0x2313);
//     if (error) {
//         return error;
//     }

//     error = SensirionI2CCommunication::sendFrame(SCD4X_I2C_ADDRESS, txFrame,
//                                                  *_i2cBus);
//     if (error) {
//         return error;
//     }

//     delay(1);

//     SensirionI2CRxFrame rxFrame(buffer, 3);
//     error = SensirionI2CCommunication::receiveFrame(SCD4X_I2C_ADDRESS, 3,
//                                                     rxFrame, *_i2cBus);
//     if (error) {
//         return error;
//     }

//     error |= rxFrame.getUInt16(ascEnabled);
//     return error;
// }

// uint16_t SensirionI2CScd4x::setAutomaticSelfCalibration(uint16_t ascEnabled) {
//     uint16_t error;
//     uint8_t buffer[5];
//     SensirionI2CTxFrame txFrame(buffer, 5);

//     error = txFrame.addCommand(0x2416);
//     error |= txFrame.addUInt16(ascEnabled);
//     if (error) {
//         return error;
//     }

//     error = SensirionI2CCommunication::sendFrame(SCD4X_I2C_ADDRESS, txFrame,
//                                                  *_i2cBus);
//     delay(1);
//     return error;
// }

// uint16_t SensirionI2CScd4x::getAutomaticSelfCalibrationInitialPeriod(
//     uint16_t& ascInitialPeriod) {
//     uint16_t error;
//     uint8_t buffer[3];
//     SensirionI2CTxFrame txFrame(buffer, 3);

//     error = txFrame.addCommand(0x2340);
//     if (error) {
//         return error;
//     }

//     error = SensirionI2CCommunication::sendFrame(SCD4X_I2C_ADDRESS, txFrame,
//                                                  *_i2cBus);
//     if (error) {
//         return error;
//     }

//     delay(1);

//     SensirionI2CRxFrame rxFrame(buffer, 3);
//     error = SensirionI2CCommunication::receiveFrame(SCD4X_I2C_ADDRESS, 3,
//                                                     rxFrame, *_i2cBus);
//     if (error) {
//         return error;
//     }

//     error |= rxFrame.getUInt16(ascInitialPeriod);
//     return error;
// }

// uint16_t SensirionI2CScd4x::getAutomaticSelfCalibrationStandardPeriod(
//     uint16_t& ascStandardPeriod) {
//     uint16_t error;
//     uint8_t buffer[3];
//     SensirionI2CTxFrame txFrame(buffer, 3);

//     error = txFrame.addCommand(0x234B);
//     if (error) {
//         return error;
//     }

//     error = SensirionI2CCommunication::sendFrame(SCD4X_I2C_ADDRESS, txFrame,
//                                                  *_i2cBus);
//     if (error) {
//         return error;
//     }

//     delay(1);

//     SensirionI2CRxFrame rxFrame(buffer, 3);
//     error = SensirionI2CCommunication::receiveFrame(SCD4X_I2C_ADDRESS, 3,
//                                                     rxFrame, *_i2cBus);
//     if (error) {
//         return error;
//     }

//     error |= rxFrame.getUInt16(ascStandardPeriod);
//     return error;
// }

// uint16_t SensirionI2CScd4x::setAutomaticSelfCalibrationInitialPeriod(
//     uint16_t& ascInitialPeriod) {
//     uint16_t error;
//     uint8_t buffer[5];
//     SensirionI2CTxFrame txFrame(buffer, 5);

//     error = txFrame.addCommand(0x2445);
//     error |= txFrame.addUInt16(ascInitialPeriod);
//     if (error) {
//         return error;
//     }

//     error = SensirionI2CCommunication::sendFrame(SCD4X_I2C_ADDRESS, txFrame,
//                                                  *_i2cBus);
//     delay(1);
//     return error;
// }

// uint16_t SensirionI2CScd4x::setAutomaticSelfCalibrationStandardPeriod(
//     uint16_t& ascStandardPeriod) {
//     uint16_t error;
//     uint8_t buffer[5];
//     SensirionI2CTxFrame txFrame(buffer, 5);

//     error = txFrame.addCommand(0x244E);
//     error |= txFrame.addUInt16(ascStandardPeriod);
//     if (error) {
//         return error;
//     }

//     error = SensirionI2CCommunication::sendFrame(SCD4X_I2C_ADDRESS, txFrame,
//                                                  *_i2cBus);
//     delay(1);
//     return error;
// }

// uint16_t SensirionI2CScd4x::startLowPowerPeriodicMeasurement() {
//     uint16_t error;
//     uint8_t buffer[2];
//     SensirionI2CTxFrame txFrame(buffer, 2);

//     error = txFrame.addCommand(0x21AC);
//     if (error) {
//         return error;
//     }

//     return SensirionI2CCommunication::sendFrame(SCD4X_I2C_ADDRESS, txFrame,
//                                                 *_i2cBus);
// }

bool scd4x_Class::isDataReady() {
	const int bytesRequested = 3;

    _i2cPort->beginTransmission(_address);
    _i2cPort->write(0xe4);
    _i2cPort->write(0xb8);

    if (_i2cPort->endTransmission(true) == 0) {

        uint8_t bytesReceived = Wire.requestFrom(_address, bytesRequested);
        if (bytesReceived == bytesRequested) {  // If received requested amount of bytes
            uint8_t data[bytesReceived];
            Wire.readBytes(data, bytesReceived);
            return data[1] != (0x00); //check if last 8 bits are not 0

        } else {
            ESP_LOGE("Wire.requestFrom",
                     "bytesReceived(%i) != bytesRequested(%i)", bytesReceived,
                     bytesRequested);
            return false;
        }
    } else {
        ESP_LOGE("Wire.endTransmission", "Returned ERROR");
        return false;
    }
}

// uint16_t SensirionI2CScd4x::persistSettings() {
//     uint16_t error;
//     uint8_t buffer[2];
//     SensirionI2CTxFrame txFrame(buffer, 2);

//     error = txFrame.addCommand(0x3615);
//     if (error) {
//         return error;
//     }

//     error = SensirionI2CCommunication::sendFrame(SCD4X_I2C_ADDRESS, txFrame,
//                                                  *_i2cBus);
//     delay(800);
//     return error;
// }

// uint16_t SensirionI2CScd4x::getSerialNumber(uint16_t& serial0,
//                                             uint16_t& serial1,
//                                             uint16_t& serial2) {
//     uint16_t error;
//     uint8_t buffer[9];
//     SensirionI2CTxFrame txFrame(buffer, 9);

//     error = txFrame.addCommand(0x3682);
//     if (error) {
//         return error;
//     }

//     error = SensirionI2CCommunication::sendFrame(SCD4X_I2C_ADDRESS, txFrame,
//                                                  *_i2cBus);
//     if (error) {
//         return error;
//     }

//     delay(1);

//     SensirionI2CRxFrame rxFrame(buffer, 9);
//     error = SensirionI2CCommunication::receiveFrame(SCD4X_I2C_ADDRESS, 9,
//                                                     rxFrame, *_i2cBus);
//     if (error) {
//         return error;
//     }

//     error |= rxFrame.getUInt16(serial0);
//     error |= rxFrame.getUInt16(serial1);
//     error |= rxFrame.getUInt16(serial2);
//     return error;
// }

// uint16_t SensirionI2CScd4x::performSelfTest(uint16_t& sensorStatus) {
//     uint16_t error;
//     uint8_t buffer[3];
//     SensirionI2CTxFrame txFrame(buffer, 3);

//     error = txFrame.addCommand(0x3639);
//     if (error) {
//         return error;
//     }

//     error = SensirionI2CCommunication::sendFrame(SCD4X_I2C_ADDRESS, txFrame,
//                                                  *_i2cBus);
//     if (error) {
//         return error;
//     }

//     delay(10000);

//     SensirionI2CRxFrame rxFrame(buffer, 3);
//     error = SensirionI2CCommunication::receiveFrame(SCD4X_I2C_ADDRESS, 3,
//                                                     rxFrame, *_i2cBus);
//     if (error) {
//         return error;
//     }

//     error |= rxFrame.getUInt16(sensorStatus);
//     return error;
// }

// uint16_t SensirionI2CScd4x::performFactoryReset() {
//     uint16_t error;
//     uint8_t buffer[2];
//     SensirionI2CTxFrame txFrame(buffer, 2);

//     error = txFrame.addCommand(0x3632);
//     if (error) {
//         return error;
//     }

//     error = SensirionI2CCommunication::sendFrame(SCD4X_I2C_ADDRESS, txFrame,
//                                                  *_i2cBus);
//     delay(800);
//     return error;
// }

// uint16_t SensirionI2CScd4x::reinit() {
//     uint16_t error;
//     uint8_t buffer[2];
//     SensirionI2CTxFrame txFrame(buffer, 2);

//     error = txFrame.addCommand(0x3646);
//     if (error) {
//         return error;
//     }

//     error = SensirionI2CCommunication::sendFrame(SCD4X_I2C_ADDRESS, txFrame,
//                                                  *_i2cBus);
//     delay(20);
//     return error;
// }

// uint16_t SensirionI2CScd4x::measureSingleShot() {
//     uint16_t error;
//     uint8_t buffer[2];
//     SensirionI2CTxFrame txFrame(buffer, 2);

//     error = txFrame.addCommand(0x219D);
//     if (error) {
//         return error;
//     }

//     error = SensirionI2CCommunication::sendFrame(SCD4X_I2C_ADDRESS, txFrame,
//                                                  *_i2cBus);
//     delay(5000);
//     return error;
// }

// uint16_t SensirionI2CScd4x::measureSingleShotRhtOnly() {
//     uint16_t error;
//     uint8_t buffer[2];
//     SensirionI2CTxFrame txFrame(buffer, 2);

//     error = txFrame.addCommand(0x2196);
//     if (error) {
//         return error;
//     }

//     error = SensirionI2CCommunication::sendFrame(SCD4X_I2C_ADDRESS, txFrame,
//                                                  *_i2cBus);
//     delay(50);
//     return error;
// }

// uint16_t SensirionI2CScd4x::powerDown() {
//     uint16_t error;
//     uint8_t buffer[2];
//     SensirionI2CTxFrame txFrame(buffer, 2);

//     error = txFrame.addCommand(0x36E0);
//     if (error) {
//         return error;
//     }

//     error = SensirionI2CCommunication::sendFrame(SCD4X_I2C_ADDRESS, txFrame,
//                                                  *_i2cBus);
//     delay(1);
//     return error;
// }

// uint16_t SensirionI2CScd4x::wakeUp() {
//     uint16_t error;
//     uint8_t buffer[2];
//     SensirionI2CTxFrame txFrame(buffer, 2);

//     error = txFrame.addCommand(0x36F6);
//     if (error) {
//         return error;
//     }

//     // Sensor does not acknowledge the wake-up call, error is ignored
//     static_cast<void>(SensirionI2CCommunication::sendFrame(SCD4X_I2C_ADDRESS,
//                                                            txFrame, *_i2cBus));
//     delay(20);
//     return NoError;
// }


