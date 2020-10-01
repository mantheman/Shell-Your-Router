#define _POSIX_C_SOURCE (199506L) // For flockfile and funlockfile functions.
#include <stdio.h>
#include <string.h>

#include "return_codes.h"
#include "common.h"
#include "logger.h"

#define LOG_LEVEL_MAX_SIZE (8)
#define FINAL_LOG_MESSAGE_MAX_SIZE (MAX_LOG_MESSAGE_SIZE * 2)

static char log_levels_g[][LOG_LEVEL_MAX_SIZE] = {"DEBUG", "INFO", "WARNING", "CRITICAL", "ERROR"};


return_code_t logger__init_logger(char *log_path, logger__log_t *new_logger)
{
    return_code_t result = RC_UNINITIALIZED;
    FILE *new_log_file = NULL;

    if (NULL == log_path || NULL == new_logger){
        handle_error(RC_LOGGER__INIT_LOGGER__BAD_PARAMS);
    }

    new_log_file = fopen(log_path, "w");
    if (NULL == new_log_file){
        handle_perror("Fopen failed", RC_LOGGER__INIT_LOGGER__FOPEN_FAILED);
    }

    new_logger->log_file = new_log_file;
    result = RC_SUCCESS;
l_cleanup:
    return result;
}

return_code_t logger__destory_logger(logger__log_t *logger)
{
    return_code_t result = RC_UNINITIALIZED;

    if (NULL == logger){
        handle_error(RC_LOGGER__DESTROY_LOGGER__BAD_PARAMS);
    }

    if (EOF == fclose(logger->log_file)){
        handle_perror("Fclose failed", RC_LOGGER__DESTROY_LOGGER__FCLOSE_FAILED);
    }

    result = RC_SUCCESS;
l_cleanup:
    return result;
}

return_code_t logger__log(logger__log_t *logger, log_level_t log_level, char *message)
{
    return_code_t result = RC_UNINITIALIZED;
    char log_message[FINAL_LOG_MESSAGE_MAX_SIZE] = {0};
    char *log_level_string = NULL;

    if (NULL == logger || NULL == message){
        handle_error(RC_LOGGER__LOG__BAD_PARAMS);
    }

    log_level_string = log_levels_g[(int)log_level];
    snprintf(log_message, FINAL_LOG_MESSAGE_MAX_SIZE, "%s: %s\n", log_level_string, message);

    flockfile(logger->log_file);

    fwrite(log_message, strlen(log_message), 1, logger->log_file);
    if (EOF == fflush(logger->log_file)){
        handle_perror("Fflush failed", RC_LOGGER__LOG__FFLUSH_FAILED);
    }

    funlockfile(logger->log_file);

    result = RC_SUCCESS;
l_cleanup:
    return result;
}
