/**
 * @file tftp.h
 * @author Yuval Krinsky (ykrinksy@gmail.com)
 * @brief TFTP server that provides uploading and downloading files mechanism.
 * @date 2020-09-28
 * @note This server only supports one connection at a time.
 */
#pragma once

#include <stdint.h>

#include "return_codes.h"
#include "logger.h"

#define MAX_FILEPATH_SIZE (256)
#define TFTP_REQUESTS_PORT (69)
#define TFTP_MAX_LISTENING_QUEUE (5)

/**
 * @brief Initializes TFTP server which listens on all interfaces on the given port.
 * 
 * @param server_port [in] New server will listen to incoming connections on this port.
 * @param server_socket_ptr [out] Will point to the socket of the new server.
 * @return return_code_t 
 */
return_code_t tftp__init_server(int32_t server_port, int32_t *server_socket_ptr);

/**
 * @brief Frees the resources of the TFTP server.
 * 
 * @param server_socket [in] Pointer to server's socket (should be acquired by tftp__init_server).
 * @return return_code_t 
 */
return_code_t tftp__destroy_server(int32_t *server_socket_ptr);

/**
 * @brief Waits for new connections and handles them.
 * 
 * @param server_socket [in] TFTP server's socket.
 * @return return_code_t 
 * @note This function shouldn't return, unless an error has happened.
 */
return_code_t tftp__run_server(int32_t server_socket);
