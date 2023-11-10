/*
 * =====================================================================================
 *
 *       Filename:  types.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/01/09 10:32:00
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Luis Marcilio Braga de Oliveira (), lmbraga@embratel.com.br
 *        Company:  EMBRATEL S.A.
 *
 * =====================================================================================
 */

#ifndef  __TYPES_H__
#define  __TYPES_H__

#include <stdint.h>
#include <stdio.h>
//typedef unsigned char uint8_t;
//typedef unsigned short int uint16_t;
//typedef unsigned long int uint32_t;


typedef enum err_t
{
    SUCCESS
    ,UNKNOWN_ERROR
    ,MEMORY_ERROR
    ,HARDWARE_ERROR
    ,USB_ERROR
    ,HCI_ERROR
    ,L2CAP_ERROR
    ,HID_ERROR
} err_t;

typedef struct
{
    uint8_t addr[6];
} bd_addr_t;

#endif  /*__types.h__*/


