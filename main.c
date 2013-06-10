#include "nrf.h"
#include "nrf_gpio.h"
#include "boards.h"
#include "uartreader.h"
#include "hexparser.h"

#define APPLICATION_BASE_ADDRESS 0x14000

volatile bool is_bootloader_running = true;

typedef void (*application_main_t)(void);

void init(void)
{
    if (NRF_UICR->CLENR0 == 0xFFFFFFFF)
    {
        NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Wen << NVMC_CONFIG_WEN_Pos;
        NRF_UICR->CLENR0 = APPLICATION_BASE_ADDRESS;
        while (!NRF_NVMC->READY);
        NRF_NVMC->CONFIG = 0;
        NVIC_SystemReset();
    }
}

void erase_app(void)
{
    uint32_t last_page_address = NRF_FICR->CODEPAGESIZE * NRF_FICR->CODESIZE;

    NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Een << NVMC_CONFIG_WEN_Pos;

    for (uint32_t address = APPLICATION_BASE_ADDRESS; address < last_page_address; address += NRF_FICR->CODEPAGESIZE)
    {
        NRF_NVMC->ERASEPAGE = address;
        while (!NRF_NVMC->READY);
    }

    NRF_NVMC->CONFIG = 0x0;
}

void write_record(uint16_t base_address, hexparser_record * record)
{
    NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Wen << NVMC_CONFIG_WEN_Pos;

    for (uint32_t i = 0; i < record->byte_count / 4; i++)
    {
        *(uint32_t *) (base_address << 4 | (record->address + (4*i))) = record->data.words[i];
        while (NRF_NVMC->READY & (NVMC_READY_READY_Busy << NVMC_READY_READY_Pos))
        {
        }
    }
    
    NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Ren << NVMC_CONFIG_WEN_Pos;
}

uartreader_response_t write_line(uartreader_evt_t * evt)
{
    nrf_gpio_pin_toggle(LED0);
    static uint16_t base_address = 0;
    hexparser_record record;
    hexparser_parse_string((char * ) evt->data, evt->len, &record);

    if (!hexparser_is_record_valid(&record))
    {
        return FAILURE;
    }

    switch (record.type)
    {
        case EXTENDED_LINEAR_ADDRESS_RECORD:
        case EXTENDED_SEGMENT_ADDRESS_RECORD:
            nrf_gpio_pin_toggle(LED1);
            base_address = record.data.words[0];
            break;

        case DATA_RECORD:
            nrf_gpio_pin_toggle(LED0);
            write_record(base_address, &record);
            break;

        default:
            nrf_gpio_pin_toggle(LED0);
            nrf_gpio_pin_toggle(LED1);
            break;
    }



    return SUCCESS;
}
void uart_handler(uartreader_evt_t * evt)
{
    uartreader_response_t result;
    switch (evt->cmd)
    {
        case ERASE_APP:
            erase_app();
            uartreader_send_response(SUCCESS);
            break;

        case WRITE_LINE:
            result = write_line(evt);
            uartreader_send_response(result);
            break;

        case RESET_AND_RUN:
            uartreader_send_response(SUCCESS);
            NVIC_SystemReset();
            break;

        case NOP:
            uartreader_send_response(SUCCESS);
            break;
    }
}

int main(void)
{
    nrf_gpio_cfg_output(LED0);
    nrf_gpio_cfg_output(LED1);
    nrf_gpio_cfg_input(BUTTON0, NRF_GPIO_PIN_PULLUP);

    init();

    application_main_t application_main = *(application_main_t *)(APPLICATION_BASE_ADDRESS+4);
    if ((application_main != (application_main_t) 0xFFFFFFFF) && 
        (nrf_gpio_pin_read(BUTTON0) != 0))
    {
        is_bootloader_running = false;
        application_main();
        
    }

    is_bootloader_running = true;
    nrf_gpio_pin_set(LED0);

    uartreader_init_t init;
    init.baudrate = UART_BAUDRATE_BAUDRATE_Baud38400;
    init.txd_pin_no = TX_PIN_NUMBER;
    init.rxd_pin_no = RX_PIN_NUMBER;
    init.cts_pin_no = CTS_PIN_NUMBER;
    init.rts_pin_no = RTS_PIN_NUMBER;
    init.hwfc = HWFC;
    init.evt_handler = uart_handler;
    uartreader_init(&init);

    while (1)
    {
        __WFI();
    }

}
