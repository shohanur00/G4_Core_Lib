#include "uart_hal.h"
#include "cdefs/cdefs.h"
#include "systemclock/systemclock.h"

/* ============================================================================
 * Private Configuration
 * ========================================================================== */

#define UART_DMA_CHANNEL_COUNT    7U

/* ============================================================================
 * Private Functions
 * ========================================================================== */

/**
 * @brief Enable clock for selected UART peripheral.
 */
static void UART_HAL_EnableClock(
    USART_TypeDef *uart
)
{
#if defined(USART1)

    if (uart == USART1)
    {
        RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
        return;
    }

#endif


#if defined(USART2)

    if (uart == USART2)
    {
        RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN;
        return;
    }

#endif


#if defined(USART3)

    if (uart == USART3)
    {
        RCC->APB1ENR1 |= RCC_APB1ENR1_USART3EN;
        return;
    }

#endif


#if defined(UART4) && defined(RCC_APB1ENR1_UART4EN)

    if (uart == UART4)
    {
        RCC->APB1ENR1 |= RCC_APB1ENR1_UART4EN;
        return;
    }

#endif


#if defined(UART5) && defined(RCC_APB1ENR1_UART5EN)

    if (uart == UART5)
    {
        RCC->APB1ENR1 |= RCC_APB1ENR1_UART5EN;
        return;
    }

#endif


#if defined(LPUART1) && defined(RCC_APB1ENR2_LPUART1EN)

    if (uart == LPUART1)
    {
        RCC->APB1ENR2 |= RCC_APB1ENR2_LPUART1EN;
        return;
    }

#endif
}


/**
 * @brief Get UART kernel clock frequency.
 *
 * @note Currently assumes UART clock equals system clock.
 */
static uint32_t UART_HAL_GetClockHz(void)
{
    return SYSTEMCLOCK_FREQ_HZ;
}


/**
 * @brief Get DMA channel pointer.
 */
static DMA_Channel_TypeDef *UART_HAL_GetDMAChannel(
    uint8_t channel
)
{
    if ((channel < 1U) ||
        (channel > UART_DMA_CHANNEL_COUNT))
    {
        return NULL;
    }

    return (DMA_Channel_TypeDef *)
        ((uintptr_t)DMA1_Channel1 +
         ((uint32_t)(channel - 1U) *
          sizeof(DMA_Channel_TypeDef)));
}


/**
 * @brief Get DMAMUX channel pointer.
 */
static DMAMUX_Channel_TypeDef *UART_HAL_GetDMAMUXChannel(
    uint8_t channel
)
{
    if ((channel < 1U) ||
        (channel > UART_DMA_CHANNEL_COUNT))
    {
        return NULL;
    }

    return (DMAMUX_Channel_TypeDef *)
        ((uintptr_t)DMAMUX1_Channel0 +
         ((uint32_t)(channel - 1U) *
          sizeof(DMAMUX_Channel_TypeDef)));
}


/* ============================================================================
 * UART Initialization
 * ========================================================================== */

void UART_HAL_Init(
    USART_TypeDef *uart,
    uint8_t        tx_enable,
    uint8_t        rx_enable,
    uint32_t       baudrate,
    uint8_t        parity,
    uint8_t        stop_bits
)
{
    uint32_t uart_clock;

    if ((uart == NULL) ||
        (baudrate == 0U))
    {
        return;
    }

    /* Enable peripheral clock */
    UART_HAL_EnableClock(uart);

    /* Disable UART */
    uart->CR1 &= ~USART_CR1_UE;

    /* Reset configuration */
    uart->CR1 = 0U;
    uart->CR2 = 0U;
    uart->CR3 = 0U;

    /* ------------------------------------------------------------------------
     * Parity
     * ---------------------------------------------------------------------- */

    switch (parity)
    {
        case 0U:
            /* No parity, 8-bit data */
            break;

        case 1U:
            /* Even parity */
            uart->CR1 |= USART_CR1_M0;
            uart->CR1 |= USART_CR1_PCE;
            break;

        case 2U:
            /* Odd parity */
            uart->CR1 |= USART_CR1_M0;
            uart->CR1 |= USART_CR1_PCE;
            uart->CR1 |= USART_CR1_PS;
            break;

        default:
            return;
    }

    /* ------------------------------------------------------------------------
     * Stop Bits
     * ---------------------------------------------------------------------- */

    switch (stop_bits)
    {
        case 0U:
            /* 1 stop bit */
            break;

        case 1U:
            /* 2 stop bits */
            uart->CR2 |= USART_CR2_STOP_1;
            break;

        default:
            return;
    }

    /* ------------------------------------------------------------------------
     * Baudrate
     * ---------------------------------------------------------------------- */

    uart_clock = UART_HAL_GetClockHz();

    if (uart_clock == 0U)
    {
        return;
    }

    uart->BRR =
        (uart_clock + (baudrate / 2U)) /
        baudrate;

    /* ------------------------------------------------------------------------
     * TX / RX
     * ---------------------------------------------------------------------- */

    if (tx_enable != 0U)
    {
        uart->CR1 |= USART_CR1_TE;
    }

    if (rx_enable != 0U)
    {
        uart->CR1 |= USART_CR1_RE;
    }

    /* Enable UART */
    uart->CR1 |= USART_CR1_UE;
}


/* ============================================================================
 * UART RX Status
 * ========================================================================== */

uint8_t UART_HAL_Rx_Ready(
    USART_TypeDef *uart
)
{
    if (uart == NULL)
    {
        return 0U;
    }

    return ((uart->ISR & USART_ISR_RXNE_RXFNE) != 0U)
           ? 1U
           : 0U;
}


/* ============================================================================
 * UART TX Status
 * ========================================================================== */

uint8_t UART_HAL_Tx_Ready(
    USART_TypeDef *uart
)
{
    if (uart == NULL)
    {
        return 0U;
    }

    return ((uart->ISR & USART_ISR_TXE_TXFNF) != 0U)
           ? 1U
           : 0U;
}


/* ============================================================================
 * UART TX Complete
 * ========================================================================== */

uint8_t UART_HAL_Tx_Complete(
    USART_TypeDef *uart
)
{
    if (uart == NULL)
    {
        return 0U;
    }

    return ((uart->ISR & USART_ISR_TC) != 0U)
           ? 1U
           : 0U;
}


/* ============================================================================
 * UART Read Byte
 * ========================================================================== */

uint8_t UART_HAL_ReadByte(
    USART_TypeDef *uart
)
{
    if (uart == NULL)
    {
        return 0U;
    }

    return (uint8_t)uart->RDR;
}


/* ============================================================================
 * UART RX Interrupt Enable
 * ========================================================================== */

void UART_HAL_Rx_Interrupt_Enable(
    USART_TypeDef *uart
)
{
    if (uart == NULL)
    {
        return;
    }

    uart->CR1 |= USART_CR1_RXNEIE_RXFNEIE;


#if defined(USART1)

    if (uart == USART1)
    {
        NVIC_EnableIRQ(USART1_IRQn);
        return;
    }

#endif


#if defined(USART2)

    if (uart == USART2)
    {
        NVIC_EnableIRQ(USART2_IRQn);
        return;
    }

#endif


#if defined(USART3)

    if (uart == USART3)
    {
        NVIC_EnableIRQ(USART3_IRQn);
        return;
    }

#endif


#if defined(UART4)

    if (uart == UART4)
    {
        NVIC_EnableIRQ(UART4_IRQn);
        return;
    }

#endif


#if defined(UART5)

    if (uart == UART5)
    {
        NVIC_EnableIRQ(UART5_IRQn);
        return;
    }

#endif


#if defined(LPUART1)

    if (uart == LPUART1)
    {
        NVIC_EnableIRQ(LPUART1_IRQn);
        return;
    }

#endif
}


/* ============================================================================
 * UART RX Interrupt Disable
 * ========================================================================== */

void UART_HAL_Rx_Interrupt_Disable(
    USART_TypeDef *uart
)
{
    if (uart == NULL)
    {
        return;
    }

    uart->CR1 &= ~USART_CR1_RXNEIE_RXFNEIE;
}


/* ============================================================================
 * UART TX Interrupt Enable
 * ========================================================================== */

void UART_HAL_Tx_Interrupt_Enable(
    USART_TypeDef *uart
)
{
    if (uart == NULL)
    {
        return;
    }

    uart->CR1 |= USART_CR1_TXEIE_TXFNFIE;


#if defined(USART1)

    if (uart == USART1)
    {
        NVIC_EnableIRQ(USART1_IRQn);
        return;
    }

#endif


#if defined(USART2)

    if (uart == USART2)
    {
        NVIC_EnableIRQ(USART2_IRQn);
        return;
    }

#endif


#if defined(USART3)

    if (uart == USART3)
    {
        NVIC_EnableIRQ(USART3_IRQn);
        return;
    }

#endif


#if defined(UART4)

    if (uart == UART4)
    {
        NVIC_EnableIRQ(UART4_IRQn);
        return;
    }

#endif


#if defined(UART5)

    if (uart == UART5)
    {
        NVIC_EnableIRQ(UART5_IRQn);
        return;
    }

#endif


#if defined(LPUART1)

    if (uart == LPUART1)
    {
        NVIC_EnableIRQ(LPUART1_IRQn);
        return;
    }

#endif
}


/* ============================================================================
 * UART TX Interrupt Disable
 * ========================================================================== */

void UART_HAL_Tx_Interrupt_Disable(
    USART_TypeDef *uart
)
{
    if (uart == NULL)
    {
        return;
    }

    uart->CR1 &= ~USART_CR1_TXEIE_TXFNFIE;
}


/* ============================================================================
 * UART DMA TX Initialization
 * ========================================================================== */

void UART_HAL_DMA_Tx_Init(
    USART_TypeDef *uart,
    uint8_t        dma_channel,
    uint32_t       dma_request
)
{
    DMA_Channel_TypeDef   *dma;
    DMAMUX_Channel_TypeDef *dmamux;

    if ((uart == NULL) ||
        (dma_channel < 1U) ||
        (dma_channel > UART_DMA_CHANNEL_COUNT))
    {
        return;
    }

    dma = UART_HAL_GetDMAChannel(dma_channel);

    dmamux = UART_HAL_GetDMAMUXChannel(dma_channel);

    if ((dma == NULL) ||
        (dmamux == NULL))
    {
        return;
    }

    /* Enable DMA1 clock */
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;

    /* Disable DMA channel before configuration */
    dma->CCR &= ~DMA_CCR_EN;

    /* Configure DMAMUX request */
    dmamux->CCR = dma_request;

    /* Configure DMA:
     * Memory -> Peripheral
     * Memory increment
     * Peripheral increment disabled
     * 8-bit peripheral size
     * 8-bit memory size
     */
    dma->CCR =
        DMA_CCR_MINC |
        DMA_CCR_DIR;

    dma->CPAR = (uint32_t)&uart->TDR;
}


/* ============================================================================
 * UART DMA RX Initialization
 * ========================================================================== */

void UART_HAL_DMA_Rx_Init(
    USART_TypeDef *uart,
    uint8_t        dma_channel,
    uint32_t       dma_request
)
{
    DMA_Channel_TypeDef    *dma;
    DMAMUX_Channel_TypeDef *dmamux;

    if ((uart == NULL) ||
        (dma_channel < 1U) ||
        (dma_channel > UART_DMA_CHANNEL_COUNT))
    {
        return;
    }

    dma = UART_HAL_GetDMAChannel(dma_channel);

    dmamux = UART_HAL_GetDMAMUXChannel(dma_channel);

    if ((dma == NULL) ||
        (dmamux == NULL))
    {
        return;
    }

    /* Enable DMA1 clock */
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;

    /* Disable DMA channel */
    dma->CCR &= ~DMA_CCR_EN;

    /* Configure DMAMUX request */
    dmamux->CCR = dma_request;

    /* Configure DMA:
     * Peripheral -> Memory
     * Memory increment
     * Peripheral increment disabled
     * 8-bit peripheral size
     * 8-bit memory size
     */
    dma->CCR =
        DMA_CCR_MINC;

    dma->CPAR = (uint32_t)&uart->RDR;
}


/* ============================================================================
 * UART Blocking Write Character
 * ========================================================================== */

void UART_HAL_WriteChar(
    USART_TypeDef *uart,
    char           ch
)
{
    if (uart == NULL)
    {
        return;
    }

    while ((uart->ISR & USART_ISR_TXE_TXFNF) == 0U)
    {
    }

    uart->TDR = (uint8_t)ch;
}


/* ============================================================================
 * UART Blocking Write
 * ========================================================================== */

void UART_HAL_Write(
    USART_TypeDef *uart,
    const uint8_t *data,
    uint32_t       length
)
{
    uint32_t i;

    if ((uart == NULL) ||
        (data == NULL))
    {
        return;
    }

    for (i = 0U; i < length; i++)
    {
        while ((uart->ISR & USART_ISR_TXE_TXFNF) == 0U)
        {
        }

        uart->TDR = data[i];
    }

    while ((uart->ISR & USART_ISR_TC) == 0U)
    {
    }
}


/* ============================================================================
 * UART DMA Write
 * ========================================================================== */

void UART_HAL_DMA_Write(
    USART_TypeDef *uart,
    const uint8_t *data,
    uint32_t       length
)
{
    /*
     * DMA transfer implementation will be added
     * when DMA channel ownership/configuration is
     * defined by the upper Driver layer.
     */

    (void)uart;
    (void)data;
    (void)length;
}


bool UART_HAL_Rx_IRQHandler(
    USART_TypeDef *uart,
    uint8_t       *data
)
{
    if ((uart == NULL) || (data == NULL))
    {
        return false;
    }

    if ((uart->ISR & USART_ISR_RXNE_RXFNE) == 0U)
    {
        return false;
    }

    *data = (uint8_t)(uart->RDR & 0xFFU);

    return true;
}