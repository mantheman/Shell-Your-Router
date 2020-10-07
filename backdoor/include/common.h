/**
 * @file common.h
 * @author Yuval Krinsky (ykrinksy@gmail.com)
 * @brief Common macro utilities and defines.
 * @date 2020-09-17
 */
 #pragma once

#define handle_error(rc) do{ result=rc; goto l_cleanup; } while (0)
#define handle_perror(msg, rc) do{ perror(msg); result=rc; goto l_cleanup; } while (0)

#define arr_length(arr) sizeof(arr) / sizeof(arr[0])