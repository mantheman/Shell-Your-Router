#define _GNU_SOURCE

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <string.h>

#define handle_error(msg, rc) do{ perror(msg); result=rc; goto l_cleanup; } while (0)
#define arr_length(arr) sizeof(arr) / sizeof(arr[0])

#define DEST_PORT (10022)
#define MESSAGE ("I'm here! Don't worry :)")
#define MAX_MESSAGE_LENGTH (100)
#define DEST_IP_ARGV_INDEX (1)
#define SLEEP_SECONDS_BETWEEN_SENDS (10)

#define INET_ATON_FAILED (0)

typedef enum return_code_e{
    RC_UNINITIAILIZED = -1,
    RC_SUCCESS = 0,
    RC_PROGRAM_BAD_PARAMS,
    RC_SOCKET_INIT_FAILED,
    RC_SOCKET_SENDTO_FAILED,
    RC_INET_ATON_FAILED,
} return_code_t;

int main(int argc, char *argv[])
{
    return_code_t result = RC_UNINITIAILIZED;
    int32_t client_socket = -1;
    uint16_t dest_port = DEST_PORT;
    char message[] = MESSAGE;
    size_t message_size = 0;
    ssize_t bytes_sent = 0;

    struct sockaddr_in dest_socket_address = {0};
    struct in_addr dest_ip_address = {0};

    if (argc <= DEST_IP_ARGV_INDEX){
        result = RC_PROGRAM_BAD_PARAMS;
        goto l_cleanup;
    }

    client_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (-1 == client_socket){
        handle_error("Socket failed", RC_SOCKET_INIT_FAILED);
    }

    if (INET_ATON_FAILED == inet_aton(argv[DEST_IP_ARGV_INDEX], &dest_ip_address)){
        handle_error("inet_aton failed", RC_INET_ATON_FAILED);
    }

    dest_socket_address.sin_family = AF_INET;
    dest_socket_address.sin_port = htons(dest_port);
    dest_socket_address.sin_addr = dest_ip_address;

    printf("Initialized client socket successfully.\n");

    message_size = strnlen(message, MAX_MESSAGE_LENGTH);

    while(true){
        printf("Sending message: %s\n", message);
        bytes_sent = sendto(
            client_socket,
            message,
            message_size,
            0, 
            (struct sockaddr *) &dest_socket_address,
            (socklen_t) sizeof(dest_socket_address)
        );
        if (-1 == bytes_sent){
            handle_error("sendto failed", RC_SOCKET_SENDTO_FAILED);
        }

        sleep(SLEEP_SECONDS_BETWEEN_SENDS);
    }

    result = RC_SUCCESS;


l_cleanup:
    if (-1 != client_socket){
        (void)close(client_socket);
    }
    
    return (int)result;
}