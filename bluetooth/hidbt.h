/*
 * =====================================================================================
 *
 *       Filename:  hidbt.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05-06-2009 23:19:19
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Luis Marcilio Braga de Oliveira (Eng. Elet.), luismarcilio@gmail.com
 *        Company:  Universidade Estácio de Sá
 *
 * =====================================================================================
 */

#ifndef __HIDBT_H__
#define __HIDBT_H__

err_t hidbt(void);

/* HIDP header masks */
#define HIDP_HEADER_TRANS_MASK			0xf0
#define HIDP_HEADER_PARAM_MASK			0x0f

/* HIDP transaction types */
#define HIDP_TRANS_HANDSHAKE			0x00
#define HIDP_TRANS_HID_CONTROL			0x10
#define HIDP_TRANS_GET_REPORT			0x40
#define HIDP_TRANS_SET_REPORT			0x50
#define HIDP_TRANS_GET_PROTOCOL			0x60
#define HIDP_TRANS_SET_PROTOCOL			0x70
#define HIDP_TRANS_GET_IDLE			0x80
#define HIDP_TRANS_SET_IDLE			0x90
#define HIDP_TRANS_DATA				0xa0
#define HIDP_TRANS_DATC				0xb0

/* HIDP handshake results */
#define HIDP_HSHK_SUCCESSFUL			0x00
#define HIDP_HSHK_NOT_READY			0x01
#define HIDP_HSHK_ERR_INVALID_REPORT_ID		0x02
#define HIDP_HSHK_ERR_UNSUPPORTED_REQUEST	0x03
#define HIDP_HSHK_ERR_INVALID_PARAMETER		0x04
#define HIDP_HSHK_ERR_UNKNOWN			0x0e
#define HIDP_HSHK_ERR_FATAL			0x0f

/* HIDP control operation parameters */
#define HIDP_CTRL_NOP				0x00
#define HIDP_CTRL_HARD_RESET			0x01
#define HIDP_CTRL_SOFT_RESET			0x02
#define HIDP_CTRL_SUSPEND			0x03
#define HIDP_CTRL_EXIT_SUSPEND			0x04
#define HIDP_CTRL_VIRTUAL_CABLE_UNPLUG		0x05

/* HIDP data transaction headers */
#define HIDP_DATA_RTYPE_MASK			0x03
#define HIDP_DATA_RSRVD_MASK			0x0c
#define HIDP_DATA_RTYPE_OTHER			0x00
#define HIDP_DATA_RTYPE_INPUT			0x01
#define HIDP_DATA_RTYPE_OUPUT			0x02
#define HIDP_DATA_RTYPE_FEATURE			0x03

/* HIDP protocol header parameters */
#define HIDP_PROTO_BOOT				0x00
#define HIDP_PROTO_REPORT			0x01




static void hidp_input_report(uint8_t *data);
static inline void hidp_process_data(uint8_t *data, uint8_t param);

#endif // __HIDBT_H__
