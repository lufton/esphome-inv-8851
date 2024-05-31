import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import select
from esphome.const import (
    CONF_PLATFORM,
    ENTITY_CATEGORY_CONFIG,
)

from . import (
    inv_8851_ns,
    Inv8851,
    CONF_INV_8851_ID,
)

DEPENDENCIES = ["inv_8851"]

CODEOWNERS = ["@lufton"]

CONF_AUTO_RETURN = "auto_return"
CONF_BACKLIGHT = "backlight"
CONF_BATTERY_EQUALIZATION = "battery_equalization"
CONF_BATTERY_TYPE = "battery_type"
CONF_BUZZER = "buzzer"
CONF_CHARGE_ENERGY_PRIORITY = "charge_energy_priority"
CONF_FAULT_RECORD = "fault_record"
CONF_FREQUENCY = "frequency"
CONF_GRID_VOLTAGE_RANGE = "grid_voltage_range"
CONF_ON_GRID = "on_grid"
CONF_OUTPUT_ENERGY_PRIORITY = "output_energy_priority"
CONF_OVERLOAD_RESTART = "overload_restart"
CONF_OVERTEMP_RESTART = "overtemp_restart"
CONF_PARALLEL_OPERATION = "parallel_operation"
CONF_PHASE = "phase"
CONF_POWER_BUZZER = "power_buzzer"
CONF_POWERSAVE_MODE = "powersave_mode"
CONF_WARNING_BUZER = "warning_buzer"

OFF_ON_OPTIONS = ["Off", "On"]
OPTIONS = {
    CONF_AUTO_RETURN: OFF_ON_OPTIONS,
    CONF_BACKLIGHT: OFF_ON_OPTIONS,
    CONF_BATTERY_EQUALIZATION: OFF_ON_OPTIONS,
    CONF_BATTERY_TYPE: ["AGM", "Flooded", "User-defined", "Library"],
    CONF_BUZZER: OFF_ON_OPTIONS,
    CONF_CHARGE_ENERGY_PRIORITY: ["PV & Grid", "PV > Grid", "PV only"],
    CONF_FAULT_RECORD: OFF_ON_OPTIONS,
    CONF_FREQUENCY: ["50Hz", "60Hz"],
    CONF_GRID_VOLTAGE_RANGE: ["UPS", "APL"],
    CONF_ON_GRID: OFF_ON_OPTIONS,
    CONF_OUTPUT_ENERGY_PRIORITY: ["PV > Grid > Battery", "PV > Battery > Grid"],
    CONF_OVERLOAD_RESTART: OFF_ON_OPTIONS,
    CONF_OVERTEMP_RESTART: OFF_ON_OPTIONS,
    CONF_PARALLEL_OPERATION: OFF_ON_OPTIONS,
    CONF_PHASE: ["A", "B", "C"],
    CONF_POWER_BUZZER: OFF_ON_OPTIONS,
    CONF_POWERSAVE_MODE: OFF_ON_OPTIONS,
    CONF_WARNING_BUZER: OFF_ON_OPTIONS,
}

Inv8851Select = inv_8851_ns.class_("Inv8851Select", select.Select, cg.Component)

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(CONF_INV_8851_ID): cv.use_id(Inv8851),
            cv.Optional(CONF_AUTO_RETURN): select.select_schema(Inv8851Select,
                entity_category=ENTITY_CATEGORY_CONFIG
            ),
            cv.Optional(CONF_BACKLIGHT): select.select_schema(Inv8851Select,
                entity_category=ENTITY_CATEGORY_CONFIG
            ),
            cv.Optional(CONF_BATTERY_EQUALIZATION): select.select_schema(Inv8851Select,
                entity_category=ENTITY_CATEGORY_CONFIG
            ),
            cv.Optional(CONF_BATTERY_TYPE): select.select_schema(Inv8851Select,
                entity_category=ENTITY_CATEGORY_CONFIG
            ),
            cv.Optional(CONF_BUZZER): select.select_schema(Inv8851Select,
                entity_category=ENTITY_CATEGORY_CONFIG
            ),
            cv.Optional(CONF_CHARGE_ENERGY_PRIORITY): select.select_schema(Inv8851Select,
                entity_category=ENTITY_CATEGORY_CONFIG
            ),
            cv.Optional(CONF_FAULT_RECORD): select.select_schema(Inv8851Select,
                entity_category=ENTITY_CATEGORY_CONFIG
            ),
            cv.Optional(CONF_FREQUENCY): select.select_schema(Inv8851Select,
                entity_category=ENTITY_CATEGORY_CONFIG
            ),
            cv.Optional(CONF_GRID_VOLTAGE_RANGE): select.select_schema(Inv8851Select,
                entity_category=ENTITY_CATEGORY_CONFIG
            ),
            cv.Optional(CONF_ON_GRID): select.select_schema(Inv8851Select,
                entity_category=ENTITY_CATEGORY_CONFIG
            ),
            cv.Optional(CONF_OUTPUT_ENERGY_PRIORITY): select.select_schema(Inv8851Select,
                entity_category=ENTITY_CATEGORY_CONFIG
            ),
            cv.Optional(CONF_OVERLOAD_RESTART): select.select_schema(Inv8851Select,
                entity_category=ENTITY_CATEGORY_CONFIG
            ),
            cv.Optional(CONF_OVERTEMP_RESTART): select.select_schema(Inv8851Select,
                entity_category=ENTITY_CATEGORY_CONFIG
            ),
            cv.Optional(CONF_PARALLEL_OPERATION): select.select_schema(Inv8851Select,
                entity_category=ENTITY_CATEGORY_CONFIG
            ),
            cv.Optional(CONF_PHASE): select.select_schema(Inv8851Select,
                entity_category=ENTITY_CATEGORY_CONFIG
            ),
            cv.Optional(CONF_POWER_BUZZER): select.select_schema(Inv8851Select,
                entity_category=ENTITY_CATEGORY_CONFIG
            ),
            cv.Optional(CONF_POWERSAVE_MODE): select.select_schema(Inv8851Select,
                entity_category=ENTITY_CATEGORY_CONFIG
            ),
            cv.Optional(CONF_WARNING_BUZER): select.select_schema(Inv8851Select,
                entity_category=ENTITY_CATEGORY_CONFIG
            ),
        }
    )
)


async def to_code(config):
    parent = await cg.get_variable(config.get(CONF_INV_8851_ID))
    for option in config:
        if option not in [CONF_PLATFORM, CONF_INV_8851_ID] and (c := config.get(option)):
            sel = await select.new_select(c, options=OPTIONS[option])
            cg.add(getattr(sel, "set_parent")(parent))
            cg.add(getattr(sel, "set_type")(option))
            cg.add(getattr(parent, f"set_{option}_select")(sel))
