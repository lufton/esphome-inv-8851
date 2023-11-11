import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import binary_sensor
from esphome.const import (
    CONF_PLATFORM,
    DEVICE_CLASS_BATTERY_CHARGING,
    DEVICE_CLASS_CONNECTIVITY,
    DEVICE_CLASS_POWER,
    DEVICE_CLASS_PROBLEM,
)

from . import (
    Inv8851,
    CONF_INV_8851_ID,
)

DEPENDENCIES = ["inv_8851"]

CODEOWNERS = ["@lufton"]

CONF_BATTERY_CONNECTED = "battery_connected"
CONF_BATTERY_CHARGING = "battery_charging"
CONF_BUS_PROBLEM = "bus_problem"
CONF_FLOAT_CHARGING = "float_charging"
CONF_GRID_PLL_PROBLEM = "grid_pll_problem"
CONF_GRID_POWER = "grid_power"
CONF_PARALLEL_LOCK_PHASE_PROBLEM = "parallel_lock_phase_problem"
CONF_PV_EXCESS = "pv_excess"
CONF_PV_INPUT_PROBLEM = "pv_input_problem"
CONF_SYSTEM_POWER = "system_power"

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(CONF_INV_8851_ID): cv.use_id(Inv8851),
            cv.Optional(CONF_BATTERY_CHARGING): binary_sensor.binary_sensor_schema(
                device_class=DEVICE_CLASS_BATTERY_CHARGING
            ),
            cv.Optional(CONF_BATTERY_CONNECTED): binary_sensor.binary_sensor_schema(
                device_class=DEVICE_CLASS_CONNECTIVITY
            ),
            cv.Optional(CONF_BUS_PROBLEM): binary_sensor.binary_sensor_schema(
                device_class=DEVICE_CLASS_PROBLEM
            ),
            cv.Optional(CONF_FLOAT_CHARGING): binary_sensor.binary_sensor_schema(
            ),
            cv.Optional(CONF_GRID_PLL_PROBLEM): binary_sensor.binary_sensor_schema(
                device_class=DEVICE_CLASS_PROBLEM
            ),
            cv.Optional(CONF_GRID_POWER): binary_sensor.binary_sensor_schema(
                device_class=DEVICE_CLASS_POWER
            ),
            cv.Optional(CONF_PARALLEL_LOCK_PHASE_PROBLEM): binary_sensor.binary_sensor_schema(
                device_class=DEVICE_CLASS_PROBLEM
            ),
            cv.Optional(CONF_PV_EXCESS): binary_sensor.binary_sensor_schema(
            ),
            cv.Optional(CONF_PV_INPUT_PROBLEM): binary_sensor.binary_sensor_schema(
                device_class=DEVICE_CLASS_PROBLEM
            ),
            cv.Optional(CONF_SYSTEM_POWER): binary_sensor.binary_sensor_schema(
                device_class=DEVICE_CLASS_POWER
            ),
        }
    )
)


async def to_code(config):
    parent = await cg.get_variable(config.get(CONF_INV_8851_ID))
    for option in config:
        if option not in [CONF_PLATFORM, CONF_INV_8851_ID] and (c := config.get(option)):
            bin_sens = await binary_sensor.new_binary_sensor(c)
            cg.add(getattr(parent, f"set_{option}_binary_sensor")(bin_sens))
