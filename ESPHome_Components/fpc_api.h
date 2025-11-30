/*
 * Copyright (c) 2024 Fingerprint Cards AB
 *
 * Licensed under the Apache License, Version 2.0 (the "License")
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   https://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * @file    fpc_api.h
 * @brief   FPC AllKey API
 *
 * This is the Command Interface for the Fingerprint Sensor Module fpc2532.
 */
#ifdef __cplusplus
extern "C" {
#endif

#ifndef FPC_API_H_
#define FPC_API_H_

#include <stdint.h>
namespace fpc {
/* -----------------------------------------------------------------------------
 Result Codes
------------------------------------------------------------------------------*/

typedef uint16_t fpc_result_t;

/* Results 0 - 10 is information */
#define FPC_RESULT_OK 0
#define FPC_PENDING_OPERATION 1
#define FPC_RESULT_DATA_NOT_SET 2
#define FPC_RESULT_CMD_ID_NOT_SUPPORTED 3

/* Errors */
#define FPC_RESULT_FAILURE 11
#define FPC_RESULT_INVALID_PARAM 12
#define FPC_RESULT_WRONG_STATE 13
#define FPC_RESULT_OUT_OF_MEMORY 14
#define FPC_RESULT_TIMEOUT 15
#define FPC_RESULT_NOT_SUPPORTED 16

/* Template and Users ID Errors */
#define FPC_RESULT_USER_ID_EXISTS 20
#define FPC_RESULT_USER_ID_NOT_FOUND 21
#define FPC_RESULT_STORAGE_IS_FULL 22
#define FPC_RESULT_FLASH_ERROR 23
#define FPC_RESULT_IDENTIFY_LOCKOUT 24
#define FPC_RESULT_STORAGE_IS_EMPTY 25

/* IO Errors */
#define FPC_RESULT_IO_BUSY 31
#define FPC_RESULT_IO_RUNTIME_FAILURE 32
#define FPC_RESULT_IO_BAD_DATA 33
#define FPC_RESULT_IO_NOT_SUPPORTED 34
#define FPC_RESULT_IO_NO_DATA 35

/* Image Capture Errors */
#define FPC_RESULT_COULD_NOT_ARM 40
#define FPC_RESULT_CAPTURE_FAILED 41
#define FPC_RESULT_BAD_IMAGE_QUALITY 42
#define FPC_RESULT_NO_IMAGE 43

/* Other Errors */
#define FPC_RESULT_SENSOR_ERROR 50
#define FPC_RESULT_PROTOCOL_VERSION_ERROR 70
#define FPC_STARTUP_FAILURE 101

/* -----------------------------------------------------------------------------
 Frame Defines and Structs
------------------------------------------------------------------------------*/

/**
 * @brief Frame Protocol Version
 */
#define FPC_FRAME_PROTOCOL_VERSION 0x0004

/**
 * @brief Frame Type
 */
#define FPC_FRAME_TYPE_CMD_REQUEST 0x11
#define FPC_FRAME_TYPE_CMD_RESPONSE 0x12
#define FPC_FRAME_TYPE_CMD_EVENT 0x13

/**
 * @brief Frame Flags
 */
#define FPC_FRAME_FLAG_SENDER_HOST 0x0010
#define FPC_FRAME_FLAG_SENDER_FW_BL 0x0020
#define FPC_FRAME_FLAG_SENDER_FW_APP 0x0040

#define MAX_HOST_PACKET_SIZE_DEFAULT (2 * 1024)

/**
 * @brief Frame Header.
 */
typedef struct {
  /** Protocol version */
  uint16_t version;
  /** Type of frame. One of FPC_FRAME_TYPE_*. */
  uint16_t type;
  /** Frame flags. A selection of FPC_FRAME_FLAG_*. */
  uint16_t flags;
  /** Size of the following payload. */
  uint16_t payload_size;
  /** Placeholder for payload. Typically a command.
   * original: uint8_t payload[];
   * opt 1. remove flexible array member since not used directly (suggested by software's owner)
   * opt2. modify to *payload to avoid flexible array member for C++17 compliance
   * memory allocation needed to be handled manually
   */
  // uint8_t *payload;
} fpc_frame_hdr_t;

/**
 * @brief Command Header.
 */
typedef struct {
  /** Command ID. One of CMD_* in fpc_cmds.h. */
  uint16_t cmd_id;
  /** Type of frame. One of FPC_BP_FRAME_TYPE_*. */
  uint16_t type;
  /** Placeholder for payload. Typically a command.
   * original: uint8_t payload[];
   * opt 1. remove flexible array member since not used directly (suggested by software's owner)
   * opt2. modify to *payload to avoid flexible array member for C++17 compliance
   * memory allocation needed to be handled manually
   */
  // uint8_t *payload;
} fpc_cmd_hdr_t;

/* -----------------------------------------------------------------------------
 Command Defines
------------------------------------------------------------------------------*/

/**
 * @brief Command IDs.
 */
#define CMD_STATUS 0x0040
#define CMD_VERSION 0x0041
#define CMD_BIST 0x0044
#define CMD_CAPTURE 0x0050
#define CMD_ABORT 0x0052
#define CMD_IMAGE_DATA 0x0053
#define CMD_ENROLL 0x0054
#define CMD_IDENTIFY 0x0055
#define CMD_LIST_TEMPLATES 0x0060
#define CMD_DELETE_TEMPLATE 0x0061
#define CMD_GET_SYSTEM_CONFIG 0x006A
#define CMD_SET_SYSTEM_CONFIG 0x006B
#define CMD_RESET 0x0072
#define CMD_SET_DBG_LOG_LEVEL 0x00B0
#define CMD_DATA_GET 0x0101
#define CMD_NAVIGATION 0x0200
#define CMD_GPIO_CONTROL 0x0300

/**
 * @brief Status Event.
 */
#define EVENT_NONE 0
#define EVENT_IDLE 1
#define EVENT_ARMED 2
#define EVENT_FINGER_DETECT 3
#define EVENT_FINGER_LOST 4
#define EVENT_IMAGE_READY 5
#define EVENT_CMD_FAILED 6

/**
 * @brief System States (Bitmap).
 */
#define STATE_APP_FW_READY 0x0001
#define STATE_CAPTURE 0x0004
#define STATE_IMAGE_AVAILABLE 0x0010
#define STATE_DATA_TRANSFER 0x0040
#define STATE_FINGER_DOWN 0x0080
#define STATE_SYS_ERROR 0x0400
#define STATE_ENROLL 0x1000
#define STATE_IDENTIFY 0x2000
#define STATE_NAVIGATION 0x4000

/* -----------------------------------------------------------------------------
 Command Payload Definitions - Core
------------------------------------------------------------------------------*/

/**
 * @brief Payload definition of the CMD_STATUS Response / Event.
 */
typedef struct {
  /** Command header. */
  fpc_cmd_hdr_t cmd;
  /** One of EVENT_* above. */
  uint16_t event;
  /** The current state. A combination of STATE_* defines above. */
  uint16_t state;
  /** Additional details of failure. */
  uint16_t app_fail_code;
  /** N/A. */
  int16_t reserved;
} fpc_cmd_status_response_t;

/**
 * @brief Payload definition of the CMD_VERSION Response.
 */
typedef struct {
  /** Command header. */
  fpc_cmd_hdr_t cmd;
  /** 96 bits of unique ID. */
  uint32_t mcu_unique_id[3];
  /** FW ID. */
  uint8_t fw_id;
  /** Fuse Level. */
  uint8_t fw_fuse_level;
  /** Version String Length */
  uint16_t version_str_len;
  /** Version String (incl termination '\0') */
  char version_str[];
} fpc_cmd_version_response_t;

/* -----------------------------------------------------------------------------
 Command Payload Definitions - Biometric
------------------------------------------------------------------------------*/

/**
 * @brief Enrollment Feedback.
 */
#define ENROLL_FEEDBACK_DONE 1
#define ENROLL_FEEDBACK_PROGRESS 2
#define ENROLL_FEEDBACK_REJECT_LOW_QUALITY 3
#define ENROLL_FEEDBACK_REJECT_LOW_COVERAGE 4
#define ENROLL_FEEDBACK_REJECT_LOW_MOBILITY 5
#define ENROLL_FEEDBACK_REJECT_OTHER 6
#define ENROLL_FEEDBACK_PROGRESS_IMMOBILE 7

/**
 * @brief Identify match results.
 */
#define IDENTIFY_RESULT_MATCH 0x61EC
#define IDENTIFY_RESULT_NO_MATCH 0xBAAD

/**
 * @brief Image request types.
 */
#define CMD_IMAGE_REQUEST_TYPE_INFO_RAW 0
#define CMD_IMAGE_REQUEST_TYPE_INFO_FMI 1
#define CMD_IMAGE_REQUEST_TYPE_GET_RAW 2
#define CMD_IMAGE_REQUEST_TYPE_GET_FMI 3

/**
 * @brief Navigation events
 */
#define CMD_NAV_EVENT_NONE 0
#define CMD_NAV_EVENT_UP 1
#define CMD_NAV_EVENT_DOWN 2
#define CMD_NAV_EVENT_RIGHT 3
#define CMD_NAV_EVENT_LEFT 4
#define CMD_NAV_EVENT_PRESS 5
#define CMD_NAV_EVENT_LONG_PRESS 6

/**
 * @brief Navigation configuration
 */
#define CMD_NAV_CFG_ORIENTATION_0 0x00000000
#define CMD_NAV_CFG_ORIENTATION_90 0x00000001
#define CMD_NAV_CFG_ORIENTATION_180 0x00000002
#define CMD_NAV_CFG_ORIENTATION_270 0x00000003
#define CMD_NAV_CFG_ORIENTATION_MASK 0x00000003
#define CMD_NAV_CFG_SKIP_FINGER_STABLE 0x00000004
#define CMD_NAV_CFG_SEND_SAMPLE_DATA 0x00000008

/**
 * @brief Template ID NONE, option valid for:
 *
 * CMD_IDENTIFY Result (In No Match case)
 */
#define ID_TYPE_NONE 0x1012

/**
 * @brief Template ID ALL, option valid for:
 *
 * CMD_IDENTIFY Request
 * CMD_DELETE_TEMPLATE Request
 */
#define ID_TYPE_ALL 0x2023

/**
 * @brief Template ID SPECIFIED, option valid for:
 *
 * CMD_IDENTIFY Request (Verify) and Response
 * CMD_ENROLL Request
 * CMD_DELETE_TEMPLATE Request
 */
#define ID_TYPE_SPECIFIED 0x3034

/**
 * @brief Template ID GENERATE NEW, option valid for:
 *
 * CMD_ENROLL Request
 */
#define ID_TYPE_GENERATE_NEW 0x4045

/**
 * @brief Template ID specifier payload.
 */
typedef struct {
  /** Type of Specifier. One of ID_TYPE_*. */
  uint16_t type;
  /** Template ID, only valid when type=ID_TYPE_SPECIFIED. */
  uint16_t id;
} fpc_id_type_t;

/**
 * @brief Payload definition of the CMD_CAPTURE Request command.
 */
typedef struct {
  /** Command header. */
  fpc_cmd_hdr_t cmd;
} fpc_cmd_capture_request_t;

/**
 * @brief Payload definition of the CMD_ENROLL Request.
 *
 * Note: The Response to this command is a CMD_STATUS Response, followed by
 * additional CMD_STATUS and CMD_ENROLL Status Events.
 */
typedef struct {
  /** Command header. */
  fpc_cmd_hdr_t cmd;
  /** Template ID. */
  fpc_id_type_t tpl_id;
} fpc_cmd_enroll_request_t;

/**
 * @brief Payload definition of the CMD_ENROLL Status Event.
 */
typedef struct {
  /** Command header. */
  fpc_cmd_hdr_t cmd;
  /** Template ID of the ongoing enrollment. */
  uint16_t id;
  /** Enroll feedback. */
  uint8_t feedback;
  /** Counter with the remaining touches for the current enrollment. */
  uint8_t samples_remaining;
} fpc_cmd_enroll_status_response_t;

/**
 * @brief Payload definition of the CMD_IDENTIFY Request.
 *
 * Note: The Response to this command is a CMD_STATUS Response, followed by
 * additional CMD_STATUS and CMD_IDENTIFY Status Events.
 */
typedef struct {
  /** Command header. */
  fpc_cmd_hdr_t cmd;
  /** Template ID. */
  fpc_id_type_t tpl_id;
  /** Operation tag. The same tag will be returned in the response. */
  uint16_t tag;
} fpc_cmd_identify_request_t;

/**
 * @brief Payload definition of the CMD_IDENTIFY Event.
 */
typedef struct {
  /** Command header. */
  fpc_cmd_hdr_t cmd;
  /** Match result. One of IDENTIFY_RESULT_*. */
  uint16_t match;
  /** Template ID. */
  fpc_id_type_t tpl_id;
  /** Operation tag. The tag that was entered via the request. */
  uint16_t tag;
} fpc_cmd_identify_status_response_t;

/**
 * @brief Payload definition of the CMD_DELETE_TEMPLATE Request.
 */
typedef struct {
  /** Command header. */
  fpc_cmd_hdr_t cmd;
  /** Template ID. */
  fpc_id_type_t tpl_id;
} fpc_cmd_template_delete_request_t;

/**
 * @brief Payload definition of the CMD_LIST_TEMPLATES Response.
 *
 * Note: The CMD_LIST_TEMPLATES Request has no payload.
 */
typedef struct {
  /** Command header. */
  fpc_cmd_hdr_t cmd;
  /** Number of enrolled templates .*/
  uint16_t number_of_templates;
  /** List of enrolled template IDs. Length uint16_t * number_of_templates. */
  uint16_t template_id_list[];
} fpc_cmd_template_info_response_t;

/**
 * @brief Payload definition of the Image request command
 */
typedef struct {
  /** Command header. */
  fpc_cmd_hdr_t cmd;
  /** Image request type. One of CMD_IMAGE_REQUEST_TYPE_* */
  uint16_t type;
  /** Size of image. Valid for PUT, set to 0 for GET */
  uint16_t total_size;
} fpc_cmd_image_request_t;

/**
 * @brief Payload definition of the Image response command
 */
typedef struct {
  /** Command header. */
  fpc_cmd_hdr_t cmd;
  /** Image size. */
  uint32_t image_size;
  /** Image width. */
  uint16_t image_width;
  /** Image height. */
  uint16_t image_height;
  /** Image request type */
  uint16_t type;
  /** The maximal chunk size for the active interface */
  uint16_t max_chunk_size;
} fpc_cmd_image_response_t;

/**
 * @brief Payload definition of the CMD_NAVIGATION Request.
 */
typedef struct {
  /** Command header. */
  fpc_cmd_hdr_t cmd;
  /* Navigation orientation. One of CMD_NAV_CFG_* */
  uint32_t config;
} fpc_cmd_navigation_request_t;

/**
 * @brief Payload definition of the CMD_NAVIGATION Event.
 */
typedef struct {
  /** Command header. */
  fpc_cmd_hdr_t cmd;
  /* Navigation gesture. One of CMD_NAV_EVENT_* */
  uint16_t gesture;
  uint16_t n_samples;
  uint16_t samples[];
} fpc_cmd_navigation_status_event_t;

/* -----------------------------------------------------------------------------
 Command Payload Definitions - System Configuration
------------------------------------------------------------------------------*/

/** Version of System Configuration Structure */
#define CFG_VERSION 1

/**
 * @brief System Configuration Flags.
 */
/** Send Status Event after system boot. */
#define CFG_SYS_FLAG_STATUS_EVT_AT_BOOT 0x00000001
/**
 * Let system go into stop mode when using UART interface. This requires the
 * system to be woken via wake-up pin (CS) before senging any UART data to host.
 */
#define CFG_SYS_FLAG_UART_IN_STOP_MODE 0x00000010
/**
 * Set IRQ pin before SiP sends UART data. The delay between IRQ and start of
 * data is configurable via uart_delay_before_irq_ms
 */
#define CFG_SYS_FLAG_UART_IRQ_BEFORE_TX 0x00000020

/**
 * @brief UART baudrate definitions.
 */
#define CFG_UART_BAUDRATE_9600 1
#define CFG_UART_BAUDRATE_19200 2
#define CFG_UART_BAUDRATE_57600 3
#define CFG_UART_BAUDRATE_115200 4
#define CFG_UART_BAUDRATE_921600 5

/**
 * @brief System Configuration parameters.
 */
typedef struct {
  /** Config Version */
  uint16_t version;
  /** Nominal sleep time between finger present queries [ms].
   * Range [0, 1020].  */
  uint16_t finger_scan_interval_ms;
  /** Combination of CFG_SYS_FLAG */
  uint32_t sys_flags;
  /** Delay between the IRQ pin is set and UART TX is started. */
  uint8_t uart_delay_before_irq_ms;
  /** One of CFG_UART_BAUDRATE_ */
  uint8_t uart_baudrate;
  /** Max number of failed Identify before lockout */
  uint8_t idfy_max_consecutive_fails;
  /** Identify lockout time, after too many fails. */
  uint8_t idfy_lockout_time_s;
  /** Idle time after last command, before entering stop mode [ms] */
  uint16_t idle_time_before_sleep_ms;
} fpc_system_config_t;

#define FPC_SYS_CFG_TYPE_DEFAULT 0
#define FPC_SYS_CFG_TYPE_CUSTOM 1

/**
 * @brief Payload definition of the CMD_GET_SYSTEM_CONFIG Request.
 */
typedef struct {
  /** Command header. */
  fpc_cmd_hdr_t cmd;
  /** Config Type. One of FPC_SYS_CFG_TYPE_ */
  uint16_t config_type;
} fpc_cmd_get_config_request_t;

/**
 * @brief Payload definition of the CMD_GET_SYSTEM_CONFIG Response.
 */
typedef struct {
  /** Command header. */
  fpc_cmd_hdr_t cmd;
  /** Config Type. One of FPC_SYS_CFG_TYPE_ */
  uint16_t config_type;
  /** Config Data */
  fpc_system_config_t cfg;
} fpc_cmd_get_config_response_t;

/**
 * @brief Payload definition of the CMD_SET_SYSTEM_CONFIG Request.
 *
 * Note, repsonse is of CMD_STATUS type
 */
typedef struct {
  /** Command header. */
  fpc_cmd_hdr_t cmd;
  /** Config Data */
  fpc_system_config_t cfg;
} fpc_cmd_set_config_request_t;

/* -----------------------------------------------------------------------------
 Command Payload Definitions - GPIO Pin Control
------------------------------------------------------------------------------*/

#define GPIO_CONTROL_SUB_CMD_GET 0
#define GPIO_CONTROL_SUB_CMD_SET 1

#define GPIO_CONTROL_MODE_NOT_USED 0
#define GPIO_CONTROL_MODE_OUTPUT_PP 1
#define GPIO_CONTROL_MODE_OUTPUT_OD 2
#define GPIO_CONTROL_MODE_INPUT_PULL_NONE 3
#define GPIO_CONTROL_MODE_INPUT_PULL_UP 4
#define GPIO_CONTROL_MODE_INPUT_PULL_DOWN 5

#define GPIO_CONTROL_STATE_RESET 0
#define GPIO_CONTROL_STATE_SET 1

/**
 * @brief Payload definition of the CMD_GPIO_CONTROL Request.
 */
typedef struct {
  /** Command header. */
  fpc_cmd_hdr_t cmd;
  /** Sub command. One of GPIO_CONTROL_SUB_CMD... */
  uint8_t sub_cmd;
  /** Gpio pin (according to product spec). */
  uint8_t pin;
  /** Gpio mode. One of GPIO_CONTROL_MODE... */
  uint8_t mode;
  /** Gpio state. One of GPIO_CONTROL_STATE... */
  uint8_t state;
} fpc_cmd_pinctrl_gpio_request_t;

/**
 * @brief Payload definition of the CMD_GPIO_CONTROL Response.
 *
 * Used as response for GPIO_CONTROL_SUB_CMD_GET.
 */
typedef struct {
  /** Command header. */
  fpc_cmd_hdr_t cmd;
  /** Gpio state. One of GPIO_CONTROL_STATE... */
  uint8_t state;
} fpc_cmd_pinctrl_gpio_response_t;

/* -----------------------------------------------------------------------------
 Command Payload Definitions - BIST
------------------------------------------------------------------------------*/

typedef struct {
  /** Command header */
  fpc_cmd_hdr_t cmd;
  /** Result of sensor test. */
  uint16_t sensor_test_result;
  /** Overall Verdict of the Builtin Self Test. */
  uint16_t test_verdict;
} fpc_cmd_bist_response_t;

}  // namespace fpc
#endif /* FPC_API_H_ */

#ifdef __cplusplus
}
#endif
