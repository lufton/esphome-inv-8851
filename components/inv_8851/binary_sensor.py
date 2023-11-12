import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import binary_sensor
from esphome.const import (
    CONF_PLATFORM,
    DEVICE_CLASS_BATTERY_CHARGING,
    DEVICE_CLASS_CONNECTIVITY,
    DEVICE_CLASS_POWER,
)

from . import (
    Inv8851,
    CONF_INV_8851_ID,
)

DEPENDENCIES = ["inv_8851"]

CODEOWNERS = ["@lufton"]

CONF_BATTERY = "battery"
CONF_BUCK_TOPOLOGY_INITIALIZATION = "buck_topology_initialization"
CONF_BUS = "bus"
CONF_BUS_AND_GRID_VOLTAGE_MATCH = "bus_and_grid_voltage_match"
CONF_CHARGING = "charging"
CONF_DISABLE_UTILITY = "disable_utility"
CONF_EQUALIZATION_FINISHED = "equalization_finished"
CONF_EQUALIZATION_STARTED = "equalization_started"
CONF_FLOAT_CHARGING = "float_charging"
CONF_GRID_PLL = "grid_pll"
CONF_INVERTER_TOPOLOGY_INITIALIZATION = "inverter_topology_initialization"
CONF_LLC_TOPOLOGY_INITIALIZATION = "llc_topology_initialization"
CONF_PARALLEL_LOCK_PHASE = "parallel_lock_phase"
CONF_PV_EXCESS = "pv_excess"
CONF_PV_INPUT = "pv_input"
CONF_PV_TOPOLOGY_INITIALIZATION = "pv_topology_initialization"
CONF_SYSTEM_INITIALIZATION = "system_initialization"
CONF_SYSTEM_POWER = "system_power"

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(CONF_INV_8851_ID): cv.use_id(Inv8851),
            cv.Optional(CONF_BATTERY): binary_sensor.binary_sensor_schema(
                device_class=DEVICE_CLASS_CONNECTIVITY
            ),
            cv.Optional(CONF_BUCK_TOPOLOGY_INITIALIZATION): binary_sensor.binary_sensor_schema(),
            cv.Optional(CONF_BUS): binary_sensor.binary_sensor_schema(
                device_class=DEVICE_CLASS_CONNECTIVITY
            ),
            cv.Optional(CONF_BUS_AND_GRID_VOLTAGE_MATCH): binary_sensor.binary_sensor_schema(),
            cv.Optional(CONF_CHARGING): binary_sensor.binary_sensor_schema(
                device_class=DEVICE_CLASS_BATTERY_CHARGING
            ),
            cv.Optional(CONF_DISABLE_UTILITY): binary_sensor.binary_sensor_schema(),
            cv.Optional(CONF_EQUALIZATION_FINISHED): binary_sensor.binary_sensor_schema(),
            cv.Optional(CONF_EQUALIZATION_STARTED): binary_sensor.binary_sensor_schema(),
            cv.Optional(CONF_FLOAT_CHARGING): binary_sensor.binary_sensor_schema(
                device_class=DEVICE_CLASS_BATTERY_CHARGING
            ),
            cv.Optional(CONF_GRID_PLL): binary_sensor.binary_sensor_schema(),
            cv.Optional(CONF_INVERTER_TOPOLOGY_INITIALIZATION): binary_sensor.binary_sensor_schema(),
            cv.Optional(CONF_LLC_TOPOLOGY_INITIALIZATION): binary_sensor.binary_sensor_schema(),
            cv.Optional(CONF_PARALLEL_LOCK_PHASE): binary_sensor.binary_sensor_schema(),
            cv.Optional(CONF_PV_EXCESS): binary_sensor.binary_sensor_schema(),
            cv.Optional(CONF_PV_INPUT): binary_sensor.binary_sensor_schema(
                device_class=DEVICE_CLASS_CONNECTIVITY
            ),
            cv.Optional(CONF_PV_TOPOLOGY_INITIALIZATION): binary_sensor.binary_sensor_schema(),
            cv.Optional(CONF_SYSTEM_INITIALIZATION): binary_sensor.binary_sensor_schema(),
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
