import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart
from esphome.const import CONF_ID

DEPENDENCIES = ["uart"]

CODEOWNERS = ["@lufton"]

CONF_INV_8851_ID = "inv_8851_id"

inv_8851_ns = cg.esphome_ns.namespace("inv_8851")
Inv8851 = inv_8851_ns.class_("Inv8851", cg.PollingComponent, uart.UARTDevice)

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(Inv8851),
        }
    )
    .extend(cv.polling_component_schema("5s"))
    .extend(uart.UART_DEVICE_SCHEMA)
)

async def to_code(config):
    var = cg.new_Pvariable(config.get(CONF_ID))
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)
