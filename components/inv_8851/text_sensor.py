import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import text_sensor
from esphome.const import (
    CONF_PLATFORM,
)

from . import (
    Inv8851,
    CONF_INV_8851_ID,
)

DEPENDENCIES = ["inv_8851"]

CODEOWNERS = ["@lufton"]

CONF_BUCK_TOPOLOGY = "buck_topology"
CONF_INVERTER_TOPOLOGY = "inverter_topology"
CONF_LLC_TOPOLOGY = "llc_topology"
CONF_PV_TOPOLOGY = "pv_topology"

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(CONF_INV_8851_ID): cv.use_id(Inv8851),
            cv.Optional(CONF_BUCK_TOPOLOGY): text_sensor.text_sensor_schema(),
            cv.Optional(CONF_INVERTER_TOPOLOGY): text_sensor.text_sensor_schema(),
            cv.Optional(CONF_LLC_TOPOLOGY): text_sensor.text_sensor_schema(),
            cv.Optional(CONF_PV_TOPOLOGY): text_sensor.text_sensor_schema(),
        }
    )
)


async def to_code(config):
    parent = await cg.get_variable(config.get(CONF_INV_8851_ID))
    for option in config:
        if option not in [CONF_PLATFORM, CONF_INV_8851_ID] and (c := config.get(option)):
            sens = await text_sensor.new_text_sensor(c)
            cg.add(getattr(parent, f"set_{option}_text_sensor")(sens))
