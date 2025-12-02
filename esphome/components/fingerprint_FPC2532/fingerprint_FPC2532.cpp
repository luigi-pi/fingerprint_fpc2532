#include "fingerprint_FPC2532.h"
#include "esphome/core/log.h"
//#include "esphome.h"
#include <cinttypes>
#include <vector>
#include <string>
#include "fpc_api.h"

namespace esphome {
namespace fingerprint_FPC2532 {

static const char *const TAG = "fingerprint_FPC2532";

/*Helper functions*/

static const char *get_id_type_str_(uint16_t id_type) {
  switch (id_type) {
    case ID_TYPE_NONE:
      return "ID.None";
    case ID_TYPE_ALL:
      return "ID.All";
    case ID_TYPE_SPECIFIED:
      return "ID.Specified";
    case ID_TYPE_GENERATE_NEW:
      return "ID.Generate";
    default:
      return "ID.Unknown";
  }
}
static const char *get_event_str_(uint16_t evt) {
  switch (evt) {
    case EVENT_NONE:
      return "Evt.None";
    case EVENT_IDLE:
      return "Evt.Idle";
    case EVENT_ARMED:
      return "Evt.Armed";
    case EVENT_FINGER_DETECT:
      return "Evt.FingerDetect";
    case EVENT_FINGER_LOST:
      return "Evt.FingerLost";
    case EVENT_IMAGE_READY:
      return "Evt.ImageCaptured";
    case EVENT_CMD_FAILED:
      return "Evt.Failure";
  }
  return "Evt.Unknown_event";
}
static std::string get_state_str_(uint16_t state) {
  std::string s;

  if (state & STATE_ENROLL)
    s += "Enroll mode | ";
  if (state & STATE_IDENTIFY)
    s += "Identification mode | ";
  if (state & STATE_NAVIGATION)
    s += "Navigation mode | ";
  if (state & STATE_APP_FW_READY)
    s += "App FW Ready | ";
  if (state & STATE_CAPTURE)
    s += "Capturing | ";
  if (state & STATE_IMAGE_AVAILABLE)
    s += "Image Available | ";
  if (state & STATE_DATA_TRANSFER)
    s += "Data Transfer | ";
  if (state & STATE_FINGER_DOWN)
    s += "Finger Down | ";
  if (state & STATE_SYS_ERROR)
    s += "System Error | ";
  if (s.empty())
    s = "Unknown_state";

  return s;
}
static const char *get_enroll_feedback_str_(uint8_t feedback) {
  switch (feedback) {
    case ENROLL_FEEDBACK_DONE:
      return "Done";
    case ENROLL_FEEDBACK_PROGRESS:
      return "Progress";
    case ENROLL_FEEDBACK_REJECT_LOW_QUALITY:
      return "Reject.LowQuality";
    case ENROLL_FEEDBACK_REJECT_LOW_COVERAGE:
      return "Reject.LowCoverage";
    case ENROLL_FEEDBACK_REJECT_LOW_MOBILITY:
      return "Reject.LowMobility";
    case ENROLL_FEEDBACK_REJECT_OTHER:
      return "Reject.Other";
    case ENROLL_FEEDBACK_PROGRESS_IMMOBILE:
      return "Progress.Immobile";
    default:
      break;
  }
  return "Unknown";
}
static const char *get_gesture_str_(uint8_t gesture) {
  switch (gesture) {
    case CMD_NAV_EVENT_NONE:
      return "None";
    case CMD_NAV_EVENT_UP:
      return "Gesture.Up";
    case CMD_NAV_EVENT_DOWN:
      return "Gesture.Down";
    case CMD_NAV_EVENT_RIGHT:
      return "Gesture.Right";
    case CMD_NAV_EVENT_LEFT:
      return "Gesture.Left";
    case CMD_NAV_EVENT_PRESS:
      return "Gesture.Press";
    case CMD_NAV_EVENT_LONG_PRESS:
      return "Gesture.LongPress";
    default:
      break;
  }
  return "Unknown";
}
static const char *fpc_result_to_string(fpc::fpc_result_t result) {
  switch (result) {
    // Information / Success
    case FPC_RESULT_OK:
      return "OK";
    case FPC_PENDING_OPERATION:
      return "Pending Operation";
    case FPC_RESULT_DATA_NOT_SET:
      return "Data Not Set";
    case FPC_RESULT_CMD_ID_NOT_SUPPORTED:
      return "Command ID Not Supported";

    // General Errors
    case FPC_RESULT_FAILURE:
      return "Failure";
    case FPC_RESULT_INVALID_PARAM:
      return "Invalid Parameter";
    case FPC_RESULT_WRONG_STATE:
      return "Wrong State";
    case FPC_RESULT_OUT_OF_MEMORY:
      return "Out of Memory";
    case FPC_RESULT_TIMEOUT:
      return "Timeout";
    case FPC_RESULT_NOT_SUPPORTED:
      return "Not Supported";

    // Template / User ID Errors
    case FPC_RESULT_USER_ID_EXISTS:
      return "User ID Exists";
    case FPC_RESULT_USER_ID_NOT_FOUND:
      return "User ID Not Found";
    case FPC_RESULT_STORAGE_IS_FULL:
      return "Storage Is Full";
    case FPC_RESULT_FLASH_ERROR:
      return "Flash Error";
    case FPC_RESULT_IDENTIFY_LOCKOUT:
      return "Identify Lockout";
    case FPC_RESULT_STORAGE_IS_EMPTY:
      return "Storage Is Empty";

    // IO Errors
    case FPC_RESULT_IO_BUSY:
      return "IO Busy";
    case FPC_RESULT_IO_RUNTIME_FAILURE:
      return "IO Runtime Failure";
    case FPC_RESULT_IO_BAD_DATA:
      return "IO Bad Data";
    case FPC_RESULT_IO_NOT_SUPPORTED:
      return "IO Not Supported";
    case FPC_RESULT_IO_NO_DATA:
      return "IO No Data";

    // Image Capture Errors
    case FPC_RESULT_COULD_NOT_ARM:
      return "Could Not Arm";
    case FPC_RESULT_CAPTURE_FAILED:
      return "Capture Failed";
    case FPC_RESULT_BAD_IMAGE_QUALITY:
      return "Bad Image Quality";
    case FPC_RESULT_NO_IMAGE:
      return "No Image";

    // Other Errors
    case FPC_RESULT_SENSOR_ERROR:
      return "Sensor Error";
    case FPC_RESULT_PROTOCOL_VERSION_ERROR:
      return "Protocol Version Error";
    case FPC_STARTUP_FAILURE:
      return "Startup Failure";

    default:
      return "Unknown Error";
  }
}
static const char *app_state_wait_str_(uint16_t app_state) {
  switch (app_state) {
    case APP_STATE_WAIT_READY:
      return "wait to be Ready";
    case APP_STATE_WAIT_VERSION:
      return "wait to read Version";
    case APP_STATE_WAIT_LIST_TEMPLATES:
      return "wait to list Templates";
    case APP_STATE_WAIT_ENROLL:
      return "wait for Enroll";
    case APP_STATE_WAIT_IDENTIFY:
      return "wait for Identify";
    case APP_STATE_WAIT_ABORT:
      return "wait for Abort";
    case APP_STATE_WAIT_DELETE_TEMPLATES:
      return "wait to Delete Templates";
    case APP_STATE_WAIT_CONFIG:
      return "wait to receive config";
    case APP_STATE_SET_CONFIG:
      return "wait to SET config";
  }
  return "app state Unknown";
}

void FingerprintFPC2532Component::update() {
  fpc::fpc_result_t result;
  size_t n = this->available();
  if (n) {
    ESP_LOGVV(TAG, "number of bytes available to read: %d", n);
    result = fpc_host_sample_handle_rx_data();
    if (result != FPC_RESULT_OK && result != FPC_PENDING_OPERATION) {
      ESP_LOGE(TAG, "Bad incoming data (%d). Wait and try again", result);
      this->fpc_hal_delay_ms(10);
      this->password_verified_ = false;
      this->app_state = APP_STATE_WAIT_READY;
      fpc_cmd_status_request();
    }
  } else {
    ESP_LOGVV(TAG, "No data available");
  }
  this->process_state();
}

void FingerprintFPC2532Component::setup() {
  this->hal_reset_device();
  this->fpc_hal_init();
  // this->fpc_cmd_abort();
  this->password_verified_ = false;
  this->device_ready_ = false;
  this->version_read_ = false;
  this->list_templates_done_ = false;
  this->device_state_ = 0;
  this->n_templates_on_device_ = 0;
  this->delay_until_ = 0;
  this->has_power_pin_ = (this->sensor_power_pin_ != nullptr);
  if (this->has_power_pin_) {
    this->sensor_power_pin_->digital_write(false);
    this->sensor_power_pin_->setup();
  }
  // If the user didn't specify an idle period to sleep, applies the default.

  if (this->enroll_timeout_s_sensor_ != nullptr) {
    this->enroll_timeout_s_sensor_->publish_state(this->enroll_timeout_ms_ / 1000);
  }
  if (this->enrolling_binary_sensor_ != nullptr) {
    this->enrolling_binary_sensor_->publish_state(false);
  }
  this->app_state = APP_STATE_WAIT_READY;
  this->fpc_cmd_status_request();
}

/*
------------------------
STATE MACHINE PROCESSING
------------------------
*/

bool FingerprintFPC2532Component::delay_elapsed(uint32_t duration_ms) {
  uint32_t now = millis();
  if (this->delay_until_ == 0) {
    this->delay_until_ = now + duration_ms;  // set target time
    return false;
  }
  if ((int32_t) (now - this->delay_until_) >= 0) {  // handle millis() overflow
    this->delay_until_ = 0;                         // reset for next delay
    return true;
  }
  return false;
}

void FingerprintFPC2532Component::process_state(void) {
  app_state_t next_state = app_state;

  switch (app_state) {
    case APP_STATE_WAIT_READY:
      ESP_LOGD(TAG, "APP_STATE_WAIT_READY");
      if (this->device_ready_) {
        if (this->delay_elapsed(5000)) {  // Wait for the device to be fully ready.
          next_state = APP_STATE_WAIT_VERSION;
          this->fpc_cmd_version_request();
          //  this->fpc_cmd_system_config_get_request(FPC_SYS_CFG_TYPE_DEFAULT);  // get current defaults
        }
      }
      break;
    case APP_STATE_WAIT_VERSION:
      ESP_LOGD(TAG, "APP_STATE_WAIT_VERSION");
      if (this->version_read_) {
        this->version_read_ = false;
        if (!this->password_verified_) {
          if (this->unique_id_ == this->password_) {
            ESP_LOGI(TAG, "Device authenticated.");
            this->password_verified_ = true;
          } else if (this->password_ == INITIAL_PASSWORD) {
            ESP_LOGW(TAG, "Device password must be reset. Add ID to YAML and reinstall.");
            this->password_verified_ = false;
            return;
          } else {
            ESP_LOGE(TAG, "DEVICE PASSWORD INCORRECT");
            this->password_verified_ = false;
            this->mark_failed();
          }
        }
        next_state = APP_STATE_WAIT_CONFIG;
        this->fpc_cmd_system_config_get_request(FPC_SYS_CFG_TYPE_CUSTOM);
      }
      break;
    case APP_STATE_WAIT_CONFIG:
      ESP_LOGD(TAG, "APP_STATE_WAIT_CONFIG");
      if (this->config_received) {
        ESP_LOGD(TAG, "CONFIG RECEIVED");
        ESP_LOGVV(TAG,
                  "System Config after GET:\n"
                  "  version                     = %u\n"
                  "  finger_scan_interval_ms     = %u\n"
                  "  sys_flags                   = 0x%08X\n"
                  "  uart_delay_before_irq_ms    = %u\n"
                  "  uart_baudrate               = %u\n"
                  "  idfy_max_consecutive_fails  = %u\n"
                  "  idfy_lockout_time_s         = %u\n"
                  "  idle_time_before_sleep_ms   = %u",
                  current_config_.version, current_config_.finger_scan_interval_ms, current_config_.sys_flags,
                  current_config_.uart_delay_before_irq_ms, current_config_.uart_baudrate,
                  current_config_.idfy_max_consecutive_fails, current_config_.idfy_lockout_time_s,
                  current_config_.idle_time_before_sleep_ms);

        if (this->uart_irq_before_tx_) {
          this->current_config_.sys_flags |= CFG_SYS_FLAG_UART_IRQ_BEFORE_TX;
        } else {
          this->current_config_.sys_flags &= ~CFG_SYS_FLAG_UART_IRQ_BEFORE_TX;
        }

        if (this->status_at_boot_) {
          this->current_config_.sys_flags |= CFG_SYS_FLAG_STATUS_EVT_AT_BOOT;
        } else {
          this->current_config_.sys_flags &= ~CFG_SYS_FLAG_STATUS_EVT_AT_BOOT;
        }

        if (this->stop_mode_uart_ && has_power_pin_) {
          this->current_config_.sys_flags |= CFG_SYS_FLAG_UART_IN_STOP_MODE;
        } else if (this->stop_mode_uart_ && !has_power_pin_) {
          ESP_LOGW(TAG, "No power_pin configured for waking up the device: setting not available");
          this->current_config_.sys_flags |= CFG_SYS_FLAG_UART_IN_STOP_MODE;
        } else {
          this->current_config_.sys_flags &= ~CFG_SYS_FLAG_UART_IN_STOP_MODE;
        }

        this->current_config_.idfy_lockout_time_s = this->lockout_time_s_;
        this->current_config_.uart_baudrate = this->uart_baudrate_;
        this->current_config_.idfy_max_consecutive_fails = this->max_consecutive_fails_;
        this->current_config_.idle_time_before_sleep_ms = this->time_before_sleep_ms_;
        this->current_config_.uart_delay_before_irq_ms = this->delay_before_irq_ms_;
        this->current_config_.finger_scan_interval_ms = this->finger_scan_interval_ms_;

        if (prev_state == APP_STATE_WAIT_VERSION) {
          next_state = APP_STATE_SET_CONFIG;
          this->fpc_cmd_system_config_set_request(&this->current_config_);
        } else if (prev_state == APP_STATE_SET_CONFIG) {
          next_state = APP_STATE_WAIT_LIST_TEMPLATES;
          this->fpc_cmd_list_templates_request();
        }
      }
      break;

    case APP_STATE_WAIT_LIST_TEMPLATES:
      ESP_LOGI(TAG, "APP_STATE_WAIT_LIST_TEMPLATES");
      if (this->list_templates_done_) {
        this->list_templates_done_ = false;
        if (this->n_templates_on_device_ == MAX_NUMBER_OF_TEMPLATES) {
          ESP_LOGW(TAG, "No space for new fingerprints. Consider deleting unused templates.");
          fpc::fpc_id_type_t id_type = {ID_TYPE_ALL, 0};
          ESP_LOGI(TAG, "Starting identify");
          next_state = APP_STATE_WAIT_IDENTIFY;
          this->fpc_cmd_identify_request(&id_type, 0);
        } else if (this->n_templates_on_device_ == 0) {
          fpc::fpc_id_type_t id_type = {ID_TYPE_GENERATE_NEW, 0};
          ESP_LOGI(TAG, "Starting enroll");
          next_state = APP_STATE_WAIT_ENROLL;
          this->fpc_cmd_enroll_request(&id_type);
        } else {
          fpc::fpc_id_type_t id_type = {ID_TYPE_ALL, 0};
          ESP_LOGI(TAG, "Starting identify");
          next_state = APP_STATE_WAIT_IDENTIFY;
          this->fpc_cmd_identify_request(&id_type, 0);
        }
      }
      break;

    case APP_STATE_WAIT_ENROLL: {
      if (millis() - this->enroll_idle_time_ > this->enroll_timeout_ms_) {
        ESP_LOGW(TAG, "Enroll timeout. Aborting operation.");
        this->enrollment_failed_callback_.call(0);
        fpc_cmd_abort();
        ESP_LOGI(TAG, "Aborting operation");
        next_state = APP_STATE_WAIT_ABORT;
        break;
      }

      if (!this->enroll_status_received_ && (this->device_ready_) && (this->device_state_ & STATE_ENROLL)) {
        this->enroll_status_received_ = true;
        ESP_LOGD(TAG, "Enrollment acknowledged by firmware.");
      }

      if (this->enroll_status_received_ && ((this->device_state_ & STATE_ENROLL) == 0)) {
        ESP_LOGI(TAG, "Finger Enrollment done.");
        this->fpc_cmd_list_templates_request();
        next_state = APP_STATE_WAIT_LIST_TEMPLATES;
        this->enroll_status_received_ = false;
      }

      break;
    }

    case APP_STATE_WAIT_IDENTIFY:
      if (this->device_ready_ && ((this->device_state_ & STATE_IDENTIFY) == 0)) {
        fpc::fpc_id_type_t id_type = {ID_TYPE_ALL, 0};
        if (this->delay_elapsed(300)) {
          this->fpc_cmd_identify_request(&id_type, 0);
        }
      }
      break;
    case APP_STATE_WAIT_ABORT:
      ESP_LOGI(TAG, "Aborting current operation..");
      if (this->device_ready_ && ((this->device_state_ & (STATE_ENROLL | STATE_IDENTIFY)) == 0)) {
        ESP_LOGI(TAG, "Operation aborted");
        enroll_status_received_ = false;
        if (this->enroll_request == true) {
          fpc::fpc_id_type_t id_type = this->id_type_enroll_request;
          ESP_LOGI(TAG, "Starting enroll");
          next_state = APP_STATE_WAIT_ENROLL;
          this->fpc_cmd_enroll_request(&id_type);
          this->enroll_request = false;
        } else if (this->delete_request == true) {
          fpc::fpc_id_type_t id_type = this->id_type_delete_request;
          ESP_LOGI(TAG, "Starting delete templates");
          next_state = APP_STATE_WAIT_DELETE_TEMPLATES;
          this->fpc_cmd_delete_template_request(&id_type);
          this->delete_request = false;
        } else {
          if (this->enrolling_binary_sensor_ != nullptr) {
            this->enrolling_binary_sensor_->publish_state(false);
          }
          fpc::fpc_id_type_t id_type = {ID_TYPE_ALL, 0};
          ESP_LOGI(TAG, "Starting identify");
          next_state = APP_STATE_WAIT_IDENTIFY;
          this->fpc_cmd_identify_request(&id_type, 0);
        }
      }
      break;
    case APP_STATE_WAIT_DELETE_TEMPLATES: {
      if (this->device_ready_) {
        ESP_LOGI(TAG, "template/s deleted.");
        this->fpc_hal_delay_ms(20);
        next_state = APP_STATE_WAIT_LIST_TEMPLATES;
        this->fpc_cmd_list_templates_request();
      }
      break;
    }
    case APP_STATE_SET_CONFIG:
      ESP_LOGI(TAG,
               "System Config in after SET:\n"
               "  version                     = %u\n"
               "  finger_scan_interval_ms     = %u\n"
               "  sys_flags                   = 0x%08X\n"
               "  uart_delay_before_irq_ms    = %u\n"
               "  uart_baudrate               = %u\n"
               "  idfy_max_consecutive_fails  = %u\n"
               "  idfy_lockout_time_s         = %u\n"
               "  idle_time_before_sleep_ms   = %u",
               current_config_.version, current_config_.finger_scan_interval_ms, current_config_.sys_flags,
               current_config_.uart_delay_before_irq_ms, current_config_.uart_baudrate,
               current_config_.idfy_max_consecutive_fails, current_config_.idfy_lockout_time_s,
               current_config_.idle_time_before_sleep_ms);
      if (this->device_ready_) {
        // this->status_at_boot = false;
        next_state = APP_STATE_WAIT_CONFIG;
        this->config_received = false;
        this->config_set = false;
        this->fpc_cmd_system_config_get_request(FPC_SYS_CFG_TYPE_CUSTOM);
      }
      break;

    default:
      break;
  }

  if (next_state != app_state) {
    this->prev_state = this->app_state;
    ESP_LOGI(TAG, "State transition %d -> %d\n", app_state, next_state);
    this->app_state = next_state;
  }
}

/*
------------------------
HOST FUNCTIONS DEFINITONS
------------------------
*/

void FingerprintFPC2532Component::sensor_wakeup_() {
  // Immediately return if there is no power pin or the sensor is already on
  if ((!this->has_power_pin_))
    return;

  this->sensor_power_pin_->digital_write(true);
  delay(1);  // datasheet set min 500uS to wake up device
  this->sensor_power_pin_->digital_write(false);
}

/* Command Requests */
fpc::fpc_result_t FingerprintFPC2532Component::fpc_send_request(fpc::fpc_cmd_hdr_t *cmd, size_t size) {
  fpc::fpc_result_t result = FPC_RESULT_OK;
  fpc::fpc_frame_hdr_t frame = {0};

  if (!cmd) {
    ESP_LOGE(TAG, "Invalid command");
    result = FPC_RESULT_INVALID_PARAM;
  }

  if (result == FPC_RESULT_OK) {
    frame.version = FPC_FRAME_PROTOCOL_VERSION;
    frame.type = FPC_FRAME_TYPE_CMD_REQUEST;
    frame.flags = FPC_FRAME_FLAG_SENDER_HOST;
    frame.payload_size = (uint16_t) size;

    /* Send frame header. */
    result = this->fpc_hal_tx((uint8_t *) &frame, sizeof(fpc::fpc_frame_hdr_t));
    ESP_LOGVV(TAG, "frame header sent: version=%02X, flags=%02X, type=%02X, payload_size=%" PRIu32, frame.version,
              frame.flags, frame.type, frame.payload_size);
  }

  if (result == FPC_RESULT_OK) {
    sensor_wakeup_();
    /* Send payload. */
    result = this->fpc_hal_tx((uint8_t *) cmd, size);
    ESP_LOGVV(TAG, "command payload sent");
  }

  const uint32_t start = millis();
  const uint32_t timeout_ms = 100;
  if (result == FPC_RESULT_OK) {
    while (!available()) {
      ESP_LOGV(TAG, "waiting time in while loop waiting command feedback %u", millis() - start);
      if (millis() - start > timeout_ms) {
        ESP_LOGE(TAG, "no feedback from sensor available (timeout)");
        return FPC_RESULT_TIMEOUT;
      }
      delay(1);
    }
    ESP_LOGVV(TAG, "packet sent and sensor feedback available");
    result = FPC_RESULT_OK;
  }
  return result;
}
fpc::fpc_result_t FingerprintFPC2532Component::fpc_cmd_status_request(void) {
  fpc::fpc_result_t result = FPC_RESULT_OK;
  fpc::fpc_cmd_hdr_t cmd;

  /* Status Command Request has no payload */
  cmd.cmd_id = CMD_STATUS;
  cmd.type = FPC_FRAME_TYPE_CMD_REQUEST;

  ESP_LOGI(TAG, ">>> Command Status Request");
  result = this->fpc_send_request(&cmd, sizeof(fpc::fpc_cmd_hdr_t));

  return result;
}
fpc::fpc_result_t FingerprintFPC2532Component::fpc_cmd_version_request(void) {
  fpc::fpc_result_t result = FPC_RESULT_OK;
  fpc::fpc_cmd_hdr_t cmd;

  /* Version Command Request has no payload */
  cmd.cmd_id = CMD_VERSION;
  cmd.type = FPC_FRAME_TYPE_CMD_REQUEST;

  ESP_LOGI(TAG, ">>> CMD_VERSION");
  result = this->fpc_send_request(&cmd, sizeof(fpc::fpc_cmd_hdr_t));

  return result;
}
fpc::fpc_result_t FingerprintFPC2532Component::fpc_cmd_enroll_request(fpc::fpc_id_type_t *id) {
  fpc::fpc_result_t result = FPC_RESULT_OK;
  fpc::fpc_cmd_enroll_request_t cmd_req;
  this->device_ready_ = false;

  if (!this->password_verified_) {
    ESP_LOGE(TAG, "Enroll Request: Password not verified");
    result = FPC_RESULT_FAILURE;
  }

  if (id->type != ID_TYPE_SPECIFIED && id->type != ID_TYPE_GENERATE_NEW) {
    ESP_LOGE(TAG, "Enroll Request: Invalid parameter");
    result = FPC_RESULT_INVALID_PARAM;
  }

  if (result == FPC_RESULT_OK) {
    cmd_req.cmd.cmd_id = CMD_ENROLL;
    cmd_req.cmd.type = FPC_FRAME_TYPE_CMD_REQUEST;

    cmd_req.tpl_id.type = id->type;
    cmd_req.tpl_id.id = id->id;

    ESP_LOGI(TAG, ">>> CMD_ENROLL (id.type=%s, id=%d)", get_id_type_str_(id->type), id->id);

    result = fpc_send_request(&cmd_req.cmd, sizeof(fpc::fpc_cmd_enroll_request_t));
    this->enroll_idle_time_ = millis();
  }

  return result;
}
fpc::fpc_result_t FingerprintFPC2532Component::fpc_cmd_identify_request(fpc::fpc_id_type_t *id, uint16_t tag) {
  fpc::fpc_result_t result = FPC_RESULT_OK;
  fpc::fpc_cmd_identify_request_t cmd_req;

  if (!this->password_verified_) {
    ESP_LOGE(TAG, "Identify Request: Password not verified");
    result = FPC_RESULT_FAILURE;
  }

  if (id->type != ID_TYPE_SPECIFIED && id->type != ID_TYPE_ALL) {
    ESP_LOGE(TAG, "Identify: Invalid parameter");
    result = FPC_RESULT_INVALID_PARAM;
  }

  if (result == FPC_RESULT_OK) {
    cmd_req.cmd.cmd_id = CMD_IDENTIFY;
    cmd_req.cmd.type = FPC_FRAME_TYPE_CMD_REQUEST;

    cmd_req.tpl_id.type = id->type;
    cmd_req.tpl_id.id = id->id;
    cmd_req.tag = tag;

    ESP_LOGI(TAG, ">>> CMD_IDENTIFY (tag=%d, id.type=%s, id=%d)", tag, get_id_type_str_(id->type), id->id);

    result = fpc_send_request(&cmd_req.cmd, sizeof(fpc::fpc_cmd_identify_request_t));
  }

  return result;
}
fpc::fpc_result_t FingerprintFPC2532Component::fpc_cmd_abort(void) {
  fpc::fpc_result_t result = FPC_RESULT_OK;
  fpc::fpc_cmd_hdr_t cmd;

  /* Abort Command Request has no payload */
  cmd.cmd_id = CMD_ABORT;
  cmd.type = FPC_FRAME_TYPE_CMD_REQUEST;

  ESP_LOGI(TAG, ">>> CMD_ABORT");
  result = fpc_send_request(&cmd, sizeof(fpc::fpc_cmd_hdr_t));

  return result;
}
fpc::fpc_result_t FingerprintFPC2532Component::fpc_cmd_list_templates_request(void) {
  fpc::fpc_result_t result = FPC_RESULT_OK;
  fpc::fpc_cmd_hdr_t cmd;
  if (!this->password_verified_) {
    ESP_LOGE(TAG, "List Template Request: Password not verified");
    result = FPC_RESULT_FAILURE;
  }
  /* List Template Command Request has no payload */
  cmd.cmd_id = CMD_LIST_TEMPLATES;
  cmd.type = FPC_FRAME_TYPE_CMD_REQUEST;

  ESP_LOGI(TAG, ">>> CMD_LIST_TEMPLATES");
  result = fpc_send_request(&cmd, sizeof(fpc::fpc_cmd_hdr_t));

  return result;
}
fpc::fpc_result_t FingerprintFPC2532Component::fpc_cmd_delete_template_request(fpc::fpc_id_type_t *id) {
  fpc::fpc_result_t result = FPC_RESULT_OK;
  fpc::fpc_cmd_template_delete_request_t cmd_req;

  if (id->type != ID_TYPE_SPECIFIED && id->type != ID_TYPE_ALL) {
    ESP_LOGE(TAG, "Delete Tpl: Invalid parameter");
    result = FPC_RESULT_INVALID_PARAM;
  }

  if (result == FPC_RESULT_OK) {
    cmd_req.cmd.cmd_id = CMD_DELETE_TEMPLATE;
    cmd_req.cmd.type = FPC_FRAME_TYPE_CMD_REQUEST;

    cmd_req.tpl_id.type = id->type;
    cmd_req.tpl_id.id = id->id;

    ESP_LOGI(TAG, ">>> CMD_DELETE_TEMPLATE (id.type=%s, id=%d)", get_id_type_str_(id->type), id->id);

    result = fpc_send_request(&cmd_req.cmd, sizeof(fpc::fpc_cmd_template_delete_request_t));
  }

  return result;
}
fpc::fpc_result_t FingerprintFPC2532Component::fpc_cmd_reset_request(void) {
  fpc::fpc_result_t result = FPC_RESULT_OK;
  fpc::fpc_cmd_hdr_t cmd;

  /* Reset Command Request has no payload */
  cmd.cmd_id = CMD_RESET;
  cmd.type = FPC_FRAME_TYPE_CMD_REQUEST;

  ESP_LOGI(TAG, ">>> CMD_RESET");
  result = fpc_send_request(&cmd, sizeof(fpc::fpc_cmd_hdr_t));

  return result;
}
fpc::fpc_result_t FingerprintFPC2532Component::fpc_cmd_system_config_set_request(fpc::fpc_system_config_t *cfg) {
  fpc::fpc_result_t result = FPC_RESULT_OK;
  fpc::fpc_cmd_set_config_request_t cmd_req;

  if (cfg == NULL) {
    ESP_LOGE(TAG, "Set System Config: Invalid parameter");
    result = FPC_RESULT_INVALID_PARAM;
  }

  if (result == FPC_RESULT_OK) {
    cmd_req.cmd.cmd_id = CMD_SET_SYSTEM_CONFIG;
    cmd_req.cmd.type = FPC_FRAME_TYPE_CMD_REQUEST;

    cmd_req.cfg = *cfg;

    ESP_LOGI(TAG, ">>> CMD_SET_SYSTEM_CONFIG");
    result = fpc_send_request(&cmd_req.cmd, sizeof(fpc::fpc_cmd_set_config_request_t));
  }

  return result;
}
fpc::fpc_result_t FingerprintFPC2532Component::fpc_cmd_system_config_get_request(uint8_t type) {
  fpc::fpc_result_t result = FPC_RESULT_OK;
  fpc::fpc_cmd_get_config_request_t cmd_req;

  if (type > FPC_SYS_CFG_TYPE_CUSTOM) {
    ESP_LOGE(TAG, "Get System Config: Invalid parameter");
    result = FPC_RESULT_INVALID_PARAM;
  }

  if (result == FPC_RESULT_OK) {
    cmd_req.cmd.cmd_id = CMD_GET_SYSTEM_CONFIG;
    cmd_req.cmd.type = FPC_FRAME_TYPE_CMD_REQUEST;
    cmd_req.config_type = type;

    ESP_LOGI(TAG, ">>> CMD_GET_SYSTEM_CONFIG (type=%d)", type);
    result = fpc_send_request(&cmd_req.cmd, sizeof(fpc::fpc_cmd_get_config_request_t));
  }

  return result;
}

/* Command Responses / Events */
fpc::fpc_result_t FingerprintFPC2532Component::fpc_host_sample_handle_rx_data(void) {
  fpc::fpc_result_t result;
  fpc::fpc_frame_hdr_t frame_hdr;
  // std::vector<uint8_t> frame_payload;
  uint8_t *frame_payload = NULL;

  /* Step 1: Read Frame Header */
  result = this->fpc_hal_rx((uint8_t *) &frame_hdr, sizeof(fpc::fpc_frame_hdr_t));

  if (result == FPC_RESULT_OK) {
    ESP_LOGVV(TAG, "Sanity check started");
    /* Sanity Check */
    if (frame_hdr.version != FPC_FRAME_PROTOCOL_VERSION || ((frame_hdr.flags & FPC_FRAME_FLAG_SENDER_FW_APP) == 0) ||
        (frame_hdr.type != FPC_FRAME_TYPE_CMD_RESPONSE && frame_hdr.type != FPC_FRAME_TYPE_CMD_EVENT)) {
      ESP_LOGE(TAG, "Sanity check of rx data failed");
      result = FPC_RESULT_IO_BAD_DATA;
    } else {
      ESP_LOGVV(TAG, "Received Header frame: version=%02X, flags=%02X, type=%02X, payload_size=%" PRIu32,
                frame_hdr.version, frame_hdr.flags, frame_hdr.type, frame_hdr.payload_size);
    }
  }

  if (result == FPC_RESULT_OK) {
    frame_payload = static_cast<uint8_t *>(malloc(frame_hdr.payload_size));
    if (!frame_payload) {
      ESP_LOGE(TAG, "Failed to malloc");
      result = FPC_RESULT_OUT_OF_MEMORY;
    }
  }

  if (result == FPC_RESULT_OK) {
    /* Step 2: Read Frame Payload (Command) */
    result = this->fpc_hal_rx(frame_payload, frame_hdr.payload_size);
  }

  if (result == FPC_RESULT_OK) {
    result = parse_cmd(frame_payload, frame_hdr.payload_size);
  }

  if (frame_payload) {
    free(frame_payload);
  }

  if (result != FPC_RESULT_OK) {
    ESP_LOGE(TAG, "Failed to handle RX data, error %d", result);
  }

  return result;
}
fpc::fpc_result_t FingerprintFPC2532Component::parse_cmd(uint8_t *frame_payload, std::size_t size) {
  fpc::fpc_result_t result = FPC_RESULT_OK;
  fpc::fpc_cmd_hdr_t *cmd_hdr;

  cmd_hdr = (fpc::fpc_cmd_hdr_t *) frame_payload;

  if (!cmd_hdr) {
    ESP_LOGE(TAG, "Parse Cmd: Invalid parameter");
    result = FPC_RESULT_INVALID_PARAM;
  }

  if (result == FPC_RESULT_OK) {
    if (cmd_hdr->type != FPC_FRAME_TYPE_CMD_EVENT && cmd_hdr->type != FPC_FRAME_TYPE_CMD_RESPONSE) {
      ESP_LOGE(TAG, "Parse Cmd: Invalid parameter (type)");
      result = FPC_RESULT_INVALID_PARAM;
    }
  }

  if (result == FPC_RESULT_OK) {
    switch (cmd_hdr->cmd_id) {
      case CMD_STATUS:
        return parse_cmd_status(cmd_hdr, size);
        break;
      case CMD_VERSION:
        return parse_cmd_version(cmd_hdr, size);
        break;
      case CMD_ENROLL:
        return parse_cmd_enroll_status(cmd_hdr, size);
        break;
      case CMD_IDENTIFY:
        return parse_cmd_identify(cmd_hdr, size);
        break;
      case CMD_LIST_TEMPLATES:
        return parse_cmd_list_templates(cmd_hdr, size);
        break;
      /*
      case CMD_NAVIGATION:
        return parse_cmd_navigation_event(cmd_hdr, size);
        break;
      */
      case CMD_GET_SYSTEM_CONFIG:
        return parse_cmd_get_system_config(cmd_hdr, size);
        break;
      /*
      case CMD_BIST:
        return parse_cmd_bist(cmd_hdr, size);
        break;
       */
      default:
        ESP_LOGE(TAG, "Parse Cmd: Unexpected Command ID");
        break;
    };
  }

  return result;
}
fpc::fpc_result_t FingerprintFPC2532Component::parse_cmd_status(fpc::fpc_cmd_hdr_t *cmd_hdr, std::size_t size) {
  fpc::fpc_result_t result = FPC_RESULT_OK;
  fpc::fpc_cmd_status_response_t *status;
  status = (fpc::fpc_cmd_status_response_t *) cmd_hdr;
  ESP_LOGI(TAG, "<<< Command Status Response");

  if (!status) {
    ESP_LOGE(TAG, "CMD_STATUS: Invalid parameter");
    result = FPC_RESULT_INVALID_PARAM;
  }

  if (result == FPC_RESULT_OK) {
    if (size != sizeof(fpc::fpc_cmd_status_response_t)) {
      ESP_LOGE(TAG, "CMD_STATUS invalid size (%d vs %d)", size, sizeof(fpc::fpc_cmd_status_response_t));
      result = FPC_RESULT_INVALID_PARAM;
    }
  }

  if (result == FPC_RESULT_OK) {
    this->device_state_ = status->state;
    ESP_LOGI(TAG, "CMD_STATUS.event = %s (%04X)", get_event_str_(status->event), status->event);
    ESP_LOGI(TAG, "CMD_STATUS.state = %s (%04X)", get_state_str_(status->state).c_str(), status->state);
    ESP_LOGI(TAG, "CMD_STATUS.error = %s (%d)", fpc_result_to_string(status->app_fail_code), status->app_fail_code);
    if (this->status_sensor_ != nullptr) {
      this->status_sensor_->publish_state(uint16_t(status->state));
    }
    if (this->text_status_sensor_ != nullptr) {
      this->text_status_sensor_->publish_state(get_state_str_(status->state));
    }
    if (status->state & STATE_ENROLL) {
      if (status->state & STATE_APP_FW_READY && (status->event == EVENT_NONE)) {
        enrollment_scan_callback_.call(enroll_id);
        if (this->enrolling_binary_sensor_ != nullptr) {
          this->enrolling_binary_sensor_->publish_state(true);
        }
      }
      if (status->state & STATE_FINGER_DOWN) {
        this->enroll_idle_time_ = millis();
      }
      if (status->app_fail_code != 0) {
        this->enrollment_failed_callback_.call(enroll_id);
        if (this->enrolling_binary_sensor_ != nullptr) {
          this->enrolling_binary_sensor_->publish_state(false);
        }
      }
    }
    if (status->state & STATE_APP_FW_READY) {
      this->device_ready_ = true;
      ESP_LOGV(TAG, "device ready");
    } else {
      this->device_ready_ = false;
      ESP_LOGV(TAG, "device not ready");
    }
    if ((this->device_state_ & STATE_FINGER_DOWN) &&
        (this->device_state_ & (STATE_IDENTIFY | STATE_ENROLL | STATE_NAVIGATION))) {
      this->finger_scan_start_callback_.call();
    }
    if ((this->device_state_ & STATE_IDENTIFY) && (status->app_fail_code != FPC_RESULT_OK)) {
      this->finger_scan_invalid_callback_.call(status->app_fail_code);
    }
  }
  return result;
}

fpc::fpc_result_t FingerprintFPC2532Component::parse_cmd_version(fpc::fpc_cmd_hdr_t *cmd_hdr, size_t size) {
  fpc::fpc_result_t result = FPC_RESULT_OK;
  fpc::fpc_cmd_version_response_t *ver;
  size_t full_size = 0;

  ver = (fpc::fpc_cmd_version_response_t *) cmd_hdr;

  if (!ver) {
    ESP_LOGE(TAG, "CMD_VERSION: Invalid parameter");
    result = FPC_RESULT_INVALID_PARAM;
  }

  if (result == FPC_RESULT_OK) {
    /* The full size of the command must include the length of the
      version string (unset array) */
    full_size = sizeof(fpc::fpc_cmd_version_response_t) + ver->version_str_len;

    if (size != full_size) {
      ESP_LOGE(TAG, "CMD_VERSION invalid size (%d vs %d)", size, full_size);
      result = FPC_RESULT_INVALID_PARAM;
    }
  }

  if (result == FPC_RESULT_OK) {
    ESP_LOGI(TAG, "CMD_VERSION.fw_id = %d", ver->fw_id);
    ESP_LOGI(TAG, "CMD_VERSION.unique_id = %08X %08X %08X", ver->mcu_unique_id[0], ver->mcu_unique_id[1],
             ver->mcu_unique_id[2]);
    ESP_LOGI(TAG, "CMD_VERSION.fuse_level = %d", ver->fw_fuse_level);
    ESP_LOGI(TAG, "CMD_VERSION.version_str_len = %d", ver->version_str_len);
    ESP_LOGI(TAG, "CMD_VERSION.version = %s", ver->version_str);
    this->version_read_ = true;
    char buf[25];
    snprintf(buf, sizeof(buf), "%08X%08X%08X", ver->mcu_unique_id[0], ver->mcu_unique_id[1], ver->mcu_unique_id[2]);
    this->unique_id_ = std::string(buf);
    if (this->unique_id_sensor_ != nullptr) {
      this->unique_id_sensor_->publish_state(this->unique_id_);
    }

    if (this->version_sensor_ != nullptr) {
      this->version_sensor_->publish_state(ver->version_str);
    }
  }

  return result;
}

fpc::fpc_result_t FingerprintFPC2532Component::parse_cmd_enroll_status(fpc::fpc_cmd_hdr_t *cmd_hdr, size_t size) {
  fpc::fpc_result_t result = FPC_RESULT_OK;
  fpc::fpc_cmd_enroll_status_response_t *status;

  status = (fpc::fpc_cmd_enroll_status_response_t *) cmd_hdr;

  if (!status) {
    ESP_LOGE(TAG, "CMD_ENROLL: Invalid parameter");
    result = FPC_RESULT_INVALID_PARAM;
  }

  if (result == FPC_RESULT_OK) {
    if (size != sizeof(fpc::fpc_cmd_enroll_status_response_t)) {
      ESP_LOGE(TAG, "CMD_ENROLL invalid size (%d vs %d)", size, sizeof(fpc::fpc_cmd_enroll_status_response_t));
      result = FPC_RESULT_INVALID_PARAM;
    }
  }

  if (result == FPC_RESULT_OK) {
    uint16_t enroll_id = status->id;
    ESP_LOGI(TAG, "CMD_ENROLL.id = %d", status->id);
    ESP_LOGI(TAG, "CMD_ENROLL.feedback = %s", get_enroll_feedback_str_(status->feedback));
    ESP_LOGI(TAG, "CMD_ENROLL.samples_remaining = %d", status->samples_remaining);

    if (this->enrollment_feedback_ != nullptr) {
      this->enrollment_feedback_->publish_state((uint8_t) status->feedback);
    }
    if (this->num_scans_ != nullptr) {
      this->num_scans_->publish_state((uint8_t) status->samples_remaining);
    }
  }

  if (status->feedback == ENROLL_FEEDBACK_REJECT_LOW_QUALITY ||
      status->feedback == ENROLL_FEEDBACK_REJECT_LOW_COVERAGE ||
      status->feedback == ENROLL_FEEDBACK_REJECT_LOW_MOBILITY || status->feedback == ENROLL_FEEDBACK_REJECT_OTHER) {
    this->finger_scan_invalid_callback_.call(status->feedback);
  }

  if (status->feedback == ENROLL_FEEDBACK_DONE) {
    this->enrollment_done_callback_.call(enroll_id);
    this->fpc_cmd_list_templates_request();
    this->app_state = APP_STATE_WAIT_LIST_TEMPLATES;
    if (this->enrolling_binary_sensor_ != nullptr) {
      this->enrolling_binary_sensor_->publish_state(false);
    }
  }

  return result;
}

fpc::fpc_result_t FingerprintFPC2532Component::parse_cmd_identify(fpc::fpc_cmd_hdr_t *cmd_hdr, size_t size) {
  fpc::fpc_result_t result = FPC_RESULT_OK;
  fpc::fpc_cmd_identify_status_response_t *id_res;

  id_res = (fpc::fpc_cmd_identify_status_response_t *) cmd_hdr;
  uint16_t finger_id = 0;
  uint16_t tag = 0;

  if (!id_res) {
    ESP_LOGE(TAG, "CMD_IDENTIFY: Invalid parameter");
    result = FPC_RESULT_INVALID_PARAM;
  }

  if (result == FPC_RESULT_OK) {
    if (size != sizeof(fpc::fpc_cmd_identify_status_response_t)) {
      ESP_LOGE(TAG, "CMD_IDENTIFY invalid size (%d vs %d)", size, sizeof(fpc::fpc_cmd_identify_status_response_t));
      result = FPC_RESULT_INVALID_PARAM;
    }
  }

  if (result == FPC_RESULT_OK) {
    ESP_LOGI(TAG, "CMD_IDENTIFY.result = %s (0x%04X)", (id_res->match == IDENTIFY_RESULT_MATCH) ? "MATCH" : "No Match",
             id_res->match);
    ESP_LOGI(TAG, "CMD_IDENTIFY.id_type = %s", get_id_type_str_(id_res->tpl_id.type));
    ESP_LOGI(TAG, "CMD_IDENTIFY.id = %d", id_res->tpl_id.id);
    ESP_LOGI(TAG, "CMD_IDENTIFY.tag = %d", id_res->tag);
    finger_id = id_res->tpl_id.id;
    tag = id_res->tag;
  }

  if (id_res->match == IDENTIFY_RESULT_MATCH && this->last_finger_id_sensor_ != nullptr) {
    this->last_finger_id_sensor_->publish_state(id_res->tpl_id.id);
  }

  if (id_res->match == IDENTIFY_RESULT_MATCH) {
    this->finger_scan_matched_callback_.call(finger_id, tag);
  }
  if (id_res->match == IDENTIFY_RESULT_NO_MATCH) {
    this->finger_scan_unmatched_callback_.call();
  }

  return result;
}

fpc::fpc_result_t FingerprintFPC2532Component::parse_cmd_list_templates(fpc::fpc_cmd_hdr_t *cmd_hdr, size_t size) {
  fpc::fpc_result_t result = FPC_RESULT_OK;
  fpc::fpc_cmd_template_info_response_t *list;
  size_t total_pl_size = 0;
  uint16_t i;

  list = (fpc::fpc_cmd_template_info_response_t *) cmd_hdr;

  if (!list) {
    ESP_LOGE(TAG, "CMD_LIST_TEMPLATES: Invalid parameter");
    result = FPC_RESULT_INVALID_PARAM;
  }

  if (result == FPC_RESULT_OK) {
    total_pl_size = sizeof(fpc::fpc_cmd_template_info_response_t) + (sizeof(uint16_t) * list->number_of_templates);

    if (size != total_pl_size) {
      ESP_LOGE(TAG, "CMD_LIST_TEMPLATES invalid size (%d vs %d)", size, total_pl_size);
      result = FPC_RESULT_INVALID_PARAM;
    }
  }

  if (result == FPC_RESULT_OK) {
    ESP_LOGI(TAG, "CMD_LIST_TEMPLATES.nbr_of_tpls = %d", list->number_of_templates);

    for (i = 0; i < list->number_of_templates; i++) {
      ESP_LOGI(TAG, "CMD_LIST_TEMPLATES.id = %d", list->template_id_list[i]);
    }
    this->list_templates_done_ = true;
    this->n_templates_on_device_ = list->number_of_templates;
    if (this->fingerprint_count_sensor_ != nullptr) {
      this->fingerprint_count_sensor_->publish_state((uint8_t) this->n_templates_on_device_);
    }
  }

  return result;
}

fpc::fpc_result_t FingerprintFPC2532Component::parse_cmd_get_system_config(fpc::fpc_cmd_hdr_t *cmd_hdr, size_t size) {
  fpc::fpc_result_t result = FPC_RESULT_OK;
  fpc::fpc_cmd_get_config_response_t *cmd_cfg = (fpc::fpc_cmd_get_config_response_t *) cmd_hdr;

  if (!cmd_cfg) {
    ESP_LOGE(TAG, "CMD_GET_SYSTEM_CONFIG: Invalid parameter");
    result = FPC_RESULT_INVALID_PARAM;
  }

  if (result == FPC_RESULT_OK) {
    if (size < sizeof(fpc::fpc_cmd_get_config_response_t)) {
      ESP_LOGE(TAG, "CMD_GET_SYSTEM_CONFIG invalid size (%d vs %d)", size, sizeof(fpc::fpc_cmd_get_config_response_t));
      result = FPC_RESULT_INVALID_PARAM;
    }
  }

  if (result == FPC_RESULT_OK) {
    ESP_LOGV(TAG, "%s Config:", cmd_cfg->config_type == 0 ? "Default" : "Custom");
    ESP_LOGV(TAG, "CMD_GET_SYSTEM_CONFIG.ver = %d", cmd_cfg->cfg.version);
    ESP_LOGV(TAG, "CMD_GET_SYSTEM_CONFIG.sys_flags = %08X:", cmd_cfg->cfg.sys_flags);
    if (cmd_cfg->cfg.sys_flags & CFG_SYS_FLAG_STATUS_EVT_AT_BOOT)
      ESP_LOGV(TAG, " - CFG_SYS_FLAG_STATUS_EVT_AT_BOOT");
    if (cmd_cfg->cfg.sys_flags & CFG_SYS_FLAG_UART_IN_STOP_MODE)
      ESP_LOGV(TAG, " - CFG_SYS_FLAG_UART_IN_STOP_MODE");
    if (cmd_cfg->cfg.sys_flags & CFG_SYS_FLAG_UART_IRQ_BEFORE_TX)
      ESP_LOGV(TAG, " - CFG_SYS_FLAG_UART_IRQ_BEFORE_TX");
    ESP_LOGV(TAG, "CMD_GET_SYSTEM_CONFIG.uart_irq_dly = %d ms", cmd_cfg->cfg.uart_delay_before_irq_ms);
    ESP_LOGV(TAG, "CMD_GET_SYSTEM_CONFIG.uart_baudrate_idx = %d", cmd_cfg->cfg.uart_baudrate);
    ESP_LOGV(TAG, "CMD_GET_SYSTEM_CONFIG.finger_scan_intv = %d ms", cmd_cfg->cfg.finger_scan_interval_ms);
    ESP_LOGV(TAG, "CMD_GET_SYSTEM_CONFIG.idfy_max_consecutive_fails = %d", cmd_cfg->cfg.idfy_max_consecutive_fails);
    ESP_LOGV(TAG, "CMD_GET_SYSTEM_CONFIG.idfy_lockout_time_s = %d s", cmd_cfg->cfg.idfy_lockout_time_s);
    ESP_LOGV(TAG, "CMD_GET_SYSTEM_CONFIG.idle_time_before_sleep_ms = %d ms", cmd_cfg->cfg.idle_time_before_sleep_ms);

    if (this->scan_interval_ms_sensor_ != nullptr)
      this->scan_interval_ms_sensor_->publish_state(cmd_cfg->cfg.finger_scan_interval_ms);

    if (this->uart_dly_before_tx_ms_sensor_ != nullptr)
      this->uart_dly_before_tx_ms_sensor_->publish_state(cmd_cfg->cfg.uart_delay_before_irq_ms);

    if (this->idle_time_before_sleep_ms_sensor_ != nullptr)
      this->idle_time_before_sleep_ms_sensor_->publish_state(cmd_cfg->cfg.idle_time_before_sleep_ms);

    if (this->lockout_after_nr_of_fails_sensor_ != nullptr)
      this->lockout_after_nr_of_fails_sensor_->publish_state(cmd_cfg->cfg.idfy_max_consecutive_fails);

    if (this->lockout_time_s_sensor_ != nullptr)
      this->lockout_time_s_sensor_->publish_state(cmd_cfg->cfg.idfy_lockout_time_s);

    if (this->baud_rate_sensor_ != nullptr)
      this->baud_rate_sensor_->publish_state(cmd_cfg->cfg.uart_baudrate);

    if (this->status_at_boot_binary_sensor_ != nullptr) {
      if ((cmd_cfg->cfg.sys_flags & CFG_SYS_FLAG_STATUS_EVT_AT_BOOT) != 0)
        this->status_at_boot_binary_sensor_->publish_state(true);
      else
        this->status_at_boot_binary_sensor_->publish_state(false);
    }
    if (this->stop_mode_uart_binary_sensor_ != nullptr) {
      if ((cmd_cfg->cfg.sys_flags & CFG_SYS_FLAG_UART_IN_STOP_MODE) != 0)
        this->stop_mode_uart_binary_sensor_->publish_state(true);
      else
        this->stop_mode_uart_binary_sensor_->publish_state(false);
    }
    if (this->uart_irq_before_tx_binary_sensor_ != nullptr) {
      if ((cmd_cfg->cfg.sys_flags & CFG_SYS_FLAG_UART_IRQ_BEFORE_TX) != 0)
        this->uart_irq_before_tx_binary_sensor_->publish_state(true);
      else
        this->uart_irq_before_tx_binary_sensor_->publish_state(false);
    }

    this->config_received = true;
    this->current_config_ = cmd_cfg->cfg;
  }

  return result;
}

/*
------------------------
HAL FUNCTIONS DEFINITONS
------------------------
*/
fpc::fpc_result_t FingerprintFPC2532Component::fpc_hal_init(void) {
  if (this->reset_pin_ != nullptr) {
    this->reset_pin_->setup();
    this->reset_pin_->digital_write(true);
  }
  return FPC_RESULT_OK;
}
void FingerprintFPC2532Component::hal_reset_device() {
  if (this->reset_pin_ != nullptr) {
    this->reset_pin_->digital_write(false);
    delay(10);
    this->reset_pin_->digital_write(true);
    ESP_LOGI(TAG, "System Reset via RST_N pin");
  }
}
fpc::fpc_result_t FingerprintFPC2532Component::fpc_hal_tx(uint8_t *data, std::size_t len) {
  if (!data || len == 0) {
    return FPC_RESULT_FAILURE;
  }
  this->write_array(data, len);
  delay(1);
  return FPC_RESULT_OK;  // doesn't guarantee array was actually sent: no timeout handling here
}
fpc::fpc_result_t FingerprintFPC2532Component::fpc_hal_rx(uint8_t *data, std::size_t len) {
  return this->read_array(data, len) ? FPC_RESULT_OK : FPC_RESULT_FAILURE;
}
void FingerprintFPC2532Component::fpc_hal_delay_ms(uint32_t ms) { delay(ms); }

void FingerprintFPC2532Component::dump_config() {}

}  // namespace fingerprint_FPC2532
}  // namespace esphome
