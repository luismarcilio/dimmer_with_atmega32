/*
 * =====================================================================================
 *
 *       Filename:  usbif.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/30/2009 08:22:49 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Luis Marcilio Braga de Oliveira (Eng. Elet.), luismarcilio@gmail.com
 *        Company:  Universidade Estacio de Sa
 *
 * =====================================================================================
 */

#ifndef __USBIF_H__
#define __USBIF_H__

/* The class code (bDeviceClass) is 0xE0 – Wireless Controller.
 * The SubClass code (bDeviceSubClass) is 0x01 – RF Controller.
 * The Protocol code (bDeviceProtocol) is 0x01 – Bluetooth programming.
 * 
 */

#include <libusb-1.0/libusb.h>
#include "l2cap.h"
#include "types.h"
#include "hci.h"


#define BLUETOOTH_ID_PRODUCT 0x2100
#define BROADCOMM_ID_VENDOR  0x0A5C
#define BLUETOOTH_CLASS 0XE0
#define BLUETOOTH_SUBCLASS 0X01
#define BLUETOOTH_PROTOCOL 0X01

typedef struct
{
    uint8_t intr_ep;
    uint8_t bulk_tx_ep;
    uint8_t bulk_rx_ep;

    
} control_ep;

typedef struct
{
    int16_t missing;
    union
    {
        hci_event_type event;
        acl_packet_type acl_package;
    } pkg;
} hci_recv_type;


err_t usb_send_hci_command(hci_command_type *command);
static void transfer_complete(struct libusb_transfer *transfer);
void  btusb_init(void);

#endif
