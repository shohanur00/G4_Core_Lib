#ifndef LOGGER_H
#define LOGGER_H

#include <stdint.h>
#include <stdarg.h>


/* Logging Macros */
#define LOG_DEBUG(module, ...) \
    LOG_Write(module, LOG_LEVEL_DEBUG, __VA_ARGS__)

#define LOG_INFO(module, ...) \
    LOG_Write(module, LOG_LEVEL_INFO, __VA_ARGS__)

#define LOG_WARNING(module, ...) \
    LOG_Write(module, LOG_LEVEL_WARNING, __VA_ARGS__)

#define LOG_ERROR(module, ...) \
    LOG_Write(module, LOG_LEVEL_ERROR, __VA_ARGS__)

#define LOG_CRITICAL(module, ...) \
    LOG_Write(module, LOG_LEVEL_CRITICAL, __VA_ARGS__)


/* Logging Modules */
typedef enum
{
    LOG_MODULE_SYSTEM = 0U,

    LOG_MODULE_MAX

} LOG_Module_t;


/* Logging Levels */
typedef enum
{
    LOG_LEVEL_DEBUG = 0U,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_CRITICAL

} LOG_Level_t;


typedef struct
{
    LOG_Module_t module;
    LOG_Level_t level;
    const char *message;

} LOG_Message_t;

/* Logger API */
void LOG_Init(void);

void LOG_SetModuleLevel(
    LOG_Module_t module,
    LOG_Level_t level
);

void LOG_Write(
    LOG_Module_t module,
    LOG_Level_t level,
    const char *format,
    ...
);


#endif /* LOGGER_H */