import esphome.codegen as cg
from esphome.components import binary_sensor
import esphome.config_validation as cv
from esphome.const import ENTITY_CATEGORY_DIAGNOSTIC

from . import CONF_FINGERPRINT_FPC2532_ID, FingerprintFPC2532Component

CONF_SET_STATUS_AT_BOOT = "status_at_boot"
CONF_STOP_MODE_UART = "stop_mode_uart"
CONF_UART_IRQ_BEFORE_TX = "uart_irq_before_tx"
CONF_ENROLLING = "enrolling_binary"

ICON_CONFIG = "mdi:cog"
ICON_ENROLL = "mdi:key-plus"

DEPENDENCIES = ["fingerprint_FPC2532"]

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_FINGERPRINT_FPC2532_ID): cv.use_id(
            FingerprintFPC2532Component
        ),
        cv.Optional(CONF_ENROLLING): binary_sensor.binary_sensor_schema(
            icon=ICON_ENROLL,
        ),
        cv.Optional(CONF_UART_IRQ_BEFORE_TX): binary_sensor.binary_sensor_schema(
            icon=ICON_CONFIG,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
        cv.Optional(CONF_SET_STATUS_AT_BOOT): binary_sensor.binary_sensor_schema(
            icon=ICON_CONFIG,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
        cv.Optional(CONF_STOP_MODE_UART): binary_sensor.binary_sensor_schema(
            icon=ICON_CONFIG,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_FINGERPRINT_FPC2532_ID])

    if CONF_ENROLLING in config:
        sens = await binary_sensor.new_binary_sensor(config[CONF_ENROLLING])
        cg.add(hub.set_enrolling_binary_sensor(sens))

    if CONF_UART_IRQ_BEFORE_TX in config:
        sens = await binary_sensor.new_binary_sensor(config[CONF_UART_IRQ_BEFORE_TX])
        cg.add(hub.set_uart_irq_before_tx_binary_sensor(sens))

    if CONF_SET_STATUS_AT_BOOT in config:
        sens = await binary_sensor.new_binary_sensor(config[CONF_SET_STATUS_AT_BOOT])
        cg.add(hub.set_status_at_boot_binary_sensor(sens))

    if CONF_STOP_MODE_UART in config:
        sens = await binary_sensor.new_binary_sensor(config[CONF_STOP_MODE_UART])
        cg.add(hub.set_stop_mode_uart_binary_sensor(sens))
