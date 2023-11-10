/*
 * =====================================================================================
 *
 *       Filename:  hidbt.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05-06-2009 23:16:54
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Luis Marcilio Braga de Oliveira (Eng. Elet.), luismarcilio@gmail.com
 *        Company:  Universidade Estácio de Sá
 *
 * =====================================================================================
 */

#include <stdint.h>
#include "hci.h"
#include "hidbt.h"
#include "l2cap.h"
#include "config.h"
#include "types.h"



err_t listener_int(uint8_t *data, uint16_t *status)
{

    l2cap_packet_type *l2cap_packet_in;
    uint8_t i;
    l2cap_packet_in = (l2cap_packet_type *)data;

    DBG("length=[%d]\n",l2cap_packet_in->length);
    DBG("cid=[%d]\n",l2cap_packet_in->cid);
    printf("        ");
    for ( i=0; i<l2cap_packet_in->length; i++)
    {
        printf("0x%02X ",((uint8_t *)l2cap_packet_in->payload)[i]);
        if(! (i+1)%8)
            printf("\n        ");

    }
    return SUCCESS;
}

err_t listener_ctrl(uint8_t *data, uint16_t *status)
{
    l2cap_packet_type *l2cap_packet_in;
    uint8_t i;
    uint8_t hdr, type, param;


    l2cap_packet_in = (l2cap_packet_type *)data;

    DBG("length=[%d]\n",l2cap_packet_in->length);
    DBG("cid=[%d]\n",l2cap_packet_in->cid);
	hdr = l2cap_packet_in->payload[0];

	type = hdr & HIDP_HEADER_TRANS_MASK;
	param = hdr & HIDP_HEADER_PARAM_MASK;

	switch (type) {
	case HIDP_TRANS_HANDSHAKE:
		break;

	case HIDP_TRANS_HID_CONTROL:
		break;

	case HIDP_TRANS_DATA:
		hidp_process_data(&(l2cap_packet_in->payload[1]), param);
		break;

    }


    return SUCCESS;
}


static inline void hidp_process_data(uint8_t *data, uint8_t param)
{
    switch (param) {
	case HIDP_DATA_RTYPE_INPUT:
			hidp_input_report(data);
		break;
	case HIDP_DATA_RTYPE_OTHER:
	case HIDP_DATA_RTYPE_OUPUT:
	case HIDP_DATA_RTYPE_FEATURE:
		break;
    }
    return;
}


static void hidp_input_report(uint8_t *data)
{
    int i;
    switch (data[0])
    {
        case 0x01:	/* Keyboard report */

                    /* The following table represents the keyboard input report (8 bytes).
                     * Byte Description
                     * 0 Modifier keys
                     * 1 Reserved
                     * 2 Keycode 1
                     * 3 Keycode 2
                     * 4 Keycode 3
                     * 5 Keycode 4
                     * 6 Keycode 5
                     * 7 Keycode 6
                     * 
                     * The following table represents the keyboard output report (1 byte).
                     * Bit Description
                     * 0 NUM LOCK
                     * 1 CAPS LOCK
                     * 2 SCROLL LOCK
                     * 3 COMPOSE
                     * 4 KANA
                     * 5 to 7 CONSTANT
                     */

            DBG("KEYBOARD REPORT. byte 0:[0x%02x]\n", data[1]);
            for(i=2; i<8; i++)
                DBG("KEYBOARD REPORT. keycode %d:[0x%02x]=%c\n", i-1, data[i], data[i],data[i]);
            break;
        case 0x02:	/* mouse report */
            DBG("MOUSE REPORT. byte 0:[0x%02x]\n", data[1]);
            DBG("MOUSE REPORT. byte 1:[0x%02x]\n", data[2]);
            DBG("MOUSE REPORT. byte 2:[0x%02x]\n", data[3]);
        break;
    }

}

err_t hidbt(void)
{
    l2cap_listen(0x11, listener_int);
    l2cap_listen(0x13, listener_ctrl);
    return SUCCESS;
}
