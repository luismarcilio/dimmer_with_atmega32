/*
 * =====================================================================================
 *
 *       Filename:  l2cap.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/06/2009 16:57:06
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */

#ifndef __L2CAP_H__
#define __L2CAP_H__

#include "config.h"
#include "types.h"

// L2CAP ASSIGNED PSM
#define SDP              0x0001 //See Bluetooth Service Discovery Protocol (SDP), Bluetooth SIG.
#define RFCOMM           0x0003 //See RFCOMM with TS 07.10, Bluetooth SIG.
#define TCS_BIN          0x0005 //See Bluetooth Telephony Control Specification / TCS Binary, Bluetooth SIG.
#define TCS_BIN_CORDLESS 0x0007 //See Bluetooth Telephony Control Specification / TCS Binary, Bluetooth SIG.
#define BNEP             0x000F //See Bluetooth Network Encapsulation Protocal, Bluetooth SIG.
#define HID_CONTROL      0x0011 //See Human Interface Device , Bluetooth SIG.
#define HID_INTERRUPT    0x0013 //See Human Interface Device, Bluetooth SIG.
#define UPNP             0x0015 //See [ESDP] , Bluetooth SIG.
#define AVCTP            0x0017 //See Audio/Video Control Transport Protocol , Bluetooth SIG.
#define AVDTP            0x0019 //See Audio/Video Distribution Transport Protocol , Bluetooth SIG.
#define AVCTP_BROWSING   0x001B //See Audio/Video Remote Control Profile, Bluetooth SIG
#define UDI_C_PLANE      0x001D //See the Unrestricted Digital Information Profile [UDI], Bluetooth SIG


// Signalling package codes
#define L2CAP_RESERVED 0X00
#define L2CAP_COMMAND_REJECT 0X01
#define L2CAP_CONNECTION_REQUEST 0X02
#define L2CAP_CONNECTION_RESPONSE 0X03
#define L2CAP_CONFIGURE_REQUEST 0X04
#define L2CAP_CONFIGURE_RESPONSE 0X05
#define L2CAP_DISCONNECTION_REQUEST 0X06
#define L2CAP_DISCONNECTION_RESPONSE 0X07
#define L2CAP_ECHO_REQUEST 0X08
#define L2CAP_ECHO_RESPONSE 0X09
#define L2CAP_INFORMATION_REQUEST 0X0A
#define L2CAP_INFORMATION_RESPONSE 0X0B

//L2cap packet struct
typedef struct
{
    uint32_t length:16;
    uint32_t cid:16;
    uint8_t *payload;
} l2cap_packet_type;

//l2cap command struct
typedef struct
{
    uint32_t code:8;
    uint32_t id:8;
    uint32_t length:16;
    uint8_t *data;
} l2cap_command_type;

// States for the state machine

/*  CLOSED  channel not connected.
 *  WAIT_CONNECT  a connection request has been received, but only a
 * connection response with indication pending can be sent.
 *  WAIT_CONNECT_RSP  a connection request has been sent, pending a
 * positive connect response.
 *  CONFIG  the different options are being negotiated for both sides; this
 * state comprises a number of substates, see Section 6.1.3 on page 67
 *  OPEN  user data transfer state.
 *  WAIT_DISCONNECT  a disconnect request has been sent, pending a disconnect
 * response.
 * 
 */


enum l2cap_state_machine { 
    CLOSED, 
    WAIT_CONNECT,
    WAIT_CONNECT_RSP,
    WAIT_CONFIG,
    WAIT_SEND_CONFIG,
    WAIT_CONFIG_REQ_RSP,
    WAIT_CONFIG_RSP,
    WAIT_CONFIG_REQ,
    OPEN,
    WAIT_DISCONNECT
};

// Callback struct for the listening CIDs
typedef struct
{
    uint16_t psm;
    err_t (* callback)(uint8_t *data, uint16_t *status);
} l2cap_callbacks_type;

// array of callbacks for the listening CIDs
extern l2cap_callbacks_type l2cap_callbacks_array[MAX_L2CAP_CONNECTION_CBS];

// struct of L2CAP connections
typedef struct
{
    uint16_t dcid;
    uint16_t scid;
    uint8_t  state;
    err_t (* callback)(uint8_t *data, uint16_t *status);
} l2cap_connection_type;

//command structures
    typedef struct 
    {
        uint32_t code:8;
        uint32_t identifier:8;
        uint32_t length:16;
        uint16_t psm;
        uint16_t scid;
    } l2cap_connection_request_type;

    typedef struct 
    {
        uint32_t code:8;
        uint32_t identifier:8;
        uint32_t length:16;
        uint16_t dcid;
        uint16_t scid;
        uint16_t result;
        uint16_t status;
    } l2cap_connection_response_type;

typedef struct
{
    uint32_t type:8;
    uint32_t length:8;
    uint32_t mtu:16;
} mtu_option_type;

typedef struct
{
    uint32_t type:8;
    uint32_t length:8;
    uint32_t flush_timeout:16;
} flush_timeout_option_type;

typedef struct
{
    uint32_t type:8;
    uint32_t length:8;
    uint32_t flags:8;
    uint32_t service_type:8;
    uint32_t token_rate;
    uint32_t token_bucket_size;
    uint32_t peak_bw;
    uint32_t latency;
    uint32_t delay_variation;
} qos_option_type;


typedef struct
{
    uint32_t type:8;
    uint32_t length:8;
    uint32_t mode:8;
    uint32_t TxWindowSize:8;
//--------------------------------- first DWORD
    uint32_t max_transmit:8;
    uint32_t retr_timout:16;
    uint32_t monitor_timeout_lsb:8;
//--------------------------------- second DWORD
    uint32_t monitor_timeout_msb:8;
    uint32_t max_pdu:16;
//--------------------------------- third DWORD
} retr_fc_option_type;

    typedef struct 
    {
        uint32_t code:8;
        uint32_t identifier:8;
        uint32_t length:16;
        uint16_t dcid;
        uint16_t flags;
        uint8_t *config_opt;
    } l2cap_configure_request_type;

    typedef struct 
    {
        uint32_t code:8;
        uint32_t identifier:8;
        uint32_t length:16;
        uint16_t scid;
        uint16_t flags;
        uint16_t result;
        uint8_t *config_opt;
    } l2cap_configure_response_type;

// array of callbacks for the listening CIDs
extern l2cap_connection_type l2cap_connections_array[MAX_L2CAP_CONNECTION_HANDLES];

// listener
err_t l2cap_listen(uint16_t psm, err_t(* callback)(uint8_t *data, uint16_t *status));



#endif
