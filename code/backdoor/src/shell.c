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

#include "shell.h"
#include "common.h"


#define LISTENING_PORT (1337)

#define FORK_FAILED (-1)
#define FORK_CHILD_PROCESS (0)


return_code_t shell__init_server(int32_t server_port, logger__log_t *logger, int32_t *server_socket_ptr)
{
    return_code_t result = RC_UNINITIALIZED;
    int32_t server_socket = -1;
    struct sockaddr_in server_address = {0};
    struct in_addr server_inteface = {0};
    int temp_result = -1;
    char log_message[MAX_LOG_MESSAGE_SIZE] = {0};

    if (NULL == server_socket_ptr || NULL == logger || 0 == server_port){
        handle_error(RC_SHELL__INIT_SERVER__BAD_PARAMS);
    }

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == server_socket){
        handle_perror("Socket failed", RC_SHELL__INIT_SERVER__SOCKET_INIT_FAILED);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(server_port);
    server_inteface.s_addr = INADDR_ANY;
    server_address.sin_addr = server_inteface;
    temp_result = bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address));
    if (-1 == temp_result){
        handle_perror("Bind failed", RC_SHELL__INIT_SERVER__SOCKET_BIND_FAILED);
    }

    temp_result = listen(server_socket, SHELL_MAX_LISTENING_QUEUE);
    if (-1 == temp_result){
        handle_perror("Listen failed", RC_SHELL__INIT_SOCKET__SOCKET_LISTEN_FAILED);
    }

    // We don't want to pass the shell_server's fd to childrens shell processes.
    temp_result = fcntl(server_socket, F_SETFD, FD_CLOEXEC);
    if (-1 == temp_result){
        handle_perror("Fcntl failed", RC_SHELL__INIT_SOCKET__FCNTL_FAILED);
    }

    *server_socket_ptr = server_socket;

    snprintf(log_message, MAX_LOG_MESSAGE_SIZE, "Shell server started on port: %d", server_port);
    logger__log(logger, LOG_INFO, log_message);
    result = RC_SUCCESS;
l_cleanup:
    return result;
}

return_code_t shell__destroy_server(int32_t *server_socket_ptr, logger__log_t *logger)
{
    return_code_t result = RC_UNINITIALIZED;
    if (NULL == server_socket_ptr || NULL == logger){
        handle_error(RC_SHELL__DESTROY_SERVER__BAD_PARAMS);
    }

    if (-1 == close(*server_socket_ptr)){
        handle_perror("Close failed", RC_SHELL__DESTROY_SERVER__CLOSE_SOCKET_FAILED);
    }

    *server_socket_ptr = -1;

    logger__log(logger, LOG_INFO, "Closed shell server successfully.");
    result = RC_SUCCESS;
l_cleanup:
    return result;
}

/**
 * @brief Starts new shell and redirect shell's stdin, stdout and stderr to given client_socket.
 * 
 * @param client_socket [in] The new shell will use this socket for all it's input and output.
 * @return return_code_t 
 * @note This function shouldn't return in case of success.
 */
static return_code_t start_shell(int32_t client_socket)
{
    return_code_t result = RC_UNINITIALIZED;
    int dup_result = -1;
    int32_t shell_fds[] = {STDIN_FILENO, STDOUT_FILENO, STDERR_FILENO};

    // TODO: Add closure of logger so it won't pass to shell process.

    // Set the shell's standards streams to use the client socket file descriptor.
    for (uint8_t i = 0; i < arr_length(shell_fds); ++i){
        dup_result = dup2(client_socket, shell_fds[i]);
        if (-1 == dup_result){
            handle_perror("Dup2 failed", RC_SHELL__START_SHELL__DUP2_FAILED);
        }
    }

    execl(SHELL_PATH, SHELL_PATH, (char *)NULL);
    // exec shouldn't return unless an error has occurred.
    handle_perror("Execl failed", RC_SHELL__START_SHELL__EXEC_FAILED);

l_cleanup:
    return result;
}

return_code_t shell__handle_new_connection(int32_t server_socket, logger__log_t *logger)
{
    return_code_t result = RC_UNINITIALIZED;
    struct sockaddr_in client_address = {0};
    socklen_t client_address_size = 0;
    int32_t client_socket = -1;
    pid_t fork_result = -1;
    char log_message[MAX_LOG_MESSAGE_SIZE] = {0};

    if (NULL == logger){
        handle_error(RC_SHELL__HANDLE_CONNECTION__BAD_PARAMS);
    }

    client_address_size = sizeof(client_address);
    client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_address_size);
    if (-1 == client_socket){
        handle_perror("Accept failed", RC_SHELL__HANDLE_CONNECTION__SOCKET_ACCEPT_FAILED);
    }

    snprintf(
        log_message,
        MAX_LOG_MESSAGE_SIZE,
        "New shell connection: IP: %s, port: %d.",
        inet_ntoa(client_address.sin_addr),
        ntohs(client_address.sin_port)
    );
    logger__log(logger, LOG_INFO, log_message);

    send(client_socket, WELCOME_BANNER, strlen(WELCOME_BANNER), 0);

    fork_result = fork();
    switch (fork_result)
    {
        case FORK_FAILED:
            result = RC_SHELL__HANDLE_CONNECTION__FORK_FAILED;
            goto l_cleanup;

        // Child process.
        case FORK_CHILD_PROCESS:
            result = start_shell(client_socket);
            break;
    
        // Parent process.
        default:
            break;
    };

    result = RC_SUCCESS;
l_cleanup:
    if (-1 != client_socket){
        (void)close(client_socket);
        client_socket = -1;
    }
    return result;
}
