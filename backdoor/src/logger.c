#define _POSIX_C_SOURCE (199506L) // For flockfile and funlockfile functions.
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "return_codes.h"
#include "common.h"
#include "logger.h"

#define LOG_LEVEL_MAX_SIZE (8)
#define FINAL_LOG_MESSAGE_MAX_SIZE (MAX_LOG_MESSAGE_SIZE * 2)

static char log_levels_g[][LOG_LEVEL_MAX_SIZE] = {"DEBUG", "INFO", "WARNING", "CRITICAL", "ERROR"};

typedef struct logger__log_s {
    FILE * log_file;
    log_level_t min_log_level;
} logger__log_t;

// Global logger of the program.
static logger__log_t *g_logger = NULL;

return_code_t logger__init_logger(const char *log_path, log_level_t min_log_level)
{
    return_code_t result = RC_UNINITIALIZED;
    FILE *new_log_file = NULL;

    if (NULL != g_logger){
        handle_error(RC_LOGGER__INIT_LOGGER__LOGGER_ALREADY_INITIALIZED);
    }

    if (NULL == log_path){
        handle_error(RC_LOGGER__INIT_LOGGER__BAD_PARAMS);
    }

    new_log_file = fopen(log_path, "w");
    if (NULL == new_log_file){
        handle_perror("Fopen failed", RC_LOGGER__INIT_LOGGER__FOPEN_FAILED);
    }

    g_logger = malloc(sizeof(*g_logger));
    if (NULL == g_logger){
        handle_perror("Malloc failed", RC_LOGGER__INIT_LOGGER__MALLOC_FAILED);
    }

    g_logger->log_file = new_log_file;
    g_logger->min_log_level = min_log_level;

    result = RC_SUCCESS;
l_cleanup:
    return result;
}

return_code_t logger__destory_logger()
{
    return_code_t result = RC_UNINITIALIZED;

    if (NULL == g_logger){
        handle_error(RC_LOGGER__DESTROY_LOGGER__LOGGER_NOT_INITIALIZED);
    }

    if (EOF == fclose(g_logger->log_file)){
        handle_perror("Fclose failed", RC_LOGGER__DESTROY_LOGGER__FCLOSE_FAILED);
    }

    free(g_logger);
    g_logger = NULL;

    result = RC_SUCCESS;
l_cleanup:
    return result;
}

return_code_t logger__log(log_level_t log_level, char *message)
{
    return_code_t result = RC_UNINITIALIZED;
    char log_message[FINAL_LOG_MESSAGE_MAX_SIZE] = {0};
    char *log_level_string = NULL;

    if (NULL == g_logger){
        handle_error(RC_LOGGER__DESTROY_LOGGER__LOGGER_NOT_INITIALIZED);
    }

    if (NULL == message){
        handle_error(RC_LOGGER__LOG__BAD_PARAMS);
    }

    if (g_logger->min_log_level > log_level){
        result = RC_SUCCESS;
        goto l_cleanup;
    }

    log_level_string = log_levels_g[(int)log_level];
    snprintf(log_message, FINAL_LOG_MESSAGE_MAX_SIZE, "%s: %s\n", log_level_string, message);

    flockfile(g_logger->log_file);

    fwrite(log_message, strlen(log_message), 1, g_logger->log_file);
    if (EOF == fflush(g_logger->log_file)){
        handle_perror("Fflush failed", RC_LOGGER__LOG__FFLUSH_FAILED);
    }

    funlockfile(g_logger->log_file);

    result = RC_SUCCESS;
l_cleanup:
    return result;
}
