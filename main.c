/*
 * =====================================================================================
 *
 *       Filename:  main.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  01-06-2009 21:55:12
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Luis Marcilio Braga de Oliveira (Eng. Elet.), luismarcilio@gmail.com
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <string.h>
#include "hci.h"
#include "l2cap.h"
#include "btusb.h"
#include "hidbt.h"
#include "types.h"

int main(int argc, char *argv[])
{
    btusb_init();
    hci_reset();
    hidbt();
    usb_read_hci_event();
    sleep(1);
    hci_write_scan_enable(0x03);
    usb_read_hci_event();
    sleep(1);
    hci_change_local_name(FRIENDLY_NAME, 1 + strlen(FRIENDLY_NAME));
    usb_read_hci_event();
    sleep(1);
    hci_read_local_name();
    while (1)
    {
        usb_read_hci_event();
        usb_read_acl_data();
    }
    return 0;
}
