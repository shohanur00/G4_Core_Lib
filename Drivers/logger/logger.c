#include "logger.h"



#define LOG_BUFFER_SIZE    128U


static void LOG_AppendChar(
    char *buffer,
    size_t *position,
    size_t size,
    char ch
)
{
    if (*position < (size - 1U))
    {
        buffer[*position] = ch;
        (*position)++;
        buffer[*position] = '\0';
    }
}


static void LOG_AppendString(
    char *buffer,
    size_t *position,
    size_t size,
    const char *string
)
{
    while ((*string != '\0') &&
           (*position < (size - 1U)))
    {
        buffer[*position] = *string;
        (*position)++;
        string++;
    }

    buffer[*position] = '\0';
}


static void LOG_AppendUnsigned(
    char *buffer,
    size_t *position,
    size_t size,
    unsigned int value
)
{
    char temp[sizeof(unsigned int) * 3U + 1U];
    int index = 0;

    if (value == 0U)
    {
        LOG_AppendChar(buffer, position, size, '0');
        return;
    }

    while (value > 0U)
    {
        temp[index++] = (char)('0' + (value % 10U));
        value /= 10U;
    }

    while (index > 0)
    {
        LOG_AppendChar(
            buffer,
            position,
            size,
            temp[--index]
        );
    }
}


static void LOG_AppendInteger(
    char *buffer,
    size_t *position,
    size_t size,
    int value
)
{
    if (value < 0)
    {
        LOG_AppendChar(buffer, position, size, '-');

        /*
         * Avoid -INT_MIN overflow.
         * Convert using unsigned arithmetic.
         */
        unsigned int magnitude =
            0U - (unsigned int)value;

        LOG_AppendUnsigned(
            buffer,
            position,
            size,
            magnitude
        );
    }
    else
    {
        LOG_AppendUnsigned(
            buffer,
            position,
            size,
            (unsigned int)value
        );
    }
}


static void LOG_AppendHex(
    char *buffer,
    size_t *position,
    size_t size,
    unsigned int value
)
{
    static const char hex[] = "0123456789ABCDEF";

    char temp[sizeof(unsigned int) * 2U];
    int index = 0;

    if (value == 0U)
    {
        LOG_AppendChar(buffer, position, size, '0');
        return;
    }

    while (value > 0U)
    {
        temp[index++] = hex[value & 0x0FU];
        value >>= 4U;
    }

    while (index > 0)
    {
        LOG_AppendChar(
            buffer,
            position,
            size,
            temp[--index]
        );
    }
}


static void LOG_AppendBinary(
    char *buffer,
    size_t *position,
    size_t size,
    unsigned int value
)
{
    int started = 0;

    for (int i = (sizeof(unsigned int) * 8U) - 1;
     i >= 0;
     i--)
    {
        if ((value & (1U << i)) != 0U)
        {
            started = 1;
        }

        if (started)
        {
            LOG_AppendChar(
                buffer,
                position,
                size,
                (value & (1U << i)) ? '1' : '0'
            );
        }
    }

    if (!started)
    {
        LOG_AppendChar(
            buffer,
            position,
            size,
            '0'
        );
    }
}


void LOG_Init(void){
    // Initialize the logger hardware abstraction layer (HAL)
    LOG_HAL_Init();
}

void LOG_SetModuleLevel(LOG_Module_t module, LOG_Level_t level){
    // Set the logging level for the specified module
    LOG_HAL_SetModuleLevel(module, level);
}

void LOG_Write(LOG_Module_t module, LOG_Level_t level, const char *format, ...){
    // Implementation for writing log messages
    char buffer[LOG_BUFFER_SIZE];
    size_t position = 0U;

    va_list args;

    buffer[0] = '\0';

    va_start(args, format);

    while (*format != '\0')
    {
        if (*format != '%')
        {
            LOG_AppendChar(
                buffer,
                &position,
                sizeof(buffer),
                *format
            );

            format++;
            continue;
        }

        /*
         * Skip '%'
         */
        format++;

        /*
         * End of format string:
         * Example: "Hello %"
         */
        if (*format == '\0')
        {
            LOG_AppendChar(
                buffer,
                &position,
                sizeof(buffer),
                '%'
            );

            break;
        }

        switch (*format)
        {
            case 'd':
            {
                int value = va_arg(args, int);

                LOG_AppendInteger(
                    buffer,
                    &position,
                    sizeof(buffer),
                    value
                );

                break;
            }

            case 'u':
            {
                unsigned int value =
                    va_arg(args, unsigned int);

                LOG_AppendUnsigned(
                    buffer,
                    &position,
                    sizeof(buffer),
                    value
                );

                break;
            }

            case 'x':
            case 'X':
            {
                unsigned int value =
                    va_arg(args, unsigned int);

                LOG_AppendHex(
                    buffer,
                    &position,
                    sizeof(buffer),
                    value
                );

                break;
            }

            case 'b':
            {
                unsigned int value =
                    va_arg(args, unsigned int);

                LOG_AppendBinary(
                    buffer,
                    &position,
                    sizeof(buffer),
                    value
                );

                break;
            }

            case 'c':
            {
                int value = va_arg(args, int);

                LOG_AppendChar(
                    buffer,
                    &position,
                    sizeof(buffer),
                    (char)value
                );

                break;
            }

            case 's':
            {
                const char *value =
                    va_arg(args, const char *);

                if (value != NULL)
                {
                    LOG_AppendString(
                        buffer,
                        &position,
                        sizeof(buffer),
                        value
                    );
                }
                else
                {
                    LOG_AppendString(
                        buffer,
                        &position,
                        sizeof(buffer),
                        "(null)"
                    );
                }

                break;
            }

            case '%':
            {
                LOG_AppendChar(
                    buffer,
                    &position,
                    sizeof(buffer),
                    '%'
                );

                break;
            }

            default:
            {
                /*
                 * Unknown format specifier.
                 *
                 * Example:
                 * %q -> "%q"
                 */
                LOG_AppendChar(
                    buffer,
                    &position,
                    sizeof(buffer),
                    '%'
                );

                LOG_AppendChar(
                    buffer,
                    &position,
                    sizeof(buffer),
                    *format
                );

                break;
            }
        }

        format++;
    }

    va_end(args);



}