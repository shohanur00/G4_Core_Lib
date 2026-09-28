
#include "uart_hal.h"
#include "stm32g431xx.h"
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
 * @brief Enable the peripheral clock for the selected UART.
 */
static void UART_HAL_EnableClock(USART_TypeDef *uart)
{
    if (uart == USART1)
    {
        RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
    }
    else if (uart == USART2)
    {
        RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN;
    }
    else if (uart == USART3)
    {
        RCC->APB1ENR1 |= RCC_APB1ENR1_USART3EN;
    }
#if defined(UART4) && defined(RCC_APB1ENR1_UART4EN)
    else if (uart == UART4)
    {
        RCC->APB1ENR1 |= RCC_APB1ENR1_UART4EN;
    }
#endif
#if defined(UART5) && defined(RCC_APB1ENR1_UART5EN)
    else if (uart == UART5)
    {
        RCC->APB1ENR1 |= RCC_APB1ENR1_UART5EN;
    }
#endif
#if defined(LPUART1) && defined(RCC_APB1ENR2_LPUART1EN)
    else if (uart == LPUART1)
    {
        RCC->APB1ENR2 |= RCC_APB1ENR2_LPUART1EN;
    }
#endif
}

/**
 * @brief Return the UART peripheral clock frequency.
 *
 * @note Temporary implementation:
 *       Assumes UART kernel clock = SystemCoreClock.
 *       Replace with clock-tree-aware implementation if
 *       APB prescalers or independent UART clocks are used.
 */
static uint32_t UART_HAL_GetClockHz(void)
{
    return SYSTEMCLOCK_FREQ_HZ;
}

/**
 * @brief Get DMA1 channel by channel number (1-7).
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
 * @brief Get corresponding DMAMUX channel.
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

    if ((uart == NULL) || (baudrate == 0U))
    {
        return;
    }

    /* Enable peripheral clock */
    UART_HAL_EnableClock(uart);

    /* Disable UART before configuration */
    uart->CR1 &= ~USART_CR1_UE;

    /* Reset UART configuration */
    uart->CR1 = 0U;
    uart->CR2 = 0U;
    uart->CR3 = 0U;

    /* Configure parity and word length */
    switch (parity)
    {
        case 0U: /* No parity: 8 data bits */
            break;

        case 1U: /* Even parity: 8 data bits + parity */
            uart->CR1 |= USART_CR1_M0 | USART_CR1_PCE;
            break;

        case 2U: /* Odd parity: 8 data bits + parity */
            uart->CR1 |= USART_CR1_M0 |
                         USART_CR1_PCE |
                         USART_CR1_PS;
            break;

        default:
            return;
    }

    /* Configure stop bits */
    switch (stop_bits)
    {
        case 0U: /* 1 stop bit */
            break;

        case 1U: /* 2 stop bits */
            uart->CR2 |= USART_CR2_STOP_1;
            break;

        default:
            return;
    }

    /* Configure baud rate: oversampling by 16 */
    uart_clock = UART_HAL_GetClockHz();

    if (uart_clock == 0U)
    {
        return;
    }

    uart->BRR = (uart_clock + (baudrate / 2U)) / baudrate;

    /* Enable transmitter */
    if (tx_enable != 0U)
    {
        uart->CR1 |= USART_CR1_TE;
    }

    /* Enable receiver */
    if (rx_enable != 0U)
    {
        uart->CR1 |= USART_CR1_RE;
    }

    /* Enable UART */
    uart->CR1 |= USART_CR1_UE;
}

/* ============================================================================
 * UART RX Interrupt Enable
 * ========================================================================== */

void UART_HAL_Rx_Interrupt_Enable(
    USART_TypeDef *uart
)
{
    if (uart == 0)
    {
        return;
    }

    /* Enable RX interrupt */
    uart->CR1 |= USART_CR1_RXNEIE_RXFNEIE;


    /* Enable corresponding NVIC interrupt */

    if (uart == USART1)
    {
        NVIC_EnableIRQ(USART1_IRQn);
    }
    else if (uart == USART2)
    {
        NVIC_EnableIRQ(USART2_IRQn);
    }
    else if (uart == USART3)
    {
        NVIC_EnableIRQ(USART3_IRQn);
    }

#if defined(UART4)
    else if (uart == UART4)
    {
        NVIC_EnableIRQ(UART4_IRQn);
    }
#endif
#if defined(UART5)
    else if (uart == UART5)
    {
        NVIC_EnableIRQ(UART5_IRQn);
    }
#endif
    else if (uart == LPUART1)
    {
        NVIC_EnableIRQ(LPUART1_IRQn);
    }
}


/* ============================================================================
 * UART TX Interrupt Enable
 * ========================================================================== */

void UART_HAL_Tx_Interrupt_Enable(
    USART_TypeDef *uart
)
{
    if (uart == 0)
    {
        return;
    }

    /* Enable TX interrupt */
    uart->CR1 |= USART_CR1_TXEIE_TXFNFIE;


    /* Enable corresponding NVIC interrupt */

    if (uart == USART1)
    {
        NVIC_EnableIRQ(USART1_IRQn);
    }
    else if (uart == USART2)
    {
        NVIC_EnableIRQ(USART2_IRQn);
    }
    else if (uart == USART3)
    {
        NVIC_EnableIRQ(USART3_IRQn);
    }
#if defined(UART4)
    else if (uart == UART4)
    {
        NVIC_EnableIRQ(UART4_IRQn);
    }
#endif
#if defined(UART5)
    else if (uart == UART5)
    {
        NVIC_EnableIRQ(UART5_IRQn);
    }
#endif 
    else if (uart == LPUART1)
    {
        NVIC_EnableIRQ(LPUART1_IRQn);
    }
}


/* ============================================================================
 * UART DMA TX Initialization
 * ========================================================================== */

void UART_HAL_DMA_Tx_Init(
    USART_TypeDef *uart,
    uint8_t dma_channel,
    uint32_t dma_request
)
{
   /* Configure DMA channel */

}

/* ============================================================================
 * UART DMA RX Initialization
 * ========================================================================== */

void UART_HAL_DMA_Rx_Init(
    USART_TypeDef *uart,
    uint8_t dma_channel,
    uint32_t dma_request
)
{
    /* Configure DMA channel */
}


void UART_HAL_WriteChar(
    USART_TypeDef *uart,
    char ch
)
{
    if (uart == NULL)
    {
        return;
    }

    /* Wait until transmit data register is empty */
    while ((uart->ISR & USART_ISR_TXE_TXFNF) == 0U)
    {
    }

    /* Write character to transmit data register */
    uart->TDR = (uint8_t)ch;

}


void UART_HAL_Write(
    USART_TypeDef *uart,
    const uint8_t *data,
    uint32_t length
)
{
    uint32_t i;

    if ((uart == NULL) || (data == NULL))
    {
        return;
    }

    for (i = 0U; i < length; i++)
    {
        /* Wait until transmit data register is empty */
        while ((uart->ISR & USART_ISR_TXE_TXFNF) == 0U)
        {
        }

        /* Write data */
        uart->TDR = data[i];
    }

    /* Wait until complete transmission */
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
    /* DMA TX implementation will be added here */
}