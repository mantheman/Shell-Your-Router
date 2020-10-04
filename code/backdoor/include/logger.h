/**
 * @file logger.h
 * @author Yuval Krinsky (ykrinksy@gmail.com)
 * @brief Manages the program's logging.
 * @date 2020-09-17
 */
#pragma once
#include <stdio.h>

#include "return_codes.h"

#define MAX_LOG_MESSAGE_SIZE (200)

typedef enum log_level_e{
    LOG_DEBUG = 0,
    LOG_INFO,
    LOG_WARNING,
    LOG_CRITICAL,
    LOG_ERROR,
} log_level_t;

/**
 * @brief Initialises the program's logger.
 * 
 * @param log_path [in] File path of the new logger.
 * @return return_code_t 
 */
return_code_t logger__init_logger(const char *log_path);

/**
 * @brief Frees the program's logger resources.
 * 
 * @return return_code_t 
 */
return_code_t logger__destory_logger();

/**
 * @brief Log new message to the given log.
 * 
 * @param log_level [in] Log level of given message.
 * @param message [in] Message to be printed to the log.
 * @return return_code_t 
 * @note Message should be no longer than MAX_LOG_MESSAGE_SIZE bytes.
 */
return_code_t logger__log(log_level_t log_level, char *message);