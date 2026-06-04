#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/i2c/i2c.h"

namespace esphome {
namespace qmi8658c {

// --- Register map (UI register addresses, datasheet Table 22) ---
static const uint8_t QMI8658C_REG_WHO_AM_I = 0x00;  // == 0x05
static const uint8_t QMI8658C_REG_REVISION = 0x01;
static const uint8_t QMI8658C_REG_CTRL1 = 0x02;   // SIM / ADDR_AI / BE / sensorDisable
static const uint8_t QMI8658C_REG_CTRL2 = 0x03;   // accel: aST / aFS / aODR
static const uint8_t QMI8658C_REG_CTRL3 = 0x04;   // gyro:  gST / gFS / gODR
static const uint8_t QMI8658C_REG_CTRL5 = 0x06;   // low-pass filter config
static const uint8_t QMI8658C_REG_CTRL7 = 0x08;   // sensor enables
static const uint8_t QMI8658C_REG_STATUS0 = 0x2E; // bit1 gDA, bit0 aDA
static const uint8_t QMI8658C_REG_TEMP_L = 0x33;  // temp, then AX_L..GZ_H follow
static const uint8_t QMI8658C_REG_AX_L = 0x35;    // AX_L..GZ_H = 12 contiguous bytes
static const uint8_t QMI8658C_REG_RESET = 0x60;   // write 0xB0 = soft reset

// CTRL1: ADDR_AI=1 (auto-increment reads), BE=0 (little-endian), 4-wire (n/a on I2C)
static const uint8_t QMI8658C_CTRL1_VALUE = 0x40;
// ODR field shared by accel-only (CTRL2) and 6DOF. 0x04 -> ~470 Hz in 6DOF mode.
static const uint8_t QMI8658C_ODR_DEFAULT = 0x04;
// CTRL7: aEN(bit0)=1, gEN(bit1)=1  -> 6DOF mode
static const uint8_t QMI8658C_CTRL7_ENABLE = 0x03;

enum AccelRange : uint8_t {
  ACCEL_RANGE_2G = 0,   // shifted <<4 into aFS
  ACCEL_RANGE_4G = 1,
  ACCEL_RANGE_8G = 2,
  ACCEL_RANGE_16G = 3,
};

enum GyroRange : uint8_t {
  GYRO_RANGE_16DPS = 0,  // shifted <<4 into gFS
  GYRO_RANGE_32DPS = 1,
  GYRO_RANGE_64DPS = 2,
  GYRO_RANGE_128DPS = 3,
  GYRO_RANGE_256DPS = 4,
  GYRO_RANGE_512DPS = 5,
  GYRO_RANGE_1024DPS = 6,
  GYRO_RANGE_2048DPS = 7,
};

class QMI8658CComponent : public PollingComponent, public i2c::I2CDevice {
 public:
  void setup() override;
  void update() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::DATA; }

  void set_accel_range(AccelRange range) { this->accel_range_ = range; }
  void set_gyro_range(GyroRange range) { this->gyro_range_ = range; }

  void set_accel_x_sensor(sensor::Sensor *s) { this->accel_x_ = s; }
  void set_accel_y_sensor(sensor::Sensor *s) { this->accel_y_ = s; }
  void set_accel_z_sensor(sensor::Sensor *s) { this->accel_z_ = s; }
  void set_gyro_x_sensor(sensor::Sensor *s) { this->gyro_x_ = s; }
  void set_gyro_y_sensor(sensor::Sensor *s) { this->gyro_y_ = s; }
  void set_gyro_z_sensor(sensor::Sensor *s) { this->gyro_z_ = s; }
  void set_temperature_sensor(sensor::Sensor *s) { this->temperature_ = s; }

 protected:
  AccelRange accel_range_{ACCEL_RANGE_4G};
  GyroRange gyro_range_{GYRO_RANGE_256DPS};
  float accel_lsb_per_g_{8192.0f};   // set from range in setup()
  float gyro_lsb_per_dps_{128.0f};   // set from range in setup()

  sensor::Sensor *accel_x_{nullptr};
  sensor::Sensor *accel_y_{nullptr};
  sensor::Sensor *accel_z_{nullptr};
  sensor::Sensor *gyro_x_{nullptr};
  sensor::Sensor *gyro_y_{nullptr};
  sensor::Sensor *gyro_z_{nullptr};
  sensor::Sensor *temperature_{nullptr};
};

}  // namespace qmi8658c
}  // namespace esphome
