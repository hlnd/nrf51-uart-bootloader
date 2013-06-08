#include "nrf.h"
#include "nrf_gpio.h"

#include "uartreader.h"

static uint8_t m_buffer_index = 0;
static uartreader_handler_t m_evt_handler;
static uartreader_evt_t m_evt;

void UART0_IRQHandler(void)
{
    if (NRF_UART0->EVENTS_RXDRDY)
    {
        NRF_UART0->EVENTS_RXDRDY = 0;
        if (m_buffer_index == 0)
        {
            m_evt.cmd = (uartreader_cmd_t) NRF_UART0->RXD;
            m_buffer_index++;
        }
        else
        {
            m_evt.data[m_buffer_index-1] = NRF_UART0->RXD;

            if (m_evt.data[m_buffer_index-1] == '\n' || 
                m_buffer_index >= UARTREADER_MAX_LEN-1)
            {
                m_evt.len = m_buffer_index-1;
                m_evt_handler(&m_evt);
                
                m_buffer_index = 0;
            }
            else
            {
                m_buffer_index++;
            }
        }
    }
}

void uartreader_init(uartreader_init_t * init)
{
    NRF_UART0->BAUDRATE = init->baudrate << UART_BAUDRATE_BAUDRATE_Pos;

    nrf_gpio_cfg_input(init->rxd_pin_no, NRF_GPIO_PIN_NOPULL);
    nrf_gpio_cfg_output(init->txd_pin_no);

    NRF_UART0->PSELTXD = init->txd_pin_no; 
    NRF_UART0->PSELRXD = init->rxd_pin_no; 

    if (init->hwfc)
    {
        nrf_gpio_cfg_input(init->cts_pin_no, NRF_GPIO_PIN_NOPULL);
        nrf_gpio_cfg_output(init->rts_pin_no);

        NRF_UART0->PSELCTS = init->cts_pin_no;
        NRF_UART0->PSELRTS = init->rts_pin_no;

        NRF_UART0->CONFIG = UART_CONFIG_HWFC_Enabled << UART_CONFIG_HWFC_Pos;
    }
    else
    {
        NRF_UART0->CONFIG = UART_CONFIG_HWFC_Disabled << UART_CONFIG_HWFC_Pos;
    }

    m_evt_handler = init->evt_handler;
    m_buffer_index = 0;

    NRF_UART0->INTENSET = UART_INTENSET_RXDRDY_Enabled << UART_INTENSET_RXDRDY_Pos;
    NVIC_EnableIRQ(UART0_IRQn);

    NRF_UART0->ENABLE = UART_ENABLE_ENABLE_Enabled << UART_ENABLE_ENABLE_Pos;
    NRF_UART0->TASKS_STARTRX = 1;
}

void uartreader_send_response(uartreader_response_t rsp)
{
	NRF_UART0->TXD = (uint8_t) rsp;

	NRF_UART0->TASKS_STARTTX = 1;
}
