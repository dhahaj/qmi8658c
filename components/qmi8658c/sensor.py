import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c, sensor
from esphome.const import (
    CONF_ID,
    CONF_TEMPERATURE,
    DEVICE_CLASS_TEMPERATURE,
    STATE_CLASS_MEASUREMENT,
    UNIT_CELSIUS,
    UNIT_DEGREE_PER_SECOND,
    UNIT_METER_PER_SECOND_SQUARED,
)

CODEOWNERS = ["@daniel"]
DEPENDENCIES = ["i2c"]

qmi8658c_ns = cg.esphome_ns.namespace("qmi8658c")
QMI8658CComponent = qmi8658c_ns.class_(
    "QMI8658CComponent", cg.PollingComponent, i2c.I2CDevice
)

# --- enums must match the C++ header exactly ---
AccelRange = qmi8658c_ns.enum("AccelRange")
ACCEL_RANGE = {
    "2G": AccelRange.ACCEL_RANGE_2G,
    "4G": AccelRange.ACCEL_RANGE_4G,
    "8G": AccelRange.ACCEL_RANGE_8G,
    "16G": AccelRange.ACCEL_RANGE_16G,
}

GyroRange = qmi8658c_ns.enum("GyroRange")
GYRO_RANGE = {
    "16DPS": GyroRange.GYRO_RANGE_16DPS,
    "32DPS": GyroRange.GYRO_RANGE_32DPS,
    "64DPS": GyroRange.GYRO_RANGE_64DPS,
    "128DPS": GyroRange.GYRO_RANGE_128DPS,
    "256DPS": GyroRange.GYRO_RANGE_256DPS,
    "512DPS": GyroRange.GYRO_RANGE_512DPS,
    "1024DPS": GyroRange.GYRO_RANGE_1024DPS,
    "2048DPS": GyroRange.GYRO_RANGE_2048DPS,
}

CONF_ACCEL_RANGE = "acceleration_range"
CONF_GYRO_RANGE = "gyroscope_range"
CONF_ACCELERATION_X = "acceleration_x"
CONF_ACCELERATION_Y = "acceleration_y"
CONF_ACCELERATION_Z = "acceleration_z"
CONF_GYROSCOPE_X = "gyroscope_x"
CONF_GYROSCOPE_Y = "gyroscope_y"
CONF_GYROSCOPE_Z = "gyroscope_z"


def accel_schema():
    return sensor.sensor_schema(
        unit_of_measurement=UNIT_METER_PER_SECOND_SQUARED,
        accuracy_decimals=2,
        state_class=STATE_CLASS_MEASUREMENT,
        icon="mdi:axis-arrow",
    )


def gyro_schema():
    return sensor.sensor_schema(
        unit_of_measurement=UNIT_DEGREE_PER_SECOND,
        accuracy_decimals=2,
        state_class=STATE_CLASS_MEASUREMENT,
        icon="mdi:axis-z-rotate-clockwise",
    )


CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(QMI8658CComponent),
            cv.Optional(CONF_ACCEL_RANGE, default="4G"): cv.enum(
                ACCEL_RANGE, upper=True
            ),
            cv.Optional(CONF_GYRO_RANGE, default="256DPS"): cv.enum(
                GYRO_RANGE, upper=True
            ),
            cv.Optional(CONF_ACCELERATION_X): accel_schema(),
            cv.Optional(CONF_ACCELERATION_Y): accel_schema(),
            cv.Optional(CONF_ACCELERATION_Z): accel_schema(),
            cv.Optional(CONF_GYROSCOPE_X): gyro_schema(),
            cv.Optional(CONF_GYROSCOPE_Y): gyro_schema(),
            cv.Optional(CONF_GYROSCOPE_Z): gyro_schema(),
            cv.Optional(CONF_TEMPERATURE): sensor.sensor_schema(
                unit_of_measurement=UNIT_CELSIUS,
                accuracy_decimals=1,
                device_class=DEVICE_CLASS_TEMPERATURE,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
        }
    )
    .extend(cv.polling_component_schema("60s"))
    .extend(i2c.i2c_device_schema(0x6B))
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)

    cg.add(var.set_accel_range(config[CONF_ACCEL_RANGE]))
    cg.add(var.set_gyro_range(config[CONF_GYRO_RANGE]))

    for axis, setter in [
        (CONF_ACCELERATION_X, var.set_accel_x_sensor),
        (CONF_ACCELERATION_Y, var.set_accel_y_sensor),
        (CONF_ACCELERATION_Z, var.set_accel_z_sensor),
        (CONF_GYROSCOPE_X, var.set_gyro_x_sensor),
        (CONF_GYROSCOPE_Y, var.set_gyro_y_sensor),
        (CONF_GYROSCOPE_Z, var.set_gyro_z_sensor),
        (CONF_TEMPERATURE, var.set_temperature_sensor),
    ]:
        if axis in config:
            sens = await sensor.new_sensor(config[axis])
            cg.add(setter(sens))
