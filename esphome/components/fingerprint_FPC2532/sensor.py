import esphome.codegen as cg
from esphome.components import sensor
import esphome.config_validation as cv
from esphome.const import (
    CONF_CAPACITY,
    CONF_FINGERPRINT_COUNT,
    CONF_LAST_FINGER_ID,
    CONF_NUM_SCANS,
    CONF_STATUS,
    ENTITY_CATEGORY_DIAGNOSTIC,
    ICON_ACCOUNT,
    ICON_DATABASE,
    ICON_FINGERPRINT,
)

from . import CONF_FINGERPRINT_FPC2532_ID, FingerprintFPC2532Component

ICON_COG = "mdi:cog"
CONF_SET_STATUS_AT_BOOT = "status_at_boot"
CONF_STOP_MODE_UART = "stop_mode_uart"
CONF_UART_IRQ_BEFORE_TX = "uart_irq_before_tx"
CONF_ENROLLMENT_FEEDBACK = "enrollment_feedback"
ICON_FEEDBACK = "mdi:message-alert-outline"
ICON_CONFIG = "mdi:settings"
CONF_LOCKOUT_TIME = "lockout_time_s"
CONF_LOCKOUT_AFTER_NR_OF_FAILS = "lockout_after_nr_of_fails"
CONF_IDLE_TIME_BEFORE_SLEEP = "idle_time_before_sleep_ms"
CONF_UART_DLY_BEFORE_TX = "uart_dly_before_tx_ms"
CONF_SCAN_INTERVAL = "scan_interval_ms"
CONF_BAUD_RATE = "baud_rate"
CONF_ENROLL_TIMEOUT = "enroll_timeout_s"

DEPENDENCIES = ["fingerprint_FPC2532"]

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_FINGERPRINT_FPC2532_ID): cv.use_id(
            FingerprintFPC2532Component
        ),
        cv.Optional(CONF_FINGERPRINT_COUNT): sensor.sensor_schema(
            icon=ICON_FINGERPRINT,
            accuracy_decimals=0,
        ),
        cv.Optional(CONF_STATUS): sensor.sensor_schema(
            accuracy_decimals=0,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
        cv.Optional(CONF_CAPACITY): sensor.sensor_schema(
            icon=ICON_DATABASE,
            accuracy_decimals=0,
        ),
        cv.Optional(CONF_NUM_SCANS): sensor.sensor_schema(
            icon=ICON_DATABASE,
            accuracy_decimals=0,
            entity_category="",
        ),
        cv.Optional(CONF_ENROLLMENT_FEEDBACK): sensor.sensor_schema(
            icon=ICON_FEEDBACK,
            accuracy_decimals=0,
            entity_category="",
        ),
        cv.Optional(CONF_LAST_FINGER_ID): sensor.sensor_schema(
            icon=ICON_ACCOUNT,
            accuracy_decimals=0,
        ),
        cv.Optional(CONF_SCAN_INTERVAL): sensor.sensor_schema(
            icon=ICON_COG,
            accuracy_decimals=0,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
        cv.Optional(CONF_UART_DLY_BEFORE_TX): sensor.sensor_schema(
            icon=ICON_COG,
            accuracy_decimals=0,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
        cv.Optional(CONF_IDLE_TIME_BEFORE_SLEEP): sensor.sensor_schema(
            icon=ICON_COG,
            accuracy_decimals=0,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
        cv.Optional(CONF_LOCKOUT_AFTER_NR_OF_FAILS): sensor.sensor_schema(
            icon=ICON_COG,
            accuracy_decimals=0,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
        cv.Optional(CONF_LOCKOUT_TIME): sensor.sensor_schema(
            icon=ICON_COG,
            accuracy_decimals=0,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
        cv.Optional(CONF_BAUD_RATE): sensor.sensor_schema(
            icon=ICON_COG,
            accuracy_decimals=0,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
        cv.Optional(CONF_ENROLL_TIMEOUT): sensor.sensor_schema(
            icon=ICON_COG,
            accuracy_decimals=0,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_FINGERPRINT_FPC2532_ID])

    for key in [
        CONF_FINGERPRINT_COUNT,
        CONF_STATUS,
        CONF_CAPACITY,
        CONF_LAST_FINGER_ID,
        CONF_NUM_SCANS,
        CONF_ENROLLMENT_FEEDBACK,
        CONF_SCAN_INTERVAL,
        CONF_UART_DLY_BEFORE_TX,
        CONF_IDLE_TIME_BEFORE_SLEEP,
        CONF_LOCKOUT_AFTER_NR_OF_FAILS,
        CONF_LOCKOUT_TIME,
        CONF_BAUD_RATE,
        CONF_ENROLL_TIMEOUT,
    ]:
        if key not in config:
            continue
        conf = config[key]
        sens = await sensor.new_sensor(conf)
        cg.add(getattr(hub, f"set_{key}_sensor")(sens))
