#ifndef LOGGER_HAL_H
#define LOGGER_HAL_H

#include <stdint.h>
#include <stddef.h>

void LOG_HAL_Init(void);
void LOG_HAL_Write(const char *buffer, size_t length);


#endif /* LOGGER_HAL_H */