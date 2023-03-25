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
#pragma once

#include <Arduino.h>
#include <Wire.h>

#define SCD4X_I2C_ADDRESS (0x62)  // 7-bit I2C Address

class scd4x_Class {
   public:
    /**
     * begin() - Initializes the SensirionI2CScd4x class.
     *
     * @param i2cBus Arduino stream object to use for communication.
     *
     */
    uint8_t begin(TwoWire& port = Wire, uint8_t addr = SCD4X_I2C_ADDRESS);

    /**
     * startPeriodicMeasurement() - start periodic measurement, signal update
     * interval is 5 seconds.
     *
     * @note This command is only available in idle mode.
     *
     * @return 0 on success, an error code otherwise
     */
    uint8_t startPeriodicMeasurement();

    /**
     * readMeasurementTicks() - read sensor output. The measurement data can
     * only be read out once per signal update interval as the buffer is emptied
     * upon read-out. If no data is available in the buffer, the sensor returns
     * a NACK. To avoid a NACK response the get_data_ready_status can be issued
     * to check data status. The I2C master can abort the read transfer with a
     * NACK followed by a STOP condition after any data byte if the user is not
     * interested in subsequent data.
     *
     * @note This command is only available in measurement mode. The firmware
     * updates the measurement values depending on the measurement mode.
     *
     * @param co2 CO₂ concentration in ppm
     *
     * @param temperature Convert value to °C by: -45 °C + 175 °C * value/2^16
     *
     * @param humidity Convert value to %RH by: 100%RH * value/2^16
     *
     * @return 0 on success, an error code otherwise
     */
    // uint16_t readMeasurementTicks(uint16_t& co2, uint16_t& temperature,
    //                               uint16_t& humidity);

    /**
     * readMeasurement() - read sensor output. The measurement data can
     * only be read out once per signal update interval as the buffer is emptied
     * upon read-out. If no data is available in the buffer, the sensor returns
     * a NACK. To avoid a NACK response the get_data_ready_status can be issued
     * to check data status. The I2C master can abort the read transfer with a
     * NACK followed by a STOP condition after any data byte if the user is not
     * interested in subsequent data.
     *
     * @note This command is only available in measurement mode. The firmware
     * updates the measurement values depending on the measurement mode.
     *
     * @param co2 CO₂ concentration in ppm
     *
     * @param temperature Temperature in °C
     *
     * @param humidity Relative humidity in %RH
     *
     * @return 0 on success, an error code otherwise
     */
    uint8_t readMeasurement(double& co2, double& temperature,
                                   double& humidity);

    /**
     * stopPeriodicMeasurement() - Stop periodic measurement and return to idle
     * mode for sensor configuration or to safe energy.
     *
     * @note This command is only available in measurement mode.
     *
     * @return 0 on success, an error code otherwise
     */
    //uint16_t stopPeriodicMeasurement(void);

    /**
     * getTemperatureOffsetTicks() - The temperature offset represents the
     * difference between the measured temperature by the SCD4x and the actual
     * ambient temperature. Per default, the temperature offset is set to 4°C.
     *
     * @note Only available in idle mode.
     *
     * @param tOffset Temperature offset. Convert value to °C by: 175 * value /
     * 2^16
     *
     * @return 0 on success, an error code otherwise
     */
    // uint16_t getTemperatureOffsetTicks(uint16_t& tOffset);

    /**
     * getTemperatureOffset() - The temperature offset represents the difference
     * between the measured temperature by the SCD4x and the actual ambient
     * temperature. Per default, the temperature offset is set to 4°C.
     *
     * @note Only available in idle mode.
     *
     * @param tOffset Temperature offset in °C
     *
     * @return 0 on success, an error code otherwise
     */
    // uint16_t getTemperatureOffset(float& tOffset);

    /**
     * setTemperatureOffsetTicks() - Setting the temperature offset of the SCD4x
     * inside the customer device correctly allows the user to leverage the RH
     * and T output signal. Note that the temperature offset can depend on
     * various factors such as the SCD4x measurement mode, self-heating of close
     * components, the ambient temperature and air flow. Thus, the SCD4x
     * temperature offset should be determined inside the customer device under
     * its typical operation and in thermal equilibrium.
     *
     * @note Only available in idle mode.
     *
     * @param tOffset Temperature offset. Convert °C to value by: T * 2^16 /
     * 175.
     *
     * @return 0 on success, an error code otherwise
     */
    // uint16_t setTemperatureOffsetTicks(uint16_t tOffset);

    /**
     * setTemperatureOffset() - Setting the temperature offset of the SCD4x
     * inside the customer device correctly allows the user to leverage the RH
     * and T output signal. Note that the temperature offset can depend on
     * various factors such as the SCD4x measurement mode, self-heating of close
     * components, the ambient temperature and air flow. Thus, the SCD4x
     * temperature offset should be determined inside the customer device under
     * its typical operation and in thermal equilibrium.
     *
     * @note Only available in idle mode.
     *
     * @param tOffset Temperature offset in °C
     *
     * @return 0 on success, an error code otherwise
     */
    // uint16_t setTemperatureOffset(float tOffset);

    /**
     * getSensorAltitude() - Get configured sensor altitude in meters above sea
     * level. Per default, the sensor altitude is set to 0 meter above
     * sea-level.
     *
     * @note Only available in idle mode.
     *
     * @param sensorAltitude Sensor altitude in meters.
     *
     * @return 0 on success, an error code otherwise
     */
    // uint16_t getSensorAltitude(uint16_t& sensorAltitude);

    /**
     * setSensorAltitude() - Set sensor altitude in meters above sea level. Note
     * that setting a sensor altitude to the sensor overrides any pressure
     * compensation based on a previously set ambient pressure.
     *
     * @note Only available in idle mode.
     *
     * @param sensorAltitude Sensor altitude in meters.
     *
     * @return 0 on success, an error code otherwise
     */
    // uint16_t setSensorAltitude(uint16_t sensorAltitude);

    /**
     * setAmbientPressure() - The set_ambient_pressure command can be sent
     * during periodic measurements to enable continuous pressure compensation.
     * Note that setting an ambient pressure to the sensor overrides any
     * pressure compensation based on a previously set sensor altitude.
     *
     * @note Available during measurements.
     *
     * @param ambientPressure Ambient pressure in hPa. Convert value to Pa by:
     * value * 100.
     *
     * @return 0 on success, an error code otherwise
     */
    // uint16_t setAmbientPressure(uint16_t ambientPressure);

    /**
     * performForcedRecalibration() - To successfully conduct an accurate forced
    recalibration, the following steps need to be carried out:
    1. Operate the SCD4x in a periodic measurement mode for > 3 minutes in an
    environment with homogenous and constant CO₂ concentration.
    2. Stop periodic measurement. Wait 500 ms.
    3. Subsequently issue the perform_forced_recalibration command and
    optionally read out the baseline correction. A return value of 0xffff
    indicates that the forced recalibration failed.
     *
     * @param targetCo2Concentration Target CO₂ concentration in ppm.
     *
     * @param frcCorrection FRC correction value in CO₂ ppm or 0xFFFF if the
    command failed. Convert value to CO₂ ppm with: value - 0x8000
     *
     * @return 0 on success, an error code otherwise
     */
    // uint16_t performForcedRecalibration(uint16_t targetCo2Concentration,
    //                                     uint16_t& frcCorrection);

    /**
     * getAutomaticSelfCalibration() - By default, the ASC is enabled.
     *
     * @param ascEnabled 1 if ASC is enabled, 0 if ASC is disabled
     *
     * @return 0 on success, an error code otherwise
     */
    // uint16_t getAutomaticSelfCalibration(uint16_t& ascEnabled);

    /**
     * setAutomaticSelfCalibration() - By default, the ASC is enabled.
     *
     * @param ascEnabled 1 to enable ASC, 0 to disable ASC
     *
     * @return 0 on success, an error code otherwise
     */
    // uint16_t setAutomaticSelfCalibration(uint16_t ascEnabled);

    /**
     * getAutomaticSelfCalibrationInitialPeriod - value in hours
     *
     * @param ascInitialPeriod number of hours before ASC is first triggered
     *
     * @return 0 on success, an error code otherwise
     */
    //uint16_t
    // getAutomaticSelfCalibrationInitialPeriod(uint16_t& ascInitialPeriod);

    /**
     * getAutomaticSelfCalibrationStandardPeriod - value in hours
     *
     * @param ascStandardPeriod number of hours to regularly trigger ASC
     *
     * @return 0 on success, an error code otherwise
     */
    // uint16_t
    // getAutomaticSelfCalibrationStandardPeriod(uint16_t& ascStandardPeriod);

    /**
     * setAutomaticSelfCalibrationInitialPeriod - value in hours
     *
     * @param ascInitialPeriod number of hours before ASC is first triggered
     *
     * @return 0 on success, an error code otherwise
     */
    // uint16_t
    // setAutomaticSelfCalibrationInitialPeriod(uint16_t& ascInitialPeriod);

    /**
     * setAutomaticSelfCalibrationStandardPeriod - value in hours
     *
     * @param ascStandardPeriod number of hours to regularly trigger ASC
     *
     * @return 0 on success, an error code otherwise
     */
    // uint16_t
    // setAutomaticSelfCalibrationStandardPeriod(uint16_t& ascStandardPeriod);

    /**
     * startLowPowerPeriodicMeasurement() - Start low power periodic
     * measurement, signal update interval is 30 seconds.
     *
     * @note This command is only available in idle mode.
     *
     * @return 0 on success, an error code otherwise
     */
    // uint16_t startLowPowerPeriodicMeasurement(void);

    /**
     * getDataReadyFlag() - Check whether new measurement data is available
     * for read-out.
     *
     * @param dataReadyFlag True if valid data is available, false otherwise.
     *
     * @return 0 on success, an error code otherwise
     */
    bool isDataReady();

    /**
     * persistSettings() - Configuration settings such as the temperature
     * offset, sensor altitude and the ASC enabled/disabled parameter are by
     * default stored in the volatile memory (RAM) only and will be lost after a
     * power-cycle. This command stores the current configuration in the EEPROM
     * of the SCD4x, making them persistent across power-cycling.
     *
     * @note To avoid unnecessary wear of the EEPROM, this command should only
     * be sent when persistence is required and if actual changes to the
     * configuration have been made (The EEPROM is guaranteed to endure at least
     * 2000 write cycles before failure). Note that field calibration history
     * (i.e. FRC and ASC) is automatically stored in a separate EEPROM
     * dimensioned for specified sensor lifetime.
     *
     * @return 0 on success, an error code otherwise
     */
    // uint16_t persistSettings(void);

    /**
     * getSerialNumber() - Reading out the serial number can be used to identify
     * the chip and to verify the presence of the sensor. The get serial number
     * command returns 3 words.  Together, the 3 words constitute a unique
     * serial number with a length of 48 bits (big endian format).
     *
     * @param serial0 First word of the 48 bit serial number
     *
     * @param serial1 Second word of the 48 bit serial number
     *
     * @param serial2 Third word of the 48 bit serial number
     *
     * @return 0 on success, an error code otherwise
     */
    // uint16_t getSerialNumber(uint16_t& serial0, uint16_t& serial1,
    //                          uint16_t& serial2);

    /**
     * performSelfTest() - The perform_self_test feature can be used as an
     * end-of-line test to confirm sensor functionality.
     *
     * @param sensorStatus 0 means no malfunction detected
     *
     * @return 0 on success, an error code otherwise
     */
    // uint16_t performSelfTest(uint16_t& sensorStatus);

    /**
     * performFactoryReset() - Initiates the reset of all configurations stored
     * in the EEPROM and erases the FRC and ASC algorithm history.
     *
     * @note To avoid unnecessary wear of the EEPROM, this command should only
     * be sent when actual changes to the configuration have been made which
     * should be reverted (The EEPROM is guaranteed to endure at least 2000
     * write cycles before failure). Note that field calibration history (i.e.
     * FRC and ASC) is automatically stored in a separate EEPROM dimensioned for
     * specified sensor lifetime.
     *
     * @return 0 on success, an error code otherwise
     */
    // uint16_t performFactoryReset(void);

    /**
     * reinit() - The reinit command reinitializes the sensor by reloading user
     * settings from EEPROM. Before sending the reinit command, the stop
     * measurement command must be issued. If reinit command does not trigger
     * the desired re-initialization, a power-cycle should be applied to the
     * SCD4x.
     *
     * @note Only available in idle mode.
     *
     * @return 0 on success, an error code otherwise
     */
    // uint16_t reinit(void);

    /**
     * measureSingleShot() - On-demand measurement of CO₂ concentration,
     * relative humidity and temperature. The sensor output is read with the
     * read_measurement command.
     *
     * @note Only available in idle mode.
     *
     * @return 0 on success, an error code otherwise
     */
    // uint16_t measureSingleShot(void);

    /**
     * measureSingleShotRhtOnly() - On-demand measurement of relative humidity
     * and temperature only.
     *
     * @note Only available in idle mode.
     *
     * @return 0 on success, an error code otherwise
     */
    // uint16_t measureSingleShotRhtOnly(void);

    /**
     * powerDown() - Put the sensor from idle to sleep mode to reduce current
     * consumption.
     *
     * @note Only available in idle mode.
     *
     * @return 0 on success, an error code otherwise
     */
    // uint16_t powerDown(void);

    /**
     * wakeUp() - Wake up sensor from sleep mode to idle mode.
     *
     * @note Only available in sleep mode.
     *
     * @return 0 on success, an error code otherwise
     */
    // uint16_t wakeUp(void);

  private:
    uint8_t _isValid = false;
    int _address;
    bool _init = false;
    TwoWire* _i2cPort;

    bool inRange(double value, double max, double min) {
        return !(value <= min) && (value <= max);
    }
};
