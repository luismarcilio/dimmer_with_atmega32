/*
 * =====================================================================================
 *
 *       Filename:  hci.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/01/09 10:04:22
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Luis Marcilio Braga de Oliveira (), lmbraga@embratel.com.br
 *        Company:  EMBRATEL S.A.
 *
 * =====================================================================================
 */

#ifndef  __HCI_H__
#define  __HCI_H__


#include <stdlib.h>
#include <sys/types.h>
#include "l2cap.h"
#include "types.h"
#include "debug.h"
#include "config.h"


/* HCI Commands */
#define LINK_CONTROL_COMMAND 0x01
#define LINK_POLICY_COMMAND 0x02
#define CONTROLLER_BASEBAND_COMMAND 0x03

/* Link Control Commands (OGF = 0x01) */

#define HCI_INQUIRY 0X0001
#define HCI_INQUIRY_CANCEL 0X0002
#define HCI_PERIODIC_INQUIRY_MODE 0X0003
#define HCI_EXIT_PERIODIC_INQUIRY_MODE 0X0004
#define HCI_CREATE_CONNECTION 0X0005
#define HCI_DISCONNECT 0X0006
#define HCI_ADD_SCO_CONNECTION 0X0007
#define HCI_ACCEPT_CONNECTION_REQUEST 0X0009
#define HCI_REJECT_CONNECTION_REQUEST 0X000A
#define HCI_LINK_KEY_REQUEST_REPLY 0X000B
#define HCI_LINK_KEY_REQUEST_NEGATIVE_REPLY 0X000C
#define HCI_PIN_CODE_REQUEST_REPLY 0X000D
#define HCI_PIN_CODE_REQUEST_NEGATIVE_REPLY 0X000E
#define HCI_CHANGE_CONNECTION_PACKET_TYPE 0X000F
#define HCI_AUTHENTICATION_REQUESTED 0X0011
#define HCI_SET_CONNECTION_ENCRYPTION 0X0013
#define HCI_CHANGE_CONNECTION_LINK_KEY 0X0015
#define HCI_MASTER_LINK_KEY 0X0017
#define HCI_REMOTE_NAME_REQUEST 0X0019
#define HCI_READ_REMOTE_SUPPORTED_FEATURES 0X001B
#define HCI_READ_REMOTE_VERSION_INFORMATION 0X001D
#define HCI_READ_CLOCK_OFFSET 0X001F

/* hci policy command (ogf=0x02) */

#define HCI_HOLD_MODE 0X0001
#define HCI_SNIFF_MODE 0X0003
#define HCI_EXIT_SNIFF_MODE 0X0004
#define HCI_PARK_MODE 0X0005
#define HCI_EXIT_PARK_MODE 0X0006
#define HCI_QOS_SETUP 0X0007
#define HCI_ROLE_DISCOVERY 0X0009
#define HCI_SWITCH_ROLE 0X000B
#define HCI_READ_LINK_POLICY_SETTINGS 0X000C
#define HCI_WRITE_LINK_POLICY_SETTINGS 0X000D

/* host controller and baseband commands (ogf=0x03) */
#define HCI_SET_EVENT_MASK 0X0001
#define HCI_RESET 0X0003
#define HCI_SET_EVENT_FILTER 0X0005
#define HCI_FLUSH 0X0008
#define HCI_READ_PIN_TYPE 0X0009
#define HCI_WRITE_PIN_TYPE 0X000A
#define HCI_CREATE_NEW_UNIT_KEY 0X000B
#define HCI_READ_STORED_LINK_KEY 0X000D
#define HCI_WRITE_STORED_LINK_KEY 0X0011
#define HCI_DELETE_STORED_LINK_KEY 0X0012
#define HCI_CHANGE_LOCAL_NAME 0X0013
#define HCI_READ_LOCAL_NAME 0X0014
#define HCI_READ_CONNECTION_ACCEPT_TIMEOUT 0X0015
#define HCI_WRITE_CONNECTION_ACCEPT_TIMEOUT 0X0016
#define HCI_READ_PAGE_TIMEOUT 0X0017
#define HCI_WRITE_PAGE_TIMEOUT 0X0018
#define HCI_READ_SCAN_ENABLE 0X0019
#define HCI_WRITE_SCAN_ENABLE 0X001A
#define HCI_READ_PAGE_SCAN_ACTIVITY 0X001B
#define HCI_WRITE_PAGE_SCAN_ACTIVITY 0X001C
#define HCI_READ_INQUIRY_SCAN_ACTIVITY 0X001D
#define HCI_WRITE_INQUIRY_SCAN_ACTIVITY 0X001E
#define HCI_READ_AUTHENTICATION_ENABLE 0X001F
#define HCI_WRITE_AUTHENTICATION_ENABLE 0X0020
#define HCI_READ_ENCRYPTION_MODE 0X0021
#define HCI_WRITE_ENCRYPTION_MODE 0X0022
#define HCI_READ_CLASS_OF_DEVICE 0X0023
#define HCI_WRITE_CLASS_OF_DEVICE 0X0024
#define HCI_READ_VOICE_SETTING 0X0025
#define HCI_WRITE_VOICE_SETTING 0X0026
#define HCI_READ_AUTOMATIC_FLUSH_TIMEOUT 0X0027
#define HCI_WRITE_AUTOMATIC_FLUSH_TIMEOUT 0X0028
#define HCI_READ_NUM_BROADCAST_RETRANSMISSIONS 0X0029
#define HCI_WRITE_NUM_BROADCAST_RETRANSMISSIONS 0X002A
#define HCI_READ_HOLD_MODE_ACTIVITY 0X002B
#define HCI_WRITE_HOLD_MODE_ACTIVITY 0X002C
#define HCI_READ_TRANSMIT_POWER_LEVEL 0X002D
#define HCI_READ_SCO_FLOW_CONTROL_ENABLE 0X002E
#define HCI_WRITE_SCO_FLOW_CONTROL_ENABLE 0X002F
#define HCI_SET_HOST_CONTROLLER_TO_HOST_FLOW_CONTROL 0X0031
#define HCI_HOST_BUFFER_SIZE 0X0033
#define HCI_HOST_NUMBER_OF_COMPLETED_PACKETS 0X0035
#define HCI_READ_LINK_SUPERVISION_TIMEOUT 0X0036
#define HCI_WRITE_LINK_SUPERVISION_TIMEOUT 0X0037
#define HCI_READ_NUMBER_OF_SUPPORTED_IAC 0X0038
#define HCI_READ_CURRENT_IAC_LAP 0X0039
#define HCI_WRITE_CURRENT_IAC_LAP 0X003A
#define HCI_READ_PAGE_SCAN_PERIOD_MODE 0X003B
#define HCI_WRITE_PAGE_SCAN_PERIOD_MODE 0X003C
#define HCI_READ_PAGE_SCAN_MODE 0X003D
#define HCI_WRITE_PAGE_SCAN_MODE 0X003E

/* hci events */
#define HCI_INQUIRY_COMPLETE_EVENT 0X01
#define HCI_INQUIRY_RESULT_EVENT 0X02
#define HCI_CONNECTION_COMPLETE_EVENT 0X03
#define HCI_CONNECTION_REQUEST_EVENT 0X04
#define HCI_DISCONNECTION_COMPLETE_EVENT 0X05
#define HCI_AUTHENTICATION_COMPLETE_EVENT 0X06
#define HCI_REMOTE_NAME_REQUEST_COMPLETE_EVENT 0X07
#define HCI_ENCRYPTION_CHANGE_EVENT 0X08
#define HCI_CHANGE_CONNECTION_LINK_KEY_COMPLETE_EVENT 0X09
#define HCI_MASTER_LINK_KEY_COMPLETE_EVENT 0X0A
#define HCI_READ_REMOTE_SUPPORTED_FEATURES_COMPLETE_EVENT 0X0B
#define HCI_READ_REMOTE_VERSION_COMPLETE_EVENT 0X0C
#define HCI_Q0S_SETUP_COMPLETE_EVENT 0X0D
#define HCI_COMMAND_COMPLETE_EVENT 0X0E
#define HCI_COMMAND_STATUS_EVENT 0X0F
#define HCI_HARDWARE_ERROR_EVENT 0X10
#define HCI_FLUSH_OCCURED_EVENT 0X11
#define HCI_ROLE_CHANGE_EVENT 0X12
#define HCI_NUMBER_OF_COMPLETED_PACKETS_EVENT 0X13
#define HCI_MODE_CHANGE_EVENT 0X14
#define HCI_RETURN_LINK_KEYS_EVENT 0X15
#define HCI_PIN_CODE_REQUEST_EVENT 0X16
#define HCI_LINK_KEY_REQUEST_EVENT 0X17
#define HCI_LINK_KEY_NOTIFICATION_EVENT 0X18
#define HCI_LOOPBACK_COMMAND_EVENT 0X19
#define HCI_DATA_BUFFER_OVERFLOW_EVENT 0X1A
#define HCI_MAX_SLOTS_CHANGE_EVENT 0X1B
#define HCI_READ_CLOCK_OFFSET_COMPLETE_EVENT 0X1C
#define HCI_CONNECTION_PACKET_TYPE_CHANGED_EVENT 0X1D
#define HCI_QOS_VIOLATION_EVENT 0X1E
#define HCI_PAGE_SCAN_MODE_CHANGE_EVENT 0X1F
#define HCI_PAGE_SCAN_REPETITION_MODE_CHANGE_EVENT 0X20



/* hci error codes */

#define HCI_UNKNOWN_HCI_COMMAND 0X01
#define HCI_NO_CONNECTION 0X02
#define HCI_HARDWARE_FAILURE 0X03
#define HCI_PAGE_TIMEOUT 0X04
#define HCI_AUTHENTICATION_FAILURE 0X05
#define HCI_KEY_MISSING 0X06
#define HCI_MEMORY_FULL 0X07
#define HCI_CONNECTION_TIMEOUT 0X08
#define HCI_MAX_NUMBER_OF_CONNECTIONS 0X09
#define HCI_MAX_NUMBER_OF_SCO_CONNECTIONS_TO_A_DEVICE 0X0A
#define HCI_ACL_CONNECTION_ALREADY_EXISTS 0X0B
#define HCI_COMMAND_DISALLOWED 0X0C
#define HCI_HOST_REJECTED_DUE_TO_LIMITED_RESOURCES 0X0D
#define HCI_HOST_REJECTED_DUE_TO_SECURITY_REASONS 0X0E
#define HCI_HOST_REJECTED_DUE_TO_A_REMOTE_DEVICE_ONLY_A_PERSONAL_DEVICE 0X0F
#define HCI_HOST_TIMEOUT 0X1
#define HCI_UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE 0X11
#define HCI_INVALID_HCI_COMMAND_PARAMETERS 0X12
#define HCI_OTHER_END_TERMINATED_CONNECTION_USER_ENDED_CONNECTION 0X13
#define HCI_OTHER_END_TERMINATED_CONNECTION_LOW_RESOURCES 0X14
#define HCI_OTHER_END_TERMINATED_CONNECTION_ABOUT_TO_POWER_OFF 0X15
#define HCI_CONNECTION_TERMINATED_BY_LOCAL_HOST 0X16
#define HCI_REPEATED_ATTEMPTS 0X17
#define HCI_PAIRING_NOT_ALLOWED 0X18
#define HCI_UNKNOWN_LMP_PDU 0X19
#define HCI_UNSUPPORTED_REMOTE_FEATURE 0X1A
#define HCI_SCO_OFFSET_REJECTED 0X1B
#define HCI_SCO_INTERVAL_REJECTED 0X1C
#define HCI_SCO_AIR_MODE_REJECTED 0X1D
#define HCI_INVALID_LMP_PARAMETERS 0X1E
#define HCI_UNSPECIFIED_ERROR 0X1F
#define HCI_UNSUPPORTED_LMP_PARAMETER 0X20
#define HCI_ROLE_CHANGE_NOT_ALLOWED 0X21
#define HCI_LMP_RESPONSE_TIMEOUT 0X22
#define HCI_LMP_ERROR_TRANSACTION_COLLISION 0X23
#define HCI_LMP_PDU_NOT_ALLOWED 0X24
#define HCI_ENCRYPTION_MODE_NOT_ACCEPTABLE 0X25
#define HCI_UNIT_KEY_USED 0X26
#define HCI_QOS_NOT_SUPPORTED 0X27
#define HCI_INSTANT_PASSED 0X28
#define HCI_PAIRING_WITH_UNIT_KEY_NOT_SUPPORTED 0X29


#define ASSEMBLE_HCI_COMMAND(OCF,OGF,TOT_LEN,PARAM,COMMAND) \
{ \
    COMMAND.OpCode.ocf = OGF; \
    COMMAND.OpCode.ogf = OCF; \
    COMMAND.length = TOT_LEN; \
    COMMAND.param = PARAM; \
}



typedef struct hci_command_type 
{
    struct
    {
        uint16_t ocf : 10;
        uint16_t ogf : 6;
    } OpCode;

    uint8_t     length;
    uint8_t*    param;
}  hci_command_type;

typedef struct hci_event_type
{
    uint8_t     eventCode;
    uint8_t     length;
    uint8_t*    param;
} hci_event_type;

typedef struct 
{
    uint16_t  handle:12;
    uint16_t  pbFlag:2;
    uint16_t  bcFlag:2;
    uint16_t  length;
    uint8_t  payload[L2CAP_MTU];
} acl_packet_type;

typedef struct connection_type
{
    uint8_t status;
    uint16_t handle;
    bd_addr_t bd_addr;
    uint8_t link_type;
    uint8_t encryption;
    uint8_t n_packets_out;
    l2cap_packet_type l2cap_packet_in[1];
    l2cap_packet_type l2cap_packet_out[2];
    struct connection_type *next;
} connection_type;


struct cb_events_item
{
    uint8_t event;
    err_t (* callback)(hci_event_type *event_data);
};


err_t send_hci_command(hci_command_type *command, 
                       err_t (* hci_command_complete_cb)(hci_event_type *event_data),
                       err_t (* hci_command_status_cb)(hci_event_type *event_data));


err_t hci_inquiry_complete_event_cb(hci_event_type *event_data);
err_t hci_inquiry_result_event_cb(hci_event_type *event_data);
err_t hci_connection_complete_event_cb(hci_event_type *event_data);
err_t hci_connection_request_event_cb(hci_event_type *event_data);
err_t hci_disconnection_complete_event_cb(hci_event_type *event_data);
err_t hci_authentication_complete_event_cb(hci_event_type *event_data);
err_t hci_remote_name_request_complete_event_cb(hci_event_type *event_data);
err_t hci_encryption_change_event_cb(hci_event_type *event_data);
err_t hci_change_connection_link_key_complete_event_cb(hci_event_type *event_data);
err_t hci_master_link_key_complete_event_cb(hci_event_type *event_data);
err_t hci_read_remote_supported_features_complete_event_cb(hci_event_type *event_data);
err_t hci_read_remote_version_complete_event_cb(hci_event_type *event_data);
err_t hci_qos_setup_complete_event_cb(hci_event_type *event_data);
err_t hci_command_complete_event_cb(hci_event_type *event_data);
err_t hci_command_status_event_cb(hci_event_type *event_data);
err_t hci_hardware_error_event_cb(hci_event_type *event_data);
err_t hci_flush_occured_event_cb(hci_event_type *event_data);
err_t hci_role_change_event_cb(hci_event_type *event_data);
err_t hci_number_of_completed_packets_event_cb(hci_event_type *event_data);
err_t hci_mode_change_event_cb(hci_event_type *event_data);
err_t hci_return_link_keys_event_cb(hci_event_type *event_data);
err_t hci_pin_code_request_event_cb(hci_event_type *event_data);
err_t hci_link_key_request_event_cb(hci_event_type *event_data);
err_t hci_link_key_notification_event_cb(hci_event_type *event_data);
err_t hci_loopback_command_event_cb(hci_event_type *event_data);
err_t hci_data_buffer_overflow_event_cb(hci_event_type *event_data);
err_t hci_max_slots_change_event_cb(hci_event_type *event_data);
err_t hci_read_clock_offset_complete_event_cb(hci_event_type *event_data);
err_t hci_connection_packet_type_changed_event_cb(hci_event_type *event_data);
err_t hci_qos_violation_event_cb(hci_event_type *event_data);
err_t hci_page_scan_mode_change_event_cb(hci_event_type *event_data);
err_t hci_page_scan_repetition_mode_change_event_cb(hci_event_type *event_data);

extern struct cb_events_item cb_events_array[] ;

connection_type connection_array[MAX_HCI_CONNECTION_HANDLES];

#endif  /*__HCI_H__*/


