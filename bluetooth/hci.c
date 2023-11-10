/*
 * =====================================================================================
 *
 *       Filename:  hci.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/01/09 12:50:09
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Luis Marcilio Braga de Oliveira (), lmbraga@embratel.com.br
 *        Company:  EMBRATEL S.A.
 *
 * =====================================================================================
 */

#include <string.h>
#include <stdlib.h>
#include "hci.h"
#include "types.h"
#include "debug.h"
#include "l2cap.h"

struct cb_events_item cb_events_array[] = 
{
//    { HCI_INQUIRY_COMPLETE_EVENT, hci_inquiry_complete_event_cb }, 
//    { HCI_INQUIRY_RESULT_EVENT, hci_inquiry_result_event_cb }, 
    { HCI_CONNECTION_COMPLETE_EVENT, hci_connection_complete_event_cb }, 
    { HCI_CONNECTION_REQUEST_EVENT, hci_connection_request_event_cb }, 
//    { HCI_DISCONNECTION_COMPLETE_EVENT, hci_disconnection_complete_event_cb }, 
//    { HCI_AUTHENTICATION_COMPLETE_EVENT, hci_authentication_complete_event_cb }, 
//    { HCI_REMOTE_NAME_REQUEST_COMPLETE_EVENT, hci_remote_name_request_complete_event_cb }, 
//    { HCI_ENCRYPTION_CHANGE_EVENT, hci_encryption_change_event_cb }, 
//    { HCI_CHANGE_CONNECTION_LINK_KEY_COMPLETE_EVENT, hci_change_connection_link_key_complete_event_cb }, 
//    { HCI_MASTER_LINK_KEY_COMPLETE_EVENT, hci_master_link_key_complete_event_cb }, 
//    { HCI_READ_REMOTE_SUPPORTED_FEATURES_COMPLETE_EVENT, hci_read_remote_supported_features_complete_event_cb }, 
//    { HCI_READ_REMOTE_VERSION_COMPLETE_EVENT, hci_read_remote_version_complete_event_cb }, 
//    { HCI_Q0S_SETUP_COMPLETE_EVENT, hci_qos_setup_complete_event_cb }, 
    { HCI_COMMAND_COMPLETE_EVENT, hci_command_complete_event_cb }, 
//    { HCI_COMMAND_STATUS_EVENT, hci_command_status_event_cb }, 
//    { HCI_HARDWARE_ERROR_EVENT, hci_hardware_error_event_cb }, 
//    { HCI_FLUSH_OCCURED_EVENT, hci_flush_occured_event_cb }, 
//    { HCI_ROLE_CHANGE_EVENT, hci_role_change_event_cb }, 
//    { HCI_NUMBER_OF_COMPLETED_PACKETS_EVENT, hci_number_of_completed_packets_event_cb }, 
//    { HCI_MODE_CHANGE_EVENT, hci_mode_change_event_cb }, 
//    { HCI_RETURN_LINK_KEYS_EVENT, hci_return_link_keys_event_cb }, 
    { HCI_PIN_CODE_REQUEST_EVENT, hci_pin_code_request_event_cb }, 
    { HCI_LINK_KEY_REQUEST_EVENT, hci_link_key_request_event_cb }, 
//    { HCI_LINK_KEY_NOTIFICATION_EVENT, hci_link_key_notification_event_cb }, 
//    { HCI_LOOPBACK_COMMAND_EVENT, hci_loopback_command_event_cb }, 
//    { HCI_DATA_BUFFER_OVERFLOW_EVENT, hci_data_buffer_overflow_event_cb }, 
//    { HCI_MAX_SLOTS_CHANGE_EVENT, hci_max_slots_change_event_cb }, 
//    { HCI_READ_CLOCK_OFFSET_COMPLETE_EVENT, hci_read_clock_offset_complete_event_cb }, 
//    { HCI_CONNECTION_PACKET_TYPE_CHANGED_EVENT, hci_connection_packet_type_changed_event_cb }, 
//    { HCI_QOS_VIOLATION_EVENT, hci_qos_violation_event_cb }, 
//    { HCI_PAGE_SCAN_MODE_CHANGE_EVENT, hci_page_scan_mode_change_event_cb }, 
//    { HCI_PAGE_SCAN_REPETITION_MODE_CHANGE_EVENT, hci_page_scan_repetition_mode_change_event_cb }, 
    {0x00, NULL}
};
static const bd_addr_t bd_addr_zero={'\0','\0','\0','\0','\0','\0'};

err_t hci_pin_code_request_reply(bd_addr_t *addr, char *pin, uint8_t size)
{
    hci_command_type command;
    uint8_t param[6+1+16]; //BD_ADDR+PIN_Code_Length+PIN_Code
    memcpy(param,addr,6);
    memcpy(&(param[6]),&size,1);
    memcpy(&(param[7]),pin, size);
    ASSEMBLE_HCI_COMMAND(LINK_CONTROL_COMMAND ,HCI_PIN_CODE_REQUEST_REPLY , 6+1+16, param, command);
    return send_hci_command(&command, NULL, NULL);
}

err_t hci_write_scan_enable(uint8_t scan_enable)
{
    hci_command_type command;
    ASSEMBLE_HCI_COMMAND(CONTROLLER_BASEBAND_COMMAND ,HCI_WRITE_SCAN_ENABLE , 1, &scan_enable, command);
    return send_hci_command(&command, NULL, NULL);
}



err_t hci_change_local_name(char *name,uint8_t size)
{
    hci_command_type command;
    ASSEMBLE_HCI_COMMAND(CONTROLLER_BASEBAND_COMMAND ,HCI_CHANGE_LOCAL_NAME , size, (uint8_t *) name, command);
    return send_hci_command(&command, NULL, NULL);
}

err_t hci_reset(void)
{
    hci_command_type command;
    ASSEMBLE_HCI_COMMAND(CONTROLLER_BASEBAND_COMMAND ,HCI_RESET, 0, NULL, command);
    return send_hci_command(&command, NULL, NULL);
}

err_t hci_read_local_name(void)
{
    hci_command_type command;
    ASSEMBLE_HCI_COMMAND(CONTROLLER_BASEBAND_COMMAND ,HCI_READ_LOCAL_NAME, 0, NULL, command);
    return send_hci_command(&command, NULL, NULL);
}

err_t send_hci_command(hci_command_type *command, 
                       err_t (* hci_command_complete_cb)(hci_event_type *event_data),
                       err_t (* hci_command_status_cb)  (hci_event_type *event_data))
{
    struct cb_events_item *cb_event;
    DBG("%s: OCF=0x%02x, OGF=0x%02x\n", __func__, command->OpCode.ocf, command->OpCode.ogf);
    // Adjust The callbacks
    if(hci_command_complete_cb || hci_command_status_cb)
        for(cb_event=cb_events_array; cb_event->event; cb_event++)
        {
            switch(cb_event->event)
            {
                case HCI_COMMAND_COMPLETE_EVENT:
                    if(hci_command_complete_cb)
                        cb_event->callback = hci_command_complete_cb;
                    break;
                case HCI_COMMAND_STATUS_EVENT:
                    if(hci_command_status_cb)
                        cb_event->callback = hci_command_status_cb;
                    break;
            }
        }
    return usb_send_hci_command(command);

}

/*-----------------------------------------------------------------------------
 *  EVENTS
 *-----------------------------------------------------------------------------*/



err_t handle_hci_event(hci_event_type *event)
{
    struct cb_events_item *cb_event;
    err_t status;
    DBG("event->event: %s[0x%02x]\n",event_descriptions[event->eventCode],event->eventCode);
    for(cb_event=cb_events_array; cb_event->event; cb_event++)
    {
        if(cb_event->event == event->eventCode)
        {
            status = cb_event->callback(event);
            break;
        }
    }
#ifdef DEBUG
    if(!cb_event->event) //Handle evento generico
    {
        DBG("event->event: [0x%02x] ignorado.\n",event->eventCode);
    }
#endif
    return status;
}

err_t hci_command_complete_event_cb(hci_event_type *event_data)
{
#if 0
    struct
    {
        uint8_t num_hci_command_packets;
        struct
        {
            uint16_t ocf : 10;
            uint16_t ogf : 6;
        } OpCode;
        uint8_t*    param;
    } comm_complete_ev_data;
    comm_complete_ev_data.num_hci_command_packets = event_data->param[0];
    memcpy(&(comm_complete_ev_data.OpCode), &(event_data->param[1]),2);
    comm_complete_ev_data.param = &(event_data->param[3]);
    DBG("Command complete: ocf=0x%02x, ogf=0x%02x, Num_HCI_Command_Packets=%d\n",
                comm_complete_ev_data.comm_complete_ev_data,
                comm_complete_ev_data.OpCode.ocf,
                comm_complete_ev_data.OpCode.ogf);

#endif
    return SUCCESS;
}


err_t hci_connection_complete_event_cb(hci_event_type *event_data)
{
    extern int bulk_input_pend;
    uint8_t i;
    connection_type *connection;
/*     Event Parameters:
 *         Status: Size: 1 Octet
 *             0x00 Connection successfully completed.
 *             0x01-0xFF Connection failed to Complete. See ¡ on page 331 [Part D]
 *         Connection_Handle: Size: 2 Octets (12 Bits meaningful)
 *             0xXXXX Connection Handle to be used to identify a connection between two Bluetooth
 *             devices. The Connection Handle is used as an identifier for transmitting
 *             and receiving voice or data.
 *         BD_ADDR: Size: 6 Octets
 *             0xXXXXXXXXXXXX BD_ADDR of the other connected Device forming the connection.
 *         Link_Type: Size: 1 Octet
 *             0x00 SCO connection.
 *             0x01 ACL connection (Data Channels).
 *             0x02-0xFF Reserved for future use.
 *         Encryption_Enabled: Size: 1 Octet
 *             0x00 Link level encryption disabled.
 *             0x01 Link level encryption enabled.
 *             0x02-0xFF Reserved for future use.
 */

    //Chain the connection
    // Search for an empty place
    for(i=0; i<MAX_HCI_CONNECTION_HANDLES ; i++) 
    {
        if(!memcmp(&(connection_array[i].bd_addr),&bd_addr_zero, sizeof(bd_addr_t)))
            break;
    }
    if(i==MAX_HCI_CONNECTION_HANDLES)
    {
        return MEMORY_ERROR;
    }

    // Search for the last record. As we tested array boundarys before, we don't need to repeat the test.
    connection=&(connection_array[i]);
    for(i=0; connection_array[i].next; i++);
    DBG("\nConnection complete. Connection number [%d]\n",i);

    connection->status = event_data->param[0];
    memcpy(&(connection->handle), &(event_data->param[1]), 2);
    memcpy(&(connection->bd_addr), &(event_data->param[2]), 6);
    connection_array[i].next = connection;
    connection->next = NULL;
    //Start listening to the ACL packets;
    bulk_input_pend=0;
    usb_read_acl_data();

    return(SUCCESS);

} 

err_t hci_link_key_request_event_cb(hci_event_type *event_data)
{
    bd_addr_t *bd_addr;
    hci_command_type command;
    bd_addr = (bd_addr_t *)event_data->param;
    ASSEMBLE_HCI_COMMAND(LINK_CONTROL_COMMAND ,
                         HCI_LINK_KEY_REQUEST_NEGATIVE_REPLY, 
                         (uint16_t) sizeof(bd_addr_t),
                         (uint8_t *)bd_addr, 
                         command);
    return send_hci_command(&command, NULL, NULL);
}

err_t hci_pin_code_request_event_cb(hci_event_type *event_data)
{

    bd_addr_t *bd_addr;
    bd_addr = (bd_addr_t *)event_data->param;
    return hci_pin_code_request_reply(bd_addr, PIN_CODE, strlen(PIN_CODE));
}


err_t hci_connection_request_event_cb(hci_event_type *event_data)
{
    uint8_t data[7];
    hci_command_type command;
    memcpy(data,event_data->param,6); //bd_addr
    data[6]= 0x01; //Remain the Slave for this connection
    ASSEMBLE_HCI_COMMAND(LINK_CONTROL_COMMAND ,
                         HCI_ACCEPT_CONNECTION_REQUEST , 
                         1+(uint16_t) sizeof(bd_addr_t),
                         (uint8_t *)data, 
                         command);
    return send_hci_command(&command, NULL, NULL);
}

/*-----------------------------------------------------------------------------
 *  ACL DATA PACKAGES
 *-----------------------------------------------------------------------------*/

err_t handle_acl_package(acl_packet_type *acl_packet)
{
    // Search for the correct connection handle
    connection_type *connection= NULL;
    acl_packet_type acl_packet_out;
    uint8_t i;
    DBG("handle_acl_package!!!!!\n");

    for(connection = connection_array;
        connection && (connection->handle != acl_packet->handle);
        connection = connection->next);
    if(! connection)
    {
#ifdef DEBUG
        DBG("ACL package to non existent connection [%d]\n",acl_packet->handle);
#endif
        return HCI_ERROR;
    }
    // Forward packet to upper layer
    DBG("ACL package to upper layer [%d]\n",acl_packet->handle);
    l2cap_handle_packet(acl_packet,connection);

    // Respond the result to the device
    acl_packet_out.handle = connection->handle;
    // TODO BP AND BP FLAGS
    acl_packet_out.pbFlag=0x02;
    acl_packet_out.bcFlag=0x00;
    for(i=0;i<  connection->n_packets_out; i++)
    {
        acl_packet_out.length = connection->l2cap_packet_out[i].length+4;
        memcpy((acl_packet_out.payload),&(connection->l2cap_packet_out[i]),4);
        memcpy(&(acl_packet_out.payload[4]),connection->l2cap_packet_out[i].payload,
                                            connection->l2cap_packet_out[i].length);

        send_acl_packet(&acl_packet_out);
    }
    return SUCCESS;
}




