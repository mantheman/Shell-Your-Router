/**
 * @file tftp.h
 * @author Yuval Krinsky (ykrinksy@gmail.com)
 * @brief TFTP server that provides uploading and downloading files mechanism.
 * @date 2020-09-28
 */
#pragma once

#include <stdint.h>

#include "return_codes.h"
#include "logger.h"

#define MAX_FILEPATH_SIZE (256)
#define TFTP_REQUESTS_PORT (69)

/**
 * @brief Initializes TFTP server which listens on all interfaces on the given port.
 * 
 * @param server_port [in] New server will listen to incoming connections on this port.
 * @param logger [in] Program's logger.
 * @param server_socket_ptr [out] Will point to the socket of the new server.
 * @return return_code_t 
 */
return_code_t tftp__init_server(int32_t server_port, logger__log_t *logger, int32_t *server_socket_ptr);

/**
 * @brief Frees the resources of the TFTP server.
 * 
 * @param server_socket [in] Pointer to server's socket (should be acquired by tftp__init_server).
 * @param logger [in] Program's logger.
 * @return return_code_t 
 */
return_code_t tftp__destroy_server(int32_t *server_socket_ptr, logger__log_t *logger);

/**
 * @brief Waits for new connections and handles them.
 * 
 * @param server_socket [in] TFTP server's socket.
 * @param logger [in] Program's logger.
 * @return return_code_t 
 * @note This function shouldn't return, unless an error has happened.
 */
return_code_t tftp__run_server(int32_t server_socket, logger__log_t *logger);
