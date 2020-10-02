#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/udp.h>
#include <assert.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "common.h"
#include "tftp.h"

#define BLOCK_SIZE (512)

#define MAX_DATAGRAM_SIZE (516) // As defined in Tftp RFC(RFC 1350).

typedef enum opcodes_e{
    RRQ = 1, // Read request.
    WRQ, // Write request.
    DATA,
    ACK,
    ERROR
} opcodes_t;

typedef enum tftp_error_codes_e{
    UNDEFINED = 0,
    FILE_NOT_FOUND,
    ACCESS_VIOLATION,
    ALLOCATION_FAILED,
    ILLEGAL_TFTP_OPERATION,
    UKNOWN_TRANSFER_ID,
    FILE_ALREADY_EXISTS,
    NO_SUCH_USER,
} tftp_error_codes_t;

typedef struct request_packet_s{
    uint16_t opcode; // Only RRQ or WRQ.
    char filename_and_mode[MAX_DATAGRAM_SIZE - 2];
} request_packet_t;

typedef struct data_packet_s{
    uint16_t opcode; // Only DATA.
    uint16_t block_number;
    int8_t data[BLOCK_SIZE];
} data_packet_t;

typedef struct ack_packet_s{
    uint16_t opcode; // Only ACK.
    uint16_t block_number;
} ack_packet_t;

typedef struct error_packet_s{
    uint16_t opcode; // Only ERROR.
    uint16_t error_code;
    char error_message[MAX_DATAGRAM_SIZE - 4];
} error_packet_t;

typedef union tftp_packet_u{
    request_packet_t request;
    data_packet_t data;
    ack_packet_t ack;
    error_packet_t error;
} tftp_packet_t;

typedef struct tftp_connection_s{
    int32_t my_tid; // TID - Transfer indefitier, in TFTP it's the UDP port.
    int32_t other_tid;
    uint32_t last_ack;
    tftp_packet_t *last_packet;
} tftp_connection_t;

static return_code_t init_udp_server_socket(int32_t server_port, int32_t *server_socket_ptr)
{
    return_code_t result = RC_UNINITIALIZED;
    int32_t server_socket = -1;
    struct sockaddr_in server_address = {0};
    struct in_addr server_inteface = {0};
    int bind_result = -1;

    assert(NULL != server_socket_ptr);

    server_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (-1 == server_socket){
        handle_perror("Socket failed", RC_TFTP__INIT_UDP_SERVER__SOCKET_INIT_FAILED);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(server_port);
    server_inteface.s_addr = INADDR_ANY;
    server_address.sin_addr = server_inteface;
    bind_result = bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address));
    if (-1 == bind_result){
        handle_perror("Bind failed", RC_TFTP__INIT_UDP_SERVER__SOCKET_BIND_FAILED);
    }

    *server_socket_ptr = server_socket;

    result = RC_SUCCESS;
l_cleanup:
    return result;

}

return_code_t tftp__init_server(int32_t server_port, logger__log_t *logger, int32_t *server_socket_ptr)
{
    return_code_t result = RC_UNINITIALIZED;
    char log_message[MAX_LOG_MESSAGE_SIZE] = {0};

    if (NULL == server_socket_ptr || NULL == logger || 0 == server_port){
        handle_error(RC_TFTP__INIT_SERVER__BAD_PARAMS);
    }

    result = init_udp_server_socket(server_port, server_socket_ptr);
    if (result != RC_SUCCESS){
        goto l_cleanup;
    }

    snprintf(log_message, MAX_LOG_MESSAGE_SIZE, "TFTP server started on port: %d", server_port);
    logger__log(logger, LOG_INFO, log_message);
    result = RC_SUCCESS;
l_cleanup:
    return result;
}

return_code_t tftp__destroy_server(int32_t *server_socket_ptr, logger__log_t *logger)
{
    return_code_t result = RC_UNINITIALIZED;
    if (NULL == server_socket_ptr || NULL == logger){
        handle_error(RC_TFTP__DESTROY_SERVER__BAD_PARAMS);
    }

    if (-1 == close(*server_socket_ptr)){
        handle_perror("Close failed", RC_TFTP__DESTROY_SERVER__CLOSE_SOCKET_FAILED);
    }

    *server_socket_ptr = -1;

    logger__log(logger, LOG_INFO, "Closed tftp server successfully.");

    result = RC_SUCCESS;
l_cleanup:
    return result;
}

return_code_t tftp__run_server(int32_t server_socket, logger__log_t *logger)
{
    return_code_t result = RC_UNINITIALIZED;
    request_packet_t request_packet = {0};
    ssize_t bytes_read = -1;
    struct sockaddr_in client_address = {0};
    socklen_t client_address_length = sizeof(client_address);
    char log_message[MAX_LOG_MESSAGE_SIZE] = {0};

    if (NULL == logger){
        handle_error(RC_TFTP__RUN_SERVER__BAD_PARAMS);
    }

    bytes_read = recvfrom(
        server_socket,
        &request_packet,
        sizeof(request_packet),
        0,
        (struct sockaddr *)&client_address,
        &client_address_length
    );
    if (-1 == bytes_read){
        handle_perror("Recv failed", RC_TFTP__RUN_SERVER__SOCKET_RECV_FAILED);
    }

    snprintf(
        log_message,
        MAX_LOG_MESSAGE_SIZE,
        "Received connection from: IP: %s, port: %d.",
        inet_ntoa(client_address.sin_addr),
        ntohs(client_address.sin_port)
    );
    logger__log(logger, LOG_DEBUG, log_message);
    
    if((RRQ != request_packet.opcode) || (WRQ != request_packet.opcode)){
        goto l_cleanup;
    }


    result = RC_SUCCESS;
l_cleanup:
    return result;
}