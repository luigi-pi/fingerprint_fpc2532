import esphome.codegen as cg
from esphome.components import text_sensor
import esphome.config_validation as cv
from esphome.const import CONF_ICON, CONF_ID

from . import CONF_FINGERPRINT_FPC2532_ID, FingerprintFPC2532Component

CONF_STATUS_TEXT = "text_status"
CONF_UNIQUE_ID = "unique_id"
CONF_VERSION = "version"
ICON_INFO = "mdi:information"

DEPENDENCIES = ["fingerprint_FPC2532"]


def validate_icons(config):
    sensor_id = str(config[CONF_ID])
    config_icon_group = {CONF_STATUS_TEXT, CONF_UNIQUE_ID, CONF_VERSION}
    return ICON_INFO if sensor_id in config_icon_group else "mdi:checkbox-blank-outline"


CONFIG_SCHEMA = text_sensor.text_sensor_schema().extend(
    {
        cv.GenerateID(CONF_FINGERPRINT_FPC2532_ID): cv.use_id(
            FingerprintFPC2532Component
        ),
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_FINGERPRINT_FPC2532_ID])
    sens = await text_sensor.new_text_sensor(config)
    if CONF_ICON not in config:
        icon = validate_icons(config)
        cg.add(sens.set_icon(icon))

    cg.add(getattr(hub, f"set_{config[CONF_ID]}_sensor")(sens))
