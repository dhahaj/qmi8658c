# QMI8658C ESPHome Component

This is a custom component for [ESPHome](https://esphome.io/) to support the **QMI8658C** 6-axis Inertial Measurement Unit (IMU). The QMI8658C provides 3-axis acceleration, 3-axis gyroscope, and die temperature measurements over I2C.

## Features

- **Accelerometer:** X, Y, Z axes (in m/s²)
- **Gyroscope:** X, Y, Z axes (in °/s or DPS)
- **Temperature:** Die temperature (in °C)
- Adjustable ranges for both accelerometer and gyroscope

## Installation

You can use this component in your ESPHome configuration by pulling it directly from your local `components` directory (if downloaded) or via GitHub (if hosted).

To use it locally, place the `qmi8658c` folder inside a `components` directory next to your YAML configuration file:

```yaml
external_components:
  - source:
      type: local
      path: components
    components: [qmi8658c]
```

*(Note: Adjust the `source` to `type: git` if pulling directly from a remote repository).*

## Configuration Variables

- **id** (*Optional*, ID): Manually specify the ID used for code generation.
- **address** (*Optional*, int): The I2C address of the sensor. Defaults to `0x6B`. (Use `0x6A` if the SA0 pin is floating/high).
- **update_interval** (*Optional*, Time): The interval to check the sensor. Defaults to `60s`.
- **acceleration_range** (*Optional*, string): The range of the accelerometer. Defaults to `4G`.
  - Allowed values: `2G`, `4G`, `8G`, `16G`
- **gyroscope_range** (*Optional*, string): The range of the gyroscope. Defaults to `256DPS`.
  - Allowed values: `16DPS`, `32DPS`, `64DPS`, `128DPS`, `256DPS`, `512DPS`, `1024DPS`, `2048DPS`
- **acceleration_x** (*Optional*): The sensor for the X-axis of the accelerometer.
- **acceleration_y** (*Optional*): The sensor for the Y-axis of the accelerometer.
- **acceleration_z** (*Optional*): The sensor for the Z-axis of the accelerometer.
- **gyroscope_x** (*Optional*): The sensor for the X-axis of the gyroscope.
- **gyroscope_y** (*Optional*): The sensor for the Y-axis of the gyroscope.
- **gyroscope_z** (*Optional*): The sensor for the Z-axis of the gyroscope.
- **temperature** (*Optional*): The sensor for the internal die temperature.

*Note: For the sensor entries (`acceleration_x`, `gyroscope_x`, `temperature`, etc.), all options from [Sensor Core](https://esphome.io/components/sensor/index.html) are supported.*

## Example Usage

Here is an example configuration for the QMI8658C sensor:

```yaml
i2c:
  sda: GPIO11
  scl: GPIO12
  frequency: 400kHz

sensor:
  - platform: qmi8658c
    address: 0x6B
    update_interval: 1s
    acceleration_range: 4G
    gyroscope_range: 256DPS
    acceleration_x:
      name: "QMI8658C Accel X"
    acceleration_y:
      name: "QMI8658C Accel Y"
    acceleration_z:
      name: "QMI8658C Accel Z"
    gyroscope_x:
      name: "QMI8658C Gyro X"
    gyroscope_y:
      name: "QMI8658C Gyro Y"
    gyroscope_z:
      name: "QMI8658C Gyro Z"
    temperature:
      name: "QMI8658C Die Temp"
```

## Authors & Maintainers

- [@daniel](https://github.com/daniel)
