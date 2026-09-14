#include "logger.h"
#include <stddef.h>
#include "HAL/logger_hal.h"


#define LOG_BUFFER_SIZE    128U
static LOG_Level_t module_levels[LOG_MODULE_MAX];
static uint32_t log_timestamp = 0U; 


/* Human-readable names, indexed by enum value (extend as modules are added) */
static const char *const module_names[LOG_MODULE_MAX] =
{
    [LOG_MODULE_SYSTEM] = "SYSTEM"
};

static const char *const level_strings[LOG_LEVEL_CRITICAL + 1U] =
{
    [LOG_LEVEL_DEBUG]    = "DBG",
    [LOG_LEVEL_INFO]     = "INF",
    [LOG_LEVEL_WARNING]  = "WRN",
    [LOG_LEVEL_ERROR]    = "ERR",
    [LOG_LEVEL_CRITICAL] = "CRI"
};

#if LOG_USE_COLOR
static const char *const level_colors[LOG_LEVEL_CRITICAL + 1U] =
{
    [LOG_LEVEL_DEBUG]    = "\x1b[92m",     /* Bright Green */
    [LOG_LEVEL_INFO]     = "\x1b[36m",     /* Cyan       */
    [LOG_LEVEL_WARNING]  = "\x1b[33m",     /* Yellow     */
    [LOG_LEVEL_ERROR]    = "\x1b[31m",     /* Red        */
    [LOG_LEVEL_CRITICAL] = "\x1b[1;91m"    /* Bold Bright Red */
};

#define LOG_COLOR_RESET  "\x1b[0m"
#endif


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


#if LOG_USE_TIMESTAMP
static void LOG_AppendUnsignedPadded(
    char *buffer,
    size_t *position,
    size_t size,
    unsigned int value,
    int width
)
{
    char temp[10];
    int index = 0;

    if (value == 0U)
    {
        temp[index++] = '0';
    }
    else
    {
        while (value > 0U)
        {
            temp[index++] = (char)('0' + (value % 10U));
            value /= 10U;
        }
    }

    for (int pad = index; pad < width; pad++)
    {
        LOG_AppendChar(buffer, position, size, '0');
    }

    while (index > 0)
    {
        LOG_AppendChar(buffer, position, size, temp[--index]);
    }
}
#endif


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
    for (LOG_Module_t module = LOG_MODULE_SYSTEM;
         module < LOG_MODULE_MAX;
         module++)
    {
        module_levels[module] = LOG_LEVEL_INFO;
    }
    LOG_HAL_Init();
}

void LOG_SetModuleLevel(LOG_Module_t module, LOG_Level_t level){
    // Set the logging level for the specified module
    if (module >= LOG_MODULE_MAX)
    {
        return;
    }

    if (level > LOG_LEVEL_CRITICAL)
    {
        return;
    }

    module_levels[module] = level; 
}

void LOG_Write(LOG_Module_t module, LOG_Level_t level, const char *format, ...){
    /*
     * Runtime filtering: bail out before touching the buffer if the
     * module index is invalid or this message is below the module's
     * configured threshold.
     */
    if ((module >= LOG_MODULE_MAX) || (level < module_levels[module]))
    {
        return;
    }

    char buffer[LOG_BUFFER_SIZE];
    size_t position = 0U;

    va_list args;

    buffer[0] = '\0';



#if LOG_USE_TIMESTAMP
    LOG_AppendChar(buffer, &position, sizeof(buffer), '[');
    LOG_AppendUnsignedPadded(
        buffer,
        &position,
        sizeof(buffer),
        log_timestamp,
        8
    );
    LOG_AppendChar(buffer, &position, sizeof(buffer), ']');
#endif

#if LOG_USE_COLOR
    LOG_AppendString(buffer, &position, sizeof(buffer), level_colors[level]);
#endif

#if LOG_USE_LEVEL_TAG
    LOG_AppendChar(buffer, &position, sizeof(buffer), '[');
    LOG_AppendString(buffer, &position, sizeof(buffer), level_strings[level]);
    LOG_AppendChar(buffer, &position, sizeof(buffer), ']');
#endif


#if LOG_USE_MODULE_NAME
    LOG_AppendChar(buffer, &position, sizeof(buffer), '[');
    LOG_AppendString(buffer, &position, sizeof(buffer), module_names[module]);
    LOG_AppendChar(buffer, &position, sizeof(buffer), ']');
#endif


#if LOG_USE_COLOR
    LOG_AppendString(buffer, &position, sizeof(buffer), LOG_COLOR_RESET);
#endif

#if (LOG_USE_TIMESTAMP || LOG_USE_LEVEL_TAG || LOG_USE_MODULE_NAME)
    LOG_AppendChar(buffer, &position, sizeof(buffer), ' ');
#endif

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


#if LOG_USE_NEWLINE
    LOG_AppendString(buffer, &position, sizeof(buffer), "\r\n");
#endif

    /* Hand the finished line off to the HAL for actual transmission */
    LOG_HAL_Write(buffer, position);
}


void LOG_MainLoop(uint32_t ref_time)
{
    #if LOG_USE_TIMESTAMP
        log_timestamp = ref_time;
    #endif
}