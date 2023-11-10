/*
 * =====================================================================================
 *
 *       Filename:  usbif.c
 *
 *    Description:  Implementation of the HCI USB transport layer for Linux
 *
 *        Version:  1.0
 *        Created:  04/30/2009 08:19:56 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Luis Marcilio Braga de Oliveira (Eng. Elet.), luismarcilio@gmail.com
 *
 * =====================================================================================
 */

#include <stdlib.h>
#include <string.h>
#include "debug.h"
#include "btusb.h"
#include "hci.h"

libusb_device_handle *dh;
libusb_context *ctx;
control_ep ep;
libusb_device *dev;
int int_input_pend=0;
int bulk_input_pend=1;

#ifdef DEBUG
static void print_transfer(struct libusb_transfer *transfer)
{
    int i;
    int l;
    printf("        endpoint: 0x%02x\n",transfer->endpoint);
    printf("        actual_length: %d\n",transfer->actual_length);
    printf("        endpoint_type: ");
    switch(transfer->type)
    {
        case LIBUSB_TRANSFER_TYPE_CONTROL:
            printf("LIBUSB_TRANSFER_TYPE_CONTROL\n");
            break;
        case LIBUSB_TRANSFER_TYPE_ISOCHRONOUS:
            printf("LIBUSB_TRANSFER_TYPE_ISOCHRONOUS\n");
            break;
        case LIBUSB_TRANSFER_TYPE_BULK:
            printf("LIBUSB_TRANSFER_TYPE_BULK\n");
            break;
        case LIBUSB_TRANSFER_TYPE_INTERRUPT:
            printf("LIBUSB_TRANSFER_TYPE_INTERRUPT\n");
            break;
    }
    printf("        ");
    l=transfer->type == LIBUSB_TRANSFER_TYPE_CONTROL?transfer->length
                                                    :transfer->length;
    for(i=0; i<l; i++)
    {
        unsigned int c;
            c=(((char *)(transfer->buffer))[i]&0x000000ff) ;

        printf("%02X ",c);
        if((i+1)%16 == 0)
            printf("\n        ");
    }
    printf("\n");
}
#endif


void  btusb_init(void)
{

    libusb_init(&ctx);
#ifdef DEBUG
    libusb_set_debug(ctx, 3);
#endif
    if((dh=libusb_open_device_with_vid_pid(ctx,BROADCOMM_ID_VENDOR,BLUETOOTH_ID_PRODUCT))== NULL)
    {
         DBG("DISPOSITIVO(0X%X,0X%X) NAO ENCONTRADO\n",BROADCOMM_ID_VENDOR,BLUETOOTH_ID_PRODUCT);
         return;
    }
    if((dev=libusb_get_device(dh))==NULL)
    {
         DBG("libusb_get_device");
         return;
    }
    if(libusb_open(dev,&dh))
    {
         DBG("libusb_open");
         return;
    }
    libusb_reset_device(dh);
    if(libusb_open(dev,&dh))
    {
         DBG("libusb_open");
         return;
    }
    if(libusb_claim_interface(dh,0))
    {
         DBG("libusb_claim_interface\n");
         perror("libusb_claim_interface");
         return;
    }
    ep.intr_ep=0x81;
    ep.bulk_tx_ep=0x02;
    ep.bulk_rx_ep=0x82;

    
}

static void acl_transfer_complete(struct libusb_transfer *transfer)
{
    hci_recv_type *recv;
    static uint8_t *payload_ptr;

#ifdef DEBUG
    DBG("print_transfer\n");
    print_transfer(transfer);
#endif
    bulk_input_pend=0;
    recv = transfer->user_data;
    if(recv->pkg.acl_package.handle == 0) //Beginning of HCI acl data package
    {
        memcpy(&(recv->pkg.acl_package),transfer->buffer,2);
        // ENDIANESS 
        memcpy(&(recv->pkg.acl_package.length), &(transfer->buffer[2]),2);
        recv->pkg.acl_package.length>>8;
        DBG("recv->pkg.acl_package.length=%d\n",recv->pkg.acl_package.length);
        payload_ptr = recv->pkg.acl_package.payload;
        memcpy(payload_ptr, &(transfer->buffer[4]), transfer->actual_length);
        payload_ptr += transfer->actual_length-4;
        recv->missing=recv->pkg.acl_package.length+4-transfer->actual_length;
    }
    else //Continuation of last HCI acl data package
    {
        memcpy(payload_ptr, transfer->buffer, transfer->actual_length);
        payload_ptr += transfer->actual_length;
        recv->missing-=transfer->actual_length;
    }
    return ;

}

static void interrupt_transfer_complete(struct libusb_transfer *transfer)
{
    hci_recv_type *recv;
    static uint8_t *param_ptr;

#ifdef DEBUG
    DBG("print_transfer\n");
    print_transfer(transfer);
#endif
    int_input_pend=0;
    recv = transfer->user_data;
    if(recv->pkg.event.eventCode == 0) //Beginning of HCI data package
    {
        recv->pkg.event.eventCode = transfer->buffer[0];
        recv->pkg.event.length = transfer->buffer[1];
        recv->pkg.event.param = (uint8_t *) malloc(recv->pkg.event.length);
        param_ptr = recv->pkg.event.param;
        memcpy(param_ptr, &(transfer->buffer[2]), transfer->actual_length);
        param_ptr += transfer->actual_length-2;
        recv->missing=recv->pkg.event.length+2-transfer->actual_length;
    }
    else //Continuation of last HCI data package
    {
        memcpy(param_ptr, transfer->buffer, transfer->actual_length);
        param_ptr += transfer->actual_length;
        recv->missing-=transfer->actual_length;
    }
    return ;

}

static void transfer_complete(struct libusb_transfer *transfer)
{
#ifdef DEBUG
    DBG("print_transfer\n");
    print_transfer(transfer);
#endif
}

err_t usb_send_hci_command(hci_command_type *command)
{
    uint8_t * buffer;
    uint8_t len;
    uint8_t status;
    struct libusb_transfer *usbt=NULL;

    len = command->length+3;
    buffer = (uint8_t *) malloc(LIBUSB_CONTROL_SETUP_SIZE + command->length+3);
    usbt = libusb_alloc_transfer(0);
    libusb_fill_control_setup(buffer /* data */
                            , LIBUSB_REQUEST_TYPE_CLASS  /* bmRequestType */
                            ,0                           /* bRequest */
                            ,0                           /* wValue */
                            ,0                           /* wIndex */
                            ,len                          /* wLength */
                            );
    memcpy(&(buffer[LIBUSB_CONTROL_SETUP_SIZE]), (uint8_t *)&(command->OpCode), 2);
    memcpy(&(buffer[LIBUSB_CONTROL_SETUP_SIZE+2]), (uint8_t *)&(command->length), 1);
    if(command->length)
        memcpy(&(buffer[LIBUSB_CONTROL_SETUP_SIZE+3]), command->param, command->length);

    libusb_fill_control_transfer(usbt /* transfer */
                                 ,dh  /* dev_handle */
                                 ,buffer /* buffer */
                                 ,transfer_complete /* callback */
                                 ,command /* user_data */
                                 ,0 /* timeout */ );
    status = libusb_submit_transfer(usbt);
    libusb_handle_events(ctx);
    free(buffer);

    return SUCCESS;
}

err_t usb_read_hci_event(void)
{
    uint8_t data[64];
    struct libusb_transfer *interrupt_transfer=NULL;
    hci_recv_type recv;
    uint8_t try=0;

    bzero(&recv,sizeof(hci_recv_type));
    do{
        if(!int_input_pend)
        {
            interrupt_transfer = libusb_alloc_transfer(0);
            libusb_fill_interrupt_transfer( interrupt_transfer           //struct libusb_transfer *        transfer,
                                            ,dh                          //libusb_device_handle *      dev_handle,
                                            ,ep.intr_ep                  //unsigned char      endpoint,
                                            ,data                        //unsigned char *      buffer,
                                            ,64                          //int      length,
                                            ,interrupt_transfer_complete //libusb_transfer_cb_fn      callback,
                                            ,&recv                      //void *      user_data,
                                            ,0                           //unsigned int      timeout     
                                          );     
            libusb_submit_transfer(interrupt_transfer);
            int_input_pend++;
        }
        libusb_handle_events(ctx);
    }while((recv.missing > 0) && (try <= 20)); //TODO -> adjust this timeout procedure
    if((try <= 20) && (recv.pkg.event.length > 0))
    {
        handle_hci_event(&(recv.pkg.event));
    }
    else
    {
        free(recv.pkg.event.param);
        return HCI_ERROR;
    }
    //free(recv.pkg.event.param);
    return SUCCESS;
}

err_t usb_read_acl_data(void)
{
    uint8_t data[64];
    struct libusb_transfer *bulk_transfer=NULL;
    hci_recv_type recv;
    uint8_t try=0;

    if(bulk_input_pend)
    return SUCCESS;

    bzero(&recv,sizeof(hci_recv_type));
    do{
        if(!bulk_input_pend)
        {
            bulk_transfer = libusb_alloc_transfer(0);
            libusb_fill_bulk_transfer( bulk_transfer         //struct libusb_transfer *        transfer,
                                ,dh                          //libusb_device_handle *      dev_handle,
                                ,ep.bulk_rx_ep               //unsigned char      endpoint,
                                ,data                        //unsigned char *      buffer,
                                ,64                          //int      length,
                                ,acl_transfer_complete       //libusb_transfer_cb_fn      callback,
                                ,&recv                       //void *      user_data,
                                ,0                           //unsigned int      timeout     
            );     
            libusb_submit_transfer(bulk_transfer);
            bulk_input_pend++;
        }
        libusb_handle_events(ctx);
        DBG("recv.missing = %d\n",recv.missing );
    }while((recv.missing > 0) && (try <= 2*(L2CAP_MTU/64))); //TODO -> adjust this timeout procedure
    if(try <= 2*(L2CAP_MTU/64))
    {
        DBG("indo para  handle_acl_package\n");
        handle_acl_package(&(recv.pkg.acl_package));
    }
    else
    {
        DBG("HCI_ERROR\n");
        free(recv.pkg.event.param);
        return HCI_ERROR;
    }
    //free(recv.pkg.event.param);
    return SUCCESS;
}


err_t send_acl_packet(acl_packet_type *acl_packet)
{
    uint8_t data[64];
    uint8_t *data_ptr;
    struct libusb_transfer *bulk_transfer=NULL;
    uint8_t missing=0;
    uint8_t length=0;

    //Assemble l2cap_packet header

    memcpy(data, acl_packet, 4);
    if(acl_packet->length>(64-4))
    {
        memcpy(&(data[4]),acl_packet->payload,64-4);
        missing=acl_packet->length-(64-4);
        data_ptr=&(acl_packet->payload[64-4]);
        length=64;
    }
    else
    {
        memcpy(&(data[4]),acl_packet->payload,acl_packet->length);
        length=acl_packet->length+4;
        missing=length;
    }

    while(missing){
        bulk_transfer = libusb_alloc_transfer(0);
        libusb_fill_bulk_transfer( bulk_transfer         //struct libusb_transfer *        transfer,
                            ,dh                          //libusb_device_handle *      dev_handle,
                            ,ep.bulk_tx_ep               //unsigned char      endpoint,
                            ,data                        //unsigned char *      buffer,
                            ,length                          //int      length,
                            ,transfer_complete       //libusb_transfer_cb_fn      callback,
                            ,NULL                        //void *      user_data,
                            ,0                           //unsigned int      timeout     
        );     
        libusb_submit_transfer(bulk_transfer);
        libusb_handle_events(ctx);
        if(missing > 64) 
        {
            memcpy(data, data_ptr+=64, 64);
            missing -= 64;
        }
        else
        {
            //memcpy(data, data_ptr+=missing, missing);
            missing=0;
        }

    }
    bulk_input_pend=0;
    return SUCCESS;
}

