#include "uart_driver.h"
#include "uart/uart_hal.h"
#include "cdefs/cdefs.h"
#include "containers/ringbuffer/ring_buffer.h"



/* ============================================================================
 * UART Ring Buffer Configuration
 * ========================================================================== */

#define UART_INSTANCE_COUNT     6U
#define UART_RX_BUFFER_SIZE     256U

#define UART_INVALID_INDEX      0xFFU


/* ============================================================================
 * UART RX Ring Buffers
 * ========================================================================== */ 

static RingBuffer_t uart_rx_ring_buffer[UART_INSTANCE_COUNT];

static volatile uint8_t uart_rx_storage[UART_INSTANCE_COUNT][UART_RX_BUFFER_SIZE];


/* ============================================================================
 * Private Functions
 * ========================================================================== */


/**
 * @brief Get UART array index from UART instance number.
 *
 * @param instance UART instance number.
 *
 * @return Zero-based UART index.
 *         UART_INVALID_INDEX if invalid.
 */
static uint8_t UART_Driver_GetIndex(
    uint8_t instance
)
{
    if ((instance == 0U) ||
        (instance > UART_INSTANCE_COUNT))
    {
        return UART_INVALID_INDEX;
    }

    return (uint8_t)(instance - 1U);
}


/**
 * @brief Get UART peripheral instance.
 *
 * @param instance UART instance number.
 *
 * @return Pointer to UART peripheral.
 *         NULL if instance is not available.
 */
static USART_TypeDef *UART_Driver_GetInstance(
    uint8_t instance
)
{
    switch (instance)
    {
        case 1U:

#if defined(USART1)

            return USART1;

#endif

        case 2U:

#if defined(USART2)

            return USART2;

#endif

        case 3U:

#if defined(USART3)

            return USART3;

#endif

        case 4U:

#if defined(UART4)

            return UART4;

#endif

        case 5U:

#if defined(UART5)

            return UART5;

#endif

        case 6U:

#if defined(LPUART1)

            return LPUART1;

#endif

        default:
            return NULL;
    }

    return NULL;
}


/* ============================================================================
 * UART Initialization
 * ========================================================================== */

/**
 * @brief Initialize RX ring buffer for a UART instance.
 *
 * @param instance UART instance number.
 */
static void UART_Driver_RxBuffer_Init(
    uint8_t instance
)
{
    uint8_t index;

    index = UART_Driver_GetIndex(instance);

    if (index == UART_INVALID_INDEX)
    {
        return;
    }

    RingBuffer_Setup(
        &uart_rx_ring_buffer[index],
        uart_rx_storage[index],
        UART_RX_BUFFER_SIZE
    );
}


void UART_Driver_Init(
    uint8_t         instance,
    UART_Enable_t   tx_enable,
    UART_Enable_t   rx_enable,
    uint32_t        baudrate,
    UART_Parity_t   parity,
    UART_StopBits_t stop_bits
)
{
    USART_TypeDef *uart;

    uart = UART_Driver_GetInstance(instance);

    if (uart == NULL)
    {
        return;
    }

    UART_HAL_Init(
        uart,
        tx_enable,
        rx_enable,
        baudrate,
        parity,
        stop_bits
    );

    if (rx_enable == UART_ENABLE)
    {
        UART_Driver_RxBuffer_Init(instance);
    }
}





/* ============================================================================
 * UART RX Interrupt Enable
 * ========================================================================== */

void UART_Driver_Rx_Interrupt_Enable(
    uint8_t instance
)
{
    USART_TypeDef *uart;

    uart = UART_Driver_GetInstance(instance);

    if (uart == NULL)
    {
        return;
    }

    UART_HAL_Rx_Interrupt_Enable(uart);
}


/* ============================================================================
 * UART RX Interrupt Disable
 * ========================================================================== */

void UART_Driver_Rx_Interrupt_Disable(
    uint8_t instance
)
{
    USART_TypeDef *uart;

    uart = UART_Driver_GetInstance(instance);

    if (uart == NULL)
    {
        return;
    }

    UART_HAL_Rx_Interrupt_Disable(uart);
}


/* ============================================================================
 * UART TX Interrupt Enable
 * ========================================================================== */

void UART_Driver_Tx_Interrupt_Enable(
    uint8_t instance
)
{
    USART_TypeDef *uart;

    uart = UART_Driver_GetInstance(instance);

    if (uart == NULL)
    {
        return;
    }

    UART_HAL_Tx_Interrupt_Enable(uart);
}


/* ============================================================================
 * UART TX Interrupt Disable
 * ========================================================================== */

void UART_Driver_Tx_Interrupt_Disable(
    uint8_t instance
)
{
    USART_TypeDef *uart;

    uart = UART_Driver_GetInstance(instance);

    if (uart == NULL)
    {
        return;
    }

    UART_HAL_Tx_Interrupt_Disable(uart);
}


/* ============================================================================
 * UART RX Status
 * ========================================================================== */

uint8_t UART_Driver_Rx_Ready(
    uint8_t instance
)
{
    USART_TypeDef *uart;

    uart = UART_Driver_GetInstance(instance);

    if (uart == NULL)
    {
        return 0U;
    }

    return UART_HAL_Rx_Ready(uart);
}

/* ============================================================================
 * UART Read Byte
 * ========================================================================== */

bool UART_Driver_ReadByte(
    uint8_t instance,
    uint8_t *data
)
{
    uint8_t index;

    if (data == NULL)
    {
        return false;
    }

    if ((instance == 0U) ||
        (instance > UART_INSTANCE_COUNT))
    {
        return false;
    }

    index = (uint8_t)(instance - 1U);

    return RingBuffer_Read(
        &uart_rx_ring_buffer[index],
        data
    );
}


/* ============================================================================
 * UART TX Status
 * ========================================================================== */

uint8_t UART_Driver_Tx_Ready(
    uint8_t instance
)
{
    USART_TypeDef *uart;

    uart = UART_Driver_GetInstance(instance);

    if (uart == NULL)
    {
        return 0U;
    }

    return UART_HAL_Tx_Ready(uart);
}


/* ============================================================================
 * UART TX Complete
 * ========================================================================== */

uint8_t UART_Driver_Tx_Complete(
    uint8_t instance
)
{
    USART_TypeDef *uart;

    uart = UART_Driver_GetInstance(instance);

    if (uart == NULL)
    {
        return 0U;
    }

    return UART_HAL_Tx_Complete(uart);
}


/* ============================================================================
 * UART Write Character
 * ========================================================================== */

void UART_Driver_WriteChar(
    uint8_t instance,
    char    ch
)
{
    USART_TypeDef *uart;

    uart = UART_Driver_GetInstance(instance);

    if (uart == NULL)
    {
        return;
    }

    UART_HAL_WriteChar(
        uart,
        ch
    );
}


/* ============================================================================
 * UART Write
 * ========================================================================== */

void UART_Driver_Write(
    uint8_t        instance,
    const uint8_t *data,
    uint32_t       length
)
{
    USART_TypeDef *uart;

    uart = UART_Driver_GetInstance(instance);

    if (uart == NULL)
    {
        return;
    }

    UART_HAL_Write(
        uart,
        data,
        length
    );
}


/* ============================================================================
 * UART DMA TX Initialization
 * ========================================================================== */

void UART_Driver_DMA_Tx_Init(
    uint8_t  instance,
    uint8_t  dma_channel,
    uint32_t dma_request
)
{
    USART_TypeDef *uart;

    uart = UART_Driver_GetInstance(instance);

    if (uart == NULL)
    {
        return;
    }

    UART_HAL_DMA_Tx_Init(
        uart,
        dma_channel,
        dma_request
    );
}


/* ============================================================================
 * UART DMA RX Initialization
 * ========================================================================== */

void UART_Driver_DMA_Rx_Init(
    uint8_t  instance,
    uint8_t  dma_channel,
    uint32_t dma_request
)
{
    USART_TypeDef *uart;

    uart = UART_Driver_GetInstance(instance);

    if (uart == NULL)
    {
        return;
    }

    UART_HAL_DMA_Rx_Init(
        uart,
        dma_channel,
        dma_request
    );
}


/* ============================================================================
 * UART DMA Write
 * ========================================================================== */

void UART_Driver_DMA_Write(
    uint8_t        instance,
    const uint8_t *data,
    uint32_t       length
)
{
    USART_TypeDef *uart;

    uart = UART_Driver_GetInstance(instance);

    if (uart == NULL)
    {
        return;
    }

    UART_HAL_DMA_Write(
        uart,
        data,
        length
    );
}


/* ============================================================================
 * UART IRQ Handler
 * ========================================================================== */


static uint8_t UART_Driver_GetRxBufferIndex(
    USART_TypeDef *uart
)
{
#if defined(USART1)
    if (uart == USART1)
    {
        return 0U;
    }
#endif

#if defined(USART2)
    if (uart == USART2)
    {
        return 1U;
    }
#endif

#if defined(USART3)
    if (uart == USART3)
    {
        return 2U;
    }
#endif

#if defined(UART4)
    if (uart == UART4)
    {
        return 3U;
    }
#endif

#if defined(UART5)
    if (uart == UART5)
    {
        return 4U;
    }
#endif

#if defined(LPUART1)
    if (uart == LPUART1)
    {
        return 5U;
    }
#endif

    return UART_INVALID_INDEX;
}


void UART_Driver_IRQHandler(
    USART_TypeDef *uart
)
{
    uint8_t data;
    uint8_t index;

    if (uart == NULL)
    {
        return;
    }

    index = UART_Driver_GetRxBufferIndex(uart);

    if (index == UART_INVALID_INDEX)
    {
        return;
    }

    if (UART_HAL_Rx_IRQHandler(uart, &data))
    {
        (void)RingBuffer_Write(
            &uart_rx_ring_buffer[index],
            data
        );
    }
}



uint16_t UART_Driver_DataAvailable(uint8_t instance)
{
    if ((instance == 0U) ||
        (instance > UART_INSTANCE_COUNT))
    {
        return 0U;
    }

    return RingBuffer_Count(
        &uart_rx_ring_buffer[instance - 1U]
    );
}


/* ============================================================================
 * UART Interrupt Service Routines
 * ========================================================================== */

#if defined(USART1)

void USART1_IRQHandler(void)
{
    UART_Driver_IRQHandler(USART1);
}

#endif


#if defined(USART2)

void USART2_IRQHandler(void)
{
    UART_Driver_IRQHandler(USART2);
}

#endif


#if defined(USART3)

void USART3_IRQHandler(void)
{
    UART_Driver_IRQHandler(USART3);
}

#endif


#if defined(UART4)

void UART4_IRQHandler(void)
{
    UART_Driver_IRQHandler(UART4);
}

#endif


#if defined(UART5)

void UART5_IRQHandler(void)
{
    UART_Driver_IRQHandler(UART5);
}

#endif


#if defined(LPUART1)

void LPUART1_IRQHandler(void)
{
    UART_Driver_IRQHandler(LPUART1);
}

#endif


