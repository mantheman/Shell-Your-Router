
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


return_code_t tftp__init_server(int32_t server_port, logger__log_t *logger, int32_t *server_socket_ptr)
{
    return_code_t result = RC_UNINITIALIZED;


    result = RC_SUCCESS;
l_cleanup:
    return result;
}

return_code_t tftp__destroy_server(int32_t *server_socket_ptr, logger__log_t *logger)
{
    return_code_t result = RC_UNINITIALIZED;

    result = RC_SUCCESS;
l_cleanup:
    return result;
}

return_code_t tftp__run_server(int32_t server_socket, logger__log_t *logger)
{
    return_code_t result = RC_UNINITIALIZED;

    result = RC_SUCCESS;
l_cleanup:
    return result;
}