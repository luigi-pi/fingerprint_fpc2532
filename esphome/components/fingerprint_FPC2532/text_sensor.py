import esphome.codegen as cg
from esphome.components import text_sensor
import esphome.config_validation as cv
from esphome.const import CONF_ICON, CONF_ID, CONF_LAST_FINGER_ID, ICON_ACCOUNT
from . import CONF_FINGERPRINT_FPC2532_ID, FingerprintFPC2532Component

CONF_STATUS_TEXT = "text_status"
CONF_UNIQUE_ID = "unique_id"
CONF_VERSION = "version"
ICON_INFO = "mdi:information"

DEPENDENCIES = ["fingerprint_FPC2532"]


def validate_icons(config):
    # Use the raw string key from config, not the generated C++ ID
    sensor_key = config.get("sensor_type", str(config[CONF_ID]))
    config_icon_group = {CONF_STATUS_TEXT, CONF_UNIQUE_ID, CONF_VERSION}
    config_icon_group1 = {CONF_LAST_FINGER_ID}
    if sensor_key in config_icon_group:
        return ICON_INFO
    if sensor_key in config_icon_group1:
        return ICON_ACCOUNT
    return "mdi:checkbox-blank-outline"


CONFIG_SCHEMA = text_sensor.text_sensor_schema().extend(
    {
        cv.GenerateID(CONF_FINGERPRINT_FPC2532_ID): cv.use_id(
            FingerprintFPC2532Component
        ),
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_FINGERPRINT_FPC2532_ID])

    # Inject icon at compile-time before sensor creation instead of calling
    # set_icon() at runtime (removed in ESPHome API change #14564)
    if CONF_ICON not in config:
        config = {**config, CONF_ICON: validate_icons(config)}

    sens = await text_sensor.new_text_sensor(config)
    cg.add(getattr(hub, f"set_{config[CONF_ID]}_sensor")(sens))
