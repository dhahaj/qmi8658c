#include "qmi8658c.h"
#include "esphome/core/log.h"
#include "esphome/core/hal.h"

namespace esphome {
namespace qmi8658c {

static const char *const TAG = "qmi8658c";
static const float GRAVITY_MS2 = 9.80665f;

void QMI8658CComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up QMI8658C...");

  uint8_t who = 0;
  if (!this->read_byte(QMI8658C_REG_WHO_AM_I, &who) || who != 0x05) {
    ESP_LOGE(TAG, "WHO_AM_I mismatch (got 0x%02X, expected 0x05)", who);
    this->mark_failed();
    return;
  }

  // Soft reset, then give the boot sequence a moment. (Full System Turn-On is
  // 1.75 s per datasheet, but that is only required out of a true power-down;
  // a short settle is fine here and keeps setup() non-blocking.)
  this->write_byte(QMI8658C_REG_RESET, 0xB0);
  delay(15);

  // Auto-increment register reads so a single burst grabs all axes.
  this->write_byte(QMI8658C_REG_CTRL1, QMI8658C_CTRL1_VALUE);

  // Accel full-scale + ODR (aFS is bits 6:4).
  this->write_byte(QMI8658C_REG_CTRL2,
                   (uint8_t) ((this->accel_range_ << 4) | QMI8658C_ODR_DEFAULT));
  // Gyro full-scale + ODR (gFS is bits 6:4).
  this->write_byte(QMI8658C_REG_CTRL3,
                   (uint8_t) ((this->gyro_range_ << 4) | QMI8658C_ODR_DEFAULT));
  // Low-pass filters off. Set bits per CTRL5 if you want filtering.
  this->write_byte(QMI8658C_REG_CTRL5, 0x00);
  // Enable accel + gyro (6DOF).
  this->write_byte(QMI8658C_REG_CTRL7, QMI8658C_CTRL7_ENABLE);

  // Precompute scale factors (datasheet Tables 7 & 8).
  switch (this->accel_range_) {
    case ACCEL_RANGE_2G: this->accel_lsb_per_g_ = 16384.0f; break;
    case ACCEL_RANGE_4G: this->accel_lsb_per_g_ = 8192.0f; break;
    case ACCEL_RANGE_8G: this->accel_lsb_per_g_ = 4096.0f; break;
    case ACCEL_RANGE_16G: this->accel_lsb_per_g_ = 2048.0f; break;
  }
  switch (this->gyro_range_) {
    case GYRO_RANGE_16DPS: this->gyro_lsb_per_dps_ = 2048.0f; break;
    case GYRO_RANGE_32DPS: this->gyro_lsb_per_dps_ = 1024.0f; break;
    case GYRO_RANGE_64DPS: this->gyro_lsb_per_dps_ = 512.0f; break;
    case GYRO_RANGE_128DPS: this->gyro_lsb_per_dps_ = 256.0f; break;
    case GYRO_RANGE_256DPS: this->gyro_lsb_per_dps_ = 128.0f; break;
    case GYRO_RANGE_512DPS: this->gyro_lsb_per_dps_ = 64.0f; break;
    case GYRO_RANGE_1024DPS: this->gyro_lsb_per_dps_ = 32.0f; break;
    case GYRO_RANGE_2048DPS: this->gyro_lsb_per_dps_ = 16.0f; break;
  }
}

void QMI8658CComponent::update() {
  // 12 contiguous bytes: AX_L AX_H AY_L AY_H AZ_L AZ_H GX_L GX_H GY_L GY_H GZ_L GZ_H
  uint8_t b[12];
  if (!this->read_bytes(QMI8658C_REG_AX_L, b, 12)) {
    ESP_LOGW(TAG, "Reading sensor data failed");
    this->status_set_warning();
    return;
  }

  auto i16 = [](uint8_t lo, uint8_t hi) -> int16_t {
    return (int16_t) ((uint16_t) hi << 8 | lo);
  };

  float ax = i16(b[0], b[1]) / this->accel_lsb_per_g_ * GRAVITY_MS2;
  float ay = i16(b[2], b[3]) / this->accel_lsb_per_g_ * GRAVITY_MS2;
  float az = i16(b[4], b[5]) / this->accel_lsb_per_g_ * GRAVITY_MS2;
  float gx = i16(b[6], b[7]) / this->gyro_lsb_per_dps_;
  float gy = i16(b[8], b[9]) / this->gyro_lsb_per_dps_;
  float gz = i16(b[10], b[11]) / this->gyro_lsb_per_dps_;

  if (this->accel_x_ != nullptr) this->accel_x_->publish_state(ax);
  if (this->accel_y_ != nullptr) this->accel_y_->publish_state(ay);
  if (this->accel_z_ != nullptr) this->accel_z_->publish_state(az);
  if (this->gyro_x_ != nullptr) this->gyro_x_->publish_state(gx);
  if (this->gyro_y_ != nullptr) this->gyro_y_->publish_state(gy);
  if (this->gyro_z_ != nullptr) this->gyro_z_->publish_state(gz);

  if (this->temperature_ != nullptr) {
    uint8_t t[2];
    if (this->read_bytes(QMI8658C_REG_TEMP_L, t, 2)) {
      // 256 LSB/degC, signed (datasheet 3.8). This is die temp, not ambient.
      float temp = i16(t[0], t[1]) / 256.0f;
      this->temperature_->publish_state(temp);
    }
  }

  this->status_clear_warning();
}

void QMI8658CComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "QMI8658C:");
  LOG_I2C_DEVICE(this);
  if (this->is_failed())
    ESP_LOGE(TAG, "  Communication failed (check WHO_AM_I / wiring)");
  LOG_UPDATE_INTERVAL(this);
  LOG_SENSOR("  ", "Accel X", this->accel_x_);
  LOG_SENSOR("  ", "Accel Y", this->accel_y_);
  LOG_SENSOR("  ", "Accel Z", this->accel_z_);
  LOG_SENSOR("  ", "Gyro X", this->gyro_x_);
  LOG_SENSOR("  ", "Gyro Y", this->gyro_y_);
  LOG_SENSOR("  ", "Gyro Z", this->gyro_z_);
  LOG_SENSOR("  ", "Temperature", this->temperature_);
}

}  // namespace qmi8658c
}  // namespace esphome
