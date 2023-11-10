/*
 * =====================================================================================
 *
 *       Filename:  l2cap.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/06/2009 17:17:46
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */

#include <string.h>
#include "hci.h"
#include "l2cap.h"
#include "config.h"
#include "types.h"

// FUNCTIONS
err_t handle_connection_request(connection_type *connection, l2cap_command_type *command);
err_t handle_connection_response(connection_type *connection, l2cap_command_type *command);
err_t handle_configure_request(connection_type *connection, l2cap_command_type *command);
err_t handle_configure_response(connection_type *connection, l2cap_command_type *command);
uint16_t verify_l2cap_config_options(l2cap_configure_request_type *l2cap_configure_request);
err_t handle_signalling_package(connection_type *connection);
err_t assemble_l2cap_sig_package(connection_type *connection, l2cap_command_type *command,uint8_t n_packet);
err_t handle_data_package(connection_type *connection);



//GLOBALS

//    Helper variables for Configuration.
//    
//    For Config_response:
//        octet 1      octet2       octet3       octet4
//    +------------+------------+------------+------------+
//    |code        | Identifier |        Length           |
//    +------------+------------+------------+------------+
//    |        Source CID       |        Flags            |
//    +------------+------------+------------+------------+
//    |        Result           |        Config           |
//    +------------+------------+------------+------------+
//    
//    
//    
//    For config requests:
//        octet 1      octet2       octet3       octet4
//    +------------+------------+------------+------------+
//    |code        | Identifier |        Length           |
//    +------------+------------+------------+------------+
//    |        Destination CID  |        Flags            |
//    +------------+------------+------------+------------+
//    |            Configuration Options                  |
//    +------------+------------+------------+------------+
//    
//    As you can see, config options begin two octets later in 
//    config response, so you will see us using &(basic_l2cap_config[2]) below.
static uint8_t basic_l2cap_config[]={
        0x00, 0x00, 0x00, 0x00, // SCID, FLAGS
        0x00, 0x00,             // Result
        0x01, 0x02, 0xa0, 0x02};// MTU -> 672


l2cap_callbacks_type l2cap_callbacks_array[MAX_L2CAP_CONNECTION_CBS];
l2cap_connection_type l2cap_connections_array[MAX_L2CAP_CONNECTION_HANDLES];

static uint8_t useful_array[12];
static l2cap_command_type generic_command;

err_t l2cap_handle_packet(acl_packet_type *acl_packet, connection_type *connection)
{

    l2cap_packet_type *l2cap_packet;
    static uint8_t *payload_ptr;
    static uint16_t transferred;


    // We must reassemble the packet.

    if(!(acl_packet->pbFlag & 1)) // First fragment of the packet
    {
        DBG("first fragment of acl packet\n");
        l2cap_packet = malloc(sizeof(l2cap_packet_type));
        bzero(l2cap_packet,sizeof(l2cap_packet_type));
        transferred = 0;
        memcpy(l2cap_packet,acl_packet->payload,4);
        l2cap_packet->payload = malloc(l2cap_packet->length);
        payload_ptr = l2cap_packet->payload;
        memcpy(payload_ptr, &(acl_packet->payload[4]), acl_packet->length-4);
        transferred += acl_packet->length -4;
        payload_ptr += transferred;
        memcpy(&(connection->l2cap_packet_in[0]),l2cap_packet,sizeof(l2cap_packet_type));
    }
    else// continuing fragment of the packet
    {
        DBG("continuing fragment of acl packet\n");
//        l2cap_packet = connection->l2cap_packet_in;
//        memcpy(payload_ptr, &(acl_packet->payload[3]), acl_packet->length-3);
//        transferred += acl_packet->length -3;
//        payload_ptr += acl_packet->length -3;
//    }
    if(transferred == l2cap_packet->length) // Packet complete. Handle the packet.
    {
        DBG("transfer complete of acl packet\n");
        if(l2cap_packet->cid == 1)
        {
                DBG("signalling packet\n");
                handle_signalling_package(connection);
        }
        else
        {
                DBG("data packet\n");
                handle_data_package(connection);
        }
    }
    return SUCCESS;
}


err_t handle_signalling_package(connection_type *connection)
{
    uint8_t *ptr;
    uint8_t *ptr_tail;
    l2cap_command_type l2cap_command;
    l2cap_command_type *outgoing_command = (l2cap_command_type *)useful_array;
    DBG("\n");

    
/* Multiple commands may have been sent in
 * a single C-frame
 * Implementations must be able to handle the reception of multiple
 * commands in an L2CAP packet.
 */
    //run into the packet to find the commands
    ptr= connection->l2cap_packet_in->payload;
    ptr_tail = ptr+ connection->l2cap_packet_in->length;
    do
    {
        memcpy(&l2cap_command, ptr,4); //copy the header
        ptr+=4;
        l2cap_command.data=ptr;
        ptr+=l2cap_command.length;

        DBG("l2cap_command.code=0x%x\n", l2cap_command.code);
        switch(l2cap_command.code)
        {
            case L2CAP_RESERVED: 
                generic_command.code = L2CAP_COMMAND_REJECT;
                generic_command.id = l2cap_command.id;
                generic_command.length = 0x02;
                useful_array[0]=0x00;//Command not understood
                useful_array[1]=0x00;//Command not understood
                generic_command.data = useful_array; 
                assemble_l2cap_sig_package(connection, &generic_command,1);
                break;
            case L2CAP_COMMAND_REJECT: 
                //handle_command_reject(connection,&l2cap_command);
                break;
            case L2CAP_CONNECTION_REQUEST: 
                handle_connection_request(connection,&l2cap_command);
                break;
            case L2CAP_CONNECTION_RESPONSE: 
                //handle_connection_response(connection,&l2cap_command);
                break;
            case L2CAP_CONFIGURE_REQUEST: 
                handle_configure_request(connection,&l2cap_command);
                break;
            case L2CAP_CONFIGURE_RESPONSE: 
                handle_configure_response(connection,&l2cap_command);
                break;
            case L2CAP_DISCONNECTION_REQUEST: 
                //handle_disconnection_request(connection,&l2cap_command);
                break;
            case L2CAP_DISCONNECTION_RESPONSE: 
                //handle_disconnection_response(connection,&l2cap_command);
                break;
            case L2CAP_ECHO_REQUEST: 
                //handle_echo_request(connection,&l2cap_command);
                break;
            case L2CAP_ECHO_RESPONSE: 
                //handle_echo_response(connection,&l2cap_command);
                break;
            case L2CAP_INFORMATION_REQUEST: 
                //handle_info_request(connection,&l2cap_command);
                break;
            case L2CAP_INFORMATION_RESPONSE: 
                //handle_info_response(connection,&l2cap_command);
                break;
            default:
                break;
                generic_command.code = L2CAP_COMMAND_REJECT;
                generic_command.id = l2cap_command.id;
                generic_command.length = 0x02;
                useful_array[0]=0x00;//Command not understood
                useful_array[1]=0x00;//Command not understood
                generic_command.data = useful_array; 
                assemble_l2cap_sig_package(connection, &generic_command,1);
        }
         
        
    } while(ptr < ptr_tail);

    return SUCCESS;
}

err_t l2cap_listen(uint16_t psm, err_t(* callback)(uint8_t *data, uint16_t *status))
{
    uint8_t i;
    for(i=0; i<MAX_L2CAP_CONNECTION_HANDLES;i++)
    {
        if((l2cap_callbacks_array[i].psm == 0) ||
           (l2cap_callbacks_array[i].psm == psm))
            break;
    }
    if(i==MAX_L2CAP_CONNECTION_HANDLES)
        return L2CAP_ERROR;
    l2cap_callbacks_array[i].psm = psm;
    l2cap_callbacks_array[i].callback = callback;
    return SUCCESS;
}


err_t handle_connection_request(connection_type *connection, l2cap_command_type *command)
{


/* The data fields are:
 * -->Destination CID - DCID (2 octets)
 * This field contains the channel endpoint on the device receiving this
 * Request packet.
 * 
 * -->Flags (2 octets)
 * Only one flag is defined, the Continuation flag (C).
 * When all configuration options cannot fit into a Configuration Request with
 * length that does not exceed the receiver's MTUsig, the options shall be
 * passed in multiple configuration command packets. If all options fit into the
 * receiver's MTUsig, then they shall be sent in a single configuration request
 * with the continuation flag set to zero. Each Configuration Request shall contain
 * an integral number of options - partially formed options shall not be sent
 * in a packet. Each Request shall be tagged with a different Identifier and shall
 * be matched with a Response with the same Identifier.
 * When used in the Configuration Request, the continuation flag indicates the
 * responder should expect to receive multiple request packets. The responder
 * shall reply to each Configuration Request packet. The responder may reply
 * to each Configuration Request with a Configuration Response containing
 * the same option(s) present in the Request (except for those error conditions
 * more appropriate for a Command Reject), or the responder may reply with a
 * "Success" Configuration Response packet containing no options, delaying
 * those options until the full Request has been received. The Configuration
 * Request packet with the continuation flag cleared shall be treated as the
 * Configuration Request event in the channel state machine.
 * When used in the Configuration Response, the continuation flag shall be set
 * to one if the flag is set to one in the Request. If the continuation flag is set to
 * one in the Response when the matching Request has the flag set to zero, it
 * indicates the responder has additional options to send to the requestor. In
 * this situation, the requestor shall send null-option Configuration Requests
 * (with continuation flag set to zero) to the responder until the responder
 * replies with a Configuration Response where the continuation flag is set to
 * zero. The Configuration Response packet with the continuation flag set to
 * zero shall be treated as the Configuration Response event in the channel
 * state machine.
 *
 * The result of the configuration transaction is the union of all the result values.
 * All the result values must succeed for the configuration transaction to
 * succeed.
 *
 * Other flags are reserved and shall be set to zero. L2CAP implementations
 * shall ignore these bits.
 * 
 * -->Configuration Options
 * A list of the parameters and their values to be negotiated shall be provided in
 * the Configuration Options field. These are defined in Section 5 on page 55.
 * A Configuration Request may contain no options (referred to as an empty or
 * null configuration request) and can be used to request a response. For an
 * empty configuration request the length field is set to 0x0004.
 */

    uint8_t conn_index;
    uint8_t cb_index;

    l2cap_connection_request_type l2cap_connection_request;
    memcpy(&l2cap_connection_request ,command,4);
    memcpy(&(l2cap_connection_request.psm) ,command->data,2);
    memcpy(&(l2cap_connection_request.scid) ,&(command->data[2]),2);

    //Initialize a connection 

    for(conn_index=0; conn_index<MAX_L2CAP_CONNECTION_HANDLES; conn_index++)
    {
        if(l2cap_connections_array[conn_index].scid == l2cap_connection_request.scid)
            break;
        if(l2cap_connections_array[conn_index].scid == 0)
            break;
    }
    if(l2cap_connections_array[conn_index].scid == l2cap_connection_request.scid)
    {
        generic_command.code = L2CAP_COMMAND_REJECT;
        generic_command.id = command->id;
        generic_command.length = 0x02;
        useful_array[0]=0x00;//Command not understood
        useful_array[1]=0x02;//Command not understood
        generic_command.data = useful_array; 
        assemble_l2cap_sig_package(connection, &generic_command,1);
    }
    if(conn_index==MAX_L2CAP_CONNECTION_HANDLES)
    {
        generic_command.code = L2CAP_CONNECTION_RESPONSE;
        generic_command.id = command->id;
        generic_command.length = 0x08;
        useful_array[0]=0x00;//DCID
        useful_array[1]=0x00;//DCID
        useful_array[2]=0x00;//SCID
        useful_array[3]=0x00;//SCID
        useful_array[4]=0x00;//RESULT - Connection refused  no resources available.
        useful_array[5]=0x04;//RESULT - Connection refused  no resources available.
        useful_array[6]=0x00;//No further information available
        useful_array[7]=0x00;//No further information available
        generic_command.data = useful_array; 
        assemble_l2cap_sig_package(connection, &generic_command,1);
    }
    switch(l2cap_connections_array[conn_index].state)
    {
        case CLOSED: 
        case WAIT_CONNECT: 
            // Verify if there is any listener function for this PSM
            for(cb_index=0;cb_index<MAX_L2CAP_CONNECTION_CBS;cb_index++)
            {
                if(l2cap_callbacks_array[cb_index].psm == l2cap_connection_request.psm)
                    break;
            }
            if(l2cap_callbacks_array[cb_index].psm != l2cap_connection_request.psm)
            {
                generic_command.code = L2CAP_CONNECTION_RESPONSE;
                generic_command.id = command->id;
                generic_command.length = 0x08;
                useful_array[0]=0x00;//DCID
                useful_array[1]=0x00;//DCID
                useful_array[2]=0x00;//SCID
                useful_array[3]=0x00;//SCID
                useful_array[4]=0x00;//RESULT - PSM not supported.
                useful_array[5]=0x04;//RESULT - PSM not supported.
                useful_array[6]=0x00;//No further information available
                useful_array[7]=0x00;//No further information available
                generic_command.data = useful_array; 
                assemble_l2cap_sig_package(connection, &generic_command,1);
                break;
            }
            l2cap_connections_array[conn_index].dcid = conn_index+0x0040;
            l2cap_connections_array[conn_index].scid = l2cap_connection_request.scid;

            generic_command.code = L2CAP_CONNECTION_RESPONSE;
            generic_command.id = command->id;
            generic_command.length = 0x08;
            memcpy(useful_array, &(l2cap_connections_array[conn_index].dcid), 2);
            memcpy(&(useful_array[2]), &(l2cap_connections_array[conn_index].scid), 2);
            useful_array[4]=0x00;//RESULT - Connection successful.
            useful_array[5]=0x00;//RESULT - Connection successful.
            useful_array[6]=0x00;//No further information available
            useful_array[7]=0x00;//No further information available
            generic_command.data = useful_array; 
            assemble_l2cap_sig_package(connection, &generic_command,1);
            l2cap_connections_array[conn_index].state=WAIT_CONFIG;
            DBG("setting callback to conn_index [%d]\n",conn_index);
            l2cap_connections_array[conn_index].callback=l2cap_callbacks_array[cb_index].callback;

            break;
        default:
            generic_command.code = L2CAP_COMMAND_REJECT;
            generic_command.id = command->id;
            generic_command.length = 0x02;
            useful_array[0]=0x00;//Command not understood
            useful_array[1]=0x02;//Command not understood
            generic_command.data = useful_array; 
            assemble_l2cap_sig_package(connection, &generic_command,1);
    }
    
    return SUCCESS;
}

err_t handle_connection_response(connection_type *connection, l2cap_command_type *command)
{

/* The data fields are:
 * --> Destination Channel Identifier - DCID (2 octets)
 * This field contains the channel endpoint on the device sending this
 * Response packet. Thus, the Destination CID represents the channel endpoint
 * on the device receiving the request and sending the response.
 * --> Source Channel Identifier - SCID (2 octets)
 * This field contains the channel endpoint on the device receiving this
 * Response packet. This is copied from the SCID field of the connection
 * request packet.
 * --> Result (2 octets)
 * The result field indicates the outcome of the connection request. The result
 * value of 0x0000 indicates success while a non-zero value indicates the connection
 * request failed or is pending. A logical channel is established on the
 * receipt of a successful result. Table 4.4 on page 44 defines values for this
 * field. The DCID and SCID fields shall be ignored when the result field indicates
 * the connection was refused
 * --> Status (2 octets)
 * Only defined for Result = Pending. Indicates the status of the connection.
 * The status is set to one of the values shown in Table 4.5 on page 45.
 */

    uint8_t conn_index;
    uint8_t cb_index;

    l2cap_connection_response_type l2cap_connection_response;
    memcpy(&l2cap_connection_response ,command,4);
    memcpy(&(l2cap_connection_response.dcid) ,command->data,2);
    memcpy(&(l2cap_connection_response.scid) ,&(command->data[2]),2);
    memcpy(&(l2cap_connection_response.result) ,&(command->data[4]),2);
    memcpy(&(l2cap_connection_response.status) ,&(command->data[6]),2);



    // search for this connection
    for(conn_index=0; conn_index<MAX_L2CAP_CONNECTION_HANDLES; conn_index++)
    {
        if(l2cap_connections_array[conn_index].scid == l2cap_connection_response.scid)
            break;
    }

    if(conn_index==MAX_L2CAP_CONNECTION_HANDLES) //Connection not found
    {
            generic_command.code = L2CAP_COMMAND_REJECT;
            generic_command.id = command->id;
            generic_command.length = 0x02;
            useful_array[0]=0x00;//Command not understood
            useful_array[1]=0x00;//Command not understood
            generic_command.data = useful_array; 
            assemble_l2cap_sig_package(connection, &generic_command,1);
    }
    switch(l2cap_connections_array[conn_index].state)
    {
        case CLOSED: 
        case WAIT_CONNECT:
        case WAIT_DISCONNECT:
            break; //Ignore
        case WAIT_CONNECT_RSP:
            switch(l2cap_connection_response.result)
            {

                case 0x0000: //Connection successful.
                    generic_command.code = L2CAP_CONFIGURE_REQUEST;
                    generic_command.id = command->id;
                    generic_command.length = 0x08;
                    generic_command.data = useful_array; 
                    assemble_l2cap_sig_package(connection, &generic_command,1);
                    l2cap_connections_array[conn_index].state = WAIT_CONFIG;
                    break;
                case 0x0001: //Connection pending
                    break; //Ignore
                case 0x0002: //Connection refused  PSM not supported
                case 0x0003: //Connection refused  security block.
                case 0x0004: //Connection refused  no resources available.
                    //Notify upper level (if any)
                    if(l2cap_connections_array[conn_index].callback)
                    {
                        l2cap_connections_array[conn_index].callback(NULL,&(l2cap_connection_response.result));
                    }
                    l2cap_connections_array[conn_index].state = CLOSED;
                    break;
            }
            break;
        default:
            generic_command.code = L2CAP_COMMAND_REJECT;
            generic_command.id = command->id;
            generic_command.length = 0x02;
            useful_array[0]=0x00;//Command not understood
            useful_array[1]=0x00;//Command not understood
            generic_command.data = useful_array; 
            assemble_l2cap_sig_package(connection, &generic_command,1);
    }
    return SUCCESS;
}

err_t handle_configure_response(connection_type *connection, l2cap_command_type *command)
{
    uint8_t conn_index;
    l2cap_configure_response_type l2cap_configure_response;
    uint16_t result;

    /* The data fields are:
     * • Source CID - SCID (2 octets)
     * This field contains the channel endpoint on the device receiving this
     * Response packet. The device receiving the Response shall check that the
     * Identifier field matches the same field in the corresponding configuration
     * request command and the SCID matches its local CID paired with the original
     * DCID.
     * • Flags (2 octets)
     * Figure 4.9 displays the two-octet Flags field. Note the most significant bit is
     * shown on the left.
     * Figure 4.9: Configuration Response Flags field format
     * Only one flag is defined, the Continuation flag (C).
     * More configuration responses will follow when C is set to one. This flag indicates
     * that the parameters included in the response are a partial subset of
     * parameters being sent by the device sending the Response packet.
     * The other flag bits are reserved and shall be set to zero. L2CAP implementations
     * shall ignore these bits.
     * • Result (2 octets)
     * The Result field indicates whether or not the Request was acceptable. See
     * Table 4.6 on page 49 for possible result codes.
     * • Configuration Options
     * This field contains the list of parameters being configured. These are
     * defined in Section 5 on page 55. On a successful result, these parameters
     * contain the return values for any wild card parameter values (see Section
     * 5.3 on page 58) contained in the request.
     */

    memcpy(&l2cap_configure_response ,command,4);
    memcpy(&(l2cap_configure_response.scid),command->data,2);
    memcpy(&(l2cap_configure_response.flags),&(command->data[2]),2);
    memcpy(&(l2cap_configure_response.result),&(command->data[4]),2);
    l2cap_configure_response.config_opt = &(command->data[6]);

    // search for this connection
    for(conn_index=0; conn_index<MAX_L2CAP_CONNECTION_HANDLES; conn_index++)
    {
        if(l2cap_connections_array[conn_index].dcid == l2cap_configure_response.scid)
            break;
    }

    if(conn_index==MAX_L2CAP_CONNECTION_HANDLES) //Connection not found
    {
        generic_command.code = L2CAP_COMMAND_REJECT;
        generic_command.id = command->id;
        generic_command.length = 0x02;
        useful_array[0]=0x00;//Command not understood
        useful_array[1]=0x00;//Command not understood
        generic_command.data = useful_array; 
        assemble_l2cap_sig_package(connection, &generic_command,1);
    }
    switch(l2cap_connections_array[conn_index].state)
    {
        case CLOSED:
        case WAIT_CONNECT_RSP:
        case WAIT_DISCONNECT:
            generic_command.code = L2CAP_COMMAND_REJECT;
            generic_command.id = command->id;
            generic_command.length = 0x02;
            useful_array[0]=0x00;//Command not understood
            useful_array[1]=0x00;//Command not understood
            generic_command.data = useful_array; 
            assemble_l2cap_sig_package(connection, &generic_command,1);
            l2cap_connections_array[conn_index].state = CLOSED;
            break;
        case OPEN:
        case WAIT_CONFIG:
        case WAIT_CONFIG_REQ:
        case WAIT_CONFIG_REQ_RSP:
// As we currently support only basic L2CAP implementation, we verify
// if these are basic options and respond with success if so or fail otherwise.

            if(l2cap_configure_response.result==0)
            {
                assemble_l2cap_sig_package(connection, NULL,0);
                l2cap_connections_array[conn_index].state = OPEN;

            }
            break;
    }
    return SUCCESS;
}

err_t handle_configure_request(connection_type *connection, l2cap_command_type *command)
{
    uint8_t conn_index;
    l2cap_configure_request_type l2cap_configure_request;
    uint16_t result;

/* The data fields are:
 * --> Destination CID - DCID (2 octets)
 * This field contains the channel endpoint on the device receiving this
 * Request packet.
 * --> Flags (2 octets)
 * Figure 4.7 shows the two-octet Flags field. Note the most significant bit is
 * shown on the left
 * Only one flag is defined, the Continuation flag (C).
 * -->Configuration Options
 * A list of the parameters and their values to be negotiated shall be provided in
 * the Configuration Options field. These are defined in Section 5 on page 55.
 * A Configuration Request may contain no options (referred to as an empty or
 * null configuration request) and can be used to request a response. For an
 * empty configuration request the length field is set to 0x0004.
 * 
 * !!!!CURRENTLY WE SUPPORT ONLY BASIC L2CAP IMPLEMENTATIION!!!!
 */

    memcpy(&l2cap_configure_request ,command,4);
    memcpy(&(l2cap_configure_request.dcid),command->data,2);
    memcpy(&(l2cap_configure_request.flags),&(command->data[2]),2);
    l2cap_configure_request.config_opt = &(command->data[6]);

    // search for this connection
    for(conn_index=0; conn_index<MAX_L2CAP_CONNECTION_HANDLES; conn_index++)
    {
        if(l2cap_connections_array[conn_index].dcid == l2cap_configure_request.dcid)
            break;
    }

    if(conn_index==MAX_L2CAP_CONNECTION_HANDLES) //Connection not found
    {
        generic_command.code = L2CAP_COMMAND_REJECT;
        generic_command.id = command->id;
        generic_command.length = 0x02;
        useful_array[0]=0x00;//Command not understood
        useful_array[1]=0x00;//Command not understood
        generic_command.data = useful_array; 
        assemble_l2cap_sig_package(connection, &generic_command,1);
    }
    switch(l2cap_connections_array[conn_index].state)
    {
        case CLOSED:
        case WAIT_CONNECT_RSP:
        case WAIT_DISCONNECT:
            generic_command.code = L2CAP_COMMAND_REJECT;
            generic_command.id = command->id;
            generic_command.length = 0x02;
            useful_array[0]=0x00;//Command not understood
            useful_array[1]=0x00;//Command not understood
            generic_command.data = useful_array; 
            assemble_l2cap_sig_package(connection, &generic_command,1);
            l2cap_connections_array[conn_index].state = CLOSED;
            break;
        case OPEN:
        case WAIT_CONFIG:
        case WAIT_CONFIG_REQ:
        case WAIT_CONFIG_REQ_RSP:
// As we currently support only basic L2CAP implementation, we verify
// if these are basic options and respond with success if so or fail otherwise.

            if((result = verify_l2cap_config_options(&l2cap_configure_request)) != 0x0000) // not acceptable
            {
                generic_command.code = L2CAP_CONFIGURE_RESPONSE; 
                generic_command.id = command->id;
                generic_command.length = 0x0a;
                memcpy(basic_l2cap_config,&(l2cap_connections_array[conn_index].scid),sizeof(uint16_t));
                generic_command.data = basic_l2cap_config; 
                assemble_l2cap_sig_package(connection, &generic_command,1);

                generic_command.code = L2CAP_CONFIGURE_REQUEST; 
                generic_command.id = command->id;
                generic_command.length = 0x04;
                memcpy(basic_l2cap_config,&(l2cap_connections_array[conn_index].scid),sizeof(uint16_t));

                generic_command.data = basic_l2cap_config; 
                assemble_l2cap_sig_package(connection, &generic_command,2);
                l2cap_connections_array[conn_index].state = WAIT_CONFIG_REQ_RSP;


            }
            break;
    }
    return SUCCESS;
}


err_t assemble_l2cap_sig_package(connection_type *connection, l2cap_command_type *command,uint8_t n_packet)
{

    connection->n_packets_out=n_packet;
    if( ! n_packet )
        return SUCCESS;
    n_packet--;
    connection->l2cap_packet_out[n_packet].length = 0;
    connection->l2cap_packet_out[n_packet].cid = connection->l2cap_packet_in->cid;
    if((connection->l2cap_packet_out[n_packet].payload = realloc(connection->l2cap_packet_out[n_packet].payload,
                                                        connection->l2cap_packet_out[n_packet].length + command->length + 4))== NULL)
    {
        return MEMORY_ERROR;
    }
    memcpy(&(((uint8_t *)(connection->l2cap_packet_out[n_packet].payload))[connection->l2cap_packet_out[n_packet].length]),command,4);
    memcpy(&(((uint8_t *)(connection->l2cap_packet_out[n_packet].payload))[connection->l2cap_packet_out[n_packet].length+4]),command->data, command->length);
    connection->l2cap_packet_out[n_packet].length = command->length+4;
    return SUCCESS;
}


uint16_t verify_l2cap_config_options(l2cap_configure_request_type *l2cap_configure_request)
{
    return 0x1;
}


err_t handle_data_package(connection_type *connection)
{

    uint8_t *ptr;
    uint8_t conn_index;
    ptr= connection->l2cap_packet_in->payload;
    DBG("connection->l2cap_packet_in->length = [0x%02x]\n", connection->l2cap_packet_in->length );
    DBG("connection->l2cap_packet_in->cid = [0x%02x]\n", connection->l2cap_packet_in->cid );
    conn_index=connection->l2cap_packet_in->cid - 0x0040;

    DBG("Chamando callback conn_index=[%d]\n",conn_index);
    l2cap_connections_array[conn_index].callback(connection->l2cap_packet_in,NULL);
}

