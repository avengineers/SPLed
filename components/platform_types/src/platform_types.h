/**
 * @file platform_types.h
 * @brief Platform-specific type definitions.
 */

#ifndef PLATFORM_TYPES_H
#define PLATFORM_TYPES_H

#include <stdint.h>
#include <stdbool.h>

/** @brief Boolean type definition. */
typedef bool bool_t;

/** @brief Macro for boolean TRUE. */
#define TRUE 1

/** @brief Macro for boolean FALSE. */
#define FALSE 0

#ifndef NULL
#ifndef __cplusplus
#define NULL (void *)0
#else
#define NULL 0
#endif
#endif

#endif /* PLATFORM_TYPES_H */
