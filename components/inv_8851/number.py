from logging import error
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import number
from esphome.const import (
    CONF_PLATFORM,
    DEVICE_CLASS_CURRENT,
    DEVICE_CLASS_FREQUENCY,
    DEVICE_CLASS_VOLTAGE,
    ENTITY_CATEGORY_CONFIG,
    UNIT_AMPERE,
    UNIT_HERTZ,
    UNIT_MINUTE,
    UNIT_VOLT,
)

from . import INV_8851_NS, CONF_INV_8851_ID, INV_8851

DEPENDENCIES = ["inv_8851"]

CODEOWNERS = ["@lufton"]

CONF_BATTERY_BACK_TO_UTIL_VOLTAGE = "battery_back_to_util_voltage"
CONF_BATTERY_BULK_VOLTAGE = "battery_bulk_voltage"
CONF_BATTERY_FLOAT_VOLTAGE = "battery_float_voltage"
CONF_BATTERY_CUT_OFF_CURRENT = "battery_charge_cut_off_current"
CONF_BATTERY_CUT_OFF_VOLTAGE = "battery_cut_off_voltage"
CONF_BATTERY_EQUALIZATION_INTERVAL = "battery_equalization_interval"
CONF_BATTERY_EQUALIZATION_TIME = "battery_equalization_time"
CONF_BATTERY_EQUALIZATION_TIMEOUT = "battery_equalization_timeout"
CONF_BATTERY_EQUALIZATION_VOLTAGE = "battery_equalization_voltage"
CONF_OUTPUT_FREQUENCY = "output_frequency"
CONF_OUTPUT_VOLTAGE = "output_voltage"
CONF_TOTAL_CHARGE_CURRENT = "total_charge_current"
CONF_UTIL_CHARGE_CURRENT = "util_charge_current"

NUMBERS = {
    CONF_BATTERY_BACK_TO_UTIL_VOLTAGE: [40, 58.4, 0.1],
    CONF_BATTERY_BULK_VOLTAGE: [48, 58.4, 0.1],
    CONF_BATTERY_FLOAT_VOLTAGE: [48, 58.4, 0.1],
    CONF_BATTERY_CUT_OFF_CURRENT: [2, 20, 0.1],
    CONF_BATTERY_CUT_OFF_VOLTAGE: [40, 48, 0.1],
    CONF_BATTERY_EQUALIZATION_INTERVAL: [0, 90, 1],
    CONF_BATTERY_EQUALIZATION_TIME: [5, 900, 1],
    CONF_BATTERY_EQUALIZATION_TIMEOUT: [5, 900, 1],
    CONF_BATTERY_EQUALIZATION_VOLTAGE: [50, 59, 0.1],
    CONF_OUTPUT_FREQUENCY: [50, 60, 1],
    CONF_OUTPUT_VOLTAGE: [100, 240, 1],
    CONF_TOTAL_CHARGE_CURRENT: [0, 130, 0.1],
    CONF_UTIL_CHARGE_CURRENT: [0, 110, 0.1],
}

INV_8851_NUMBER = INV_8851_NS.class_("Inv8851Number", number.Number, cg.Component)

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(CONF_INV_8851_ID): cv.use_id(INV_8851),
            cv.Optional(CONF_BATTERY_BACK_TO_UTIL_VOLTAGE): number.number_schema(INV_8851_NUMBER,
                unit_of_measurement=UNIT_VOLT,
                device_class=DEVICE_CLASS_VOLTAGE,
                entity_category=ENTITY_CATEGORY_CONFIG,
            ),
            cv.Optional(CONF_BATTERY_BULK_VOLTAGE): number.number_schema(INV_8851_NUMBER,
                unit_of_measurement=UNIT_VOLT,
                device_class=DEVICE_CLASS_VOLTAGE,
                entity_category=ENTITY_CATEGORY_CONFIG,
            ),
            cv.Optional(CONF_BATTERY_FLOAT_VOLTAGE): number.number_schema(INV_8851_NUMBER,
                unit_of_measurement=UNIT_VOLT,
                device_class=DEVICE_CLASS_VOLTAGE,
                entity_category=ENTITY_CATEGORY_CONFIG,
            ),
            cv.Optional(CONF_BATTERY_CUT_OFF_CURRENT): number.number_schema(INV_8851_NUMBER,
                unit_of_measurement=UNIT_AMPERE,
                device_class=DEVICE_CLASS_CURRENT,
                entity_category=ENTITY_CATEGORY_CONFIG,
            ),
            cv.Optional(CONF_BATTERY_CUT_OFF_VOLTAGE): number.number_schema(INV_8851_NUMBER,
                unit_of_measurement=UNIT_VOLT,
                device_class=DEVICE_CLASS_VOLTAGE,
                entity_category=ENTITY_CATEGORY_CONFIG,
            ),
            cv.Optional(CONF_BATTERY_EQUALIZATION_INTERVAL): number.number_schema(INV_8851_NUMBER,
                entity_category=ENTITY_CATEGORY_CONFIG,
            ),
            cv.Optional(CONF_BATTERY_EQUALIZATION_TIME): number.number_schema(INV_8851_NUMBER,
                unit_of_measurement=UNIT_MINUTE,
                entity_category=ENTITY_CATEGORY_CONFIG,
            ),
            cv.Optional(CONF_BATTERY_EQUALIZATION_TIMEOUT): number.number_schema(INV_8851_NUMBER,
                unit_of_measurement=UNIT_MINUTE,
                entity_category=ENTITY_CATEGORY_CONFIG,
            ),
            cv.Optional(CONF_BATTERY_EQUALIZATION_VOLTAGE): number.number_schema(INV_8851_NUMBER,
                unit_of_measurement=UNIT_VOLT,
                device_class=DEVICE_CLASS_VOLTAGE,
                entity_category=ENTITY_CATEGORY_CONFIG,
            ),
            cv.Optional(CONF_OUTPUT_FREQUENCY): number.number_schema(INV_8851_NUMBER,
                unit_of_measurement=UNIT_HERTZ,
                device_class=DEVICE_CLASS_FREQUENCY,
                entity_category=ENTITY_CATEGORY_CONFIG,
            ),
            cv.Optional(CONF_OUTPUT_VOLTAGE): number.number_schema(INV_8851_NUMBER,
                unit_of_measurement=UNIT_VOLT,
                device_class=DEVICE_CLASS_VOLTAGE,
                entity_category=ENTITY_CATEGORY_CONFIG,
            ),
            cv.Optional(CONF_TOTAL_CHARGE_CURRENT): number.number_schema(INV_8851_NUMBER,
                unit_of_measurement=UNIT_AMPERE,
                device_class=DEVICE_CLASS_CURRENT,
                entity_category=ENTITY_CATEGORY_CONFIG,
            ),
            cv.Optional(CONF_UTIL_CHARGE_CURRENT): number.number_schema(INV_8851_NUMBER,
                unit_of_measurement=UNIT_AMPERE,
                device_class=DEVICE_CLASS_CURRENT,
                entity_category=ENTITY_CATEGORY_CONFIG,
            ),
        }
    )
)


async def to_code(config):
    parent = await cg.get_variable(config.get(CONF_INV_8851_ID))
    for option in config:
        if option not in [CONF_PLATFORM, CONF_INV_8851_ID] and (c := config.get(option)):
            num = await number.new_number(c, min_value=NUMBERS[option][0], max_value=NUMBERS[option][1], step=NUMBERS[option][2])
            cg.add(getattr(num, "set_parent")(parent))
            cg.add(getattr(num, "set_type")(option))
            cg.add(getattr(parent, "set_" + option + "_number")(num))
