#include "nrf.h"

#include "uartreader.h"

static uint8_t m_buffer[UARTREADER_MAX_LEN];
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
            m_evt.data[m_buffer_index] = NRF_UART0->RXD;

            if (m_evt.data[m_buffer_index] == '\n' || 
                m_buffer_index >= UARTREADER_MAX_LEN-1)
            {
                m_evt.len = m_buffer_index;
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
    NRF_UART0->CONFIG = UART_CONFIG_HWFC_Disabled << UART_CONFIG_HWFC_Pos;

    NRF_UART0->PSELTXD = init->txd_pin_no; 
    NRF_UART0->PSELRXD = init->rxd_pin_no; 
 
    m_evt_handler = init->evt_handler;
    m_buffer_index = 0;

    NRF_UART0->ENABLE = UART_ENABLE_ENABLE_Enabled << UART_ENABLE_ENABLE_Pos;
 
    NRF_UART0->INTENSET = UART_INTENSET_TXDRDY_Enabled << UART_INTENSET_TXDRDY_Pos;
    NVIC_EnableIRQ(UART0_IRQn);
}

void uartreader_send_reply(uartreader_rsp_t rsp)
{

}
