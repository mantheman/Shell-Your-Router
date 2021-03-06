/**
 * @file shell.h
 * @author Yuval Krinsky (ykrinksy@gmail.com)
 * @brief Manages tcp server that opens shells for incoming connections.
 * @date 2020-09-17
 */
#pragma once

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <fcntl.h>
#include <string.h>

#include "return_codes.h"
#include "logger.h"

#ifdef DEBUG
#define SHELL_PATH ("/bin/sh")
#else
#define SHELL_PATH ("/bin/ash")
#endif

#define WELCOME_BANNER ("Welcome to a secret shell...\n")
#define SHELL_MAX_LISTENING_QUEUE (5)

/**
 * @brief Initializes TCP server which listens on all interfaces on the given port.
 * 
 * @param server_port [in] New server will listen to incoming connections on this port.
 * @param server_socket_ptr [out] Will point to the socket of the new server.
 * @return return_code_t 
 */
return_code_t shell__init_server(int32_t server_port, int32_t *server_socket_ptr);

/**
 * @brief Frees the resources of the shell server.
 * 
 * @param server_socket [in] Pointer to server's socket (should be acquired by shell__init_server).
 * @return return_code_t 
 */
return_code_t shell__destroy_server(int32_t *server_socket_ptr);

/**
 * @brief Waits for a new connection, and then opens a shell for it.
 * 
 * @param server_socket [in] Shell server's socket.
 * @return return_code_t 
 * @note This function blocks until a new connection is received.
 */
return_code_t shell__handle_new_connection(int32_t server_socket);
