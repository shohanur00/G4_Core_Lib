#ifndef LOG_UART_H
#define LOG_UART_H

#include <stddef.h>
#include <board.h>

#ifdef __cplusplus
extern "C" {
#endif



void LOG_UART_Init(void);
void LOG_UART_Write(const char *buffer, size_t length);

#ifdef __cplusplus
}
#endif

#endif /* LOG_UART_H */