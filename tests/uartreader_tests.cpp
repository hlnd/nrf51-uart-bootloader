#include "CppUTest/TestHarness.h"

extern "C"
{
    #include "uartreader.h"
    #include "nrf.h"
    #include "nrf_error.h"

    bool is_handler_called = false;

    extern void UART0_IRQHandler(void);

    void uartreader_handler(uartreader_evt_t * evt)
    {
        is_handler_called = true;
    }
}

#include <sys/mman.h>

#define FICR_SIZE 0x2000
#define PERIPHERALS_SIZE 0x20000
#define GPIO_SIZE 0x1000
#define NVIC_SIZE 0x1000

TEST_GROUP(uartreader)
{
    void * ficr;
    void * peripherals;
    void * gpio;
    void * nvic;

    void setup(void)
    {   
        // Allocate space for FICR, UICR
        ficr = mmap((void *) 0x10000000, FICR_SIZE, PROT_READ | PROT_WRITE, MAP_ANON|MAP_SHARED, -1, 0); 
        // Allocate space for all normal peripherals
        peripherals = mmap((void *) 0x40000000, PERIPHERALS_SIZE, PROT_READ | PROT_WRITE, MAP_ANON|MAP_SHARED, -1, 0); 
        // Allocate space for GPIO
        gpio = mmap((void *) 0x50000000, GPIO_SIZE, PROT_READ | PROT_WRITE, MAP_ANON|MAP_SHARED, -1, 0); 
        // Allocate space for NVIC
        nvic = mmap((void *) 0xE000E000, NVIC_SIZE, PROT_READ | PROT_WRITE, MAP_ANON|MAP_SHARED, -1, 0); 

        is_handler_called = false;
    }   
 
    void teardown(void)
    {   
        munmap(ficr, FICR_SIZE);
        munmap(peripherals, PERIPHERALS_SIZE);
        munmap(gpio, GPIO_SIZE);
        munmap(nvic, NVIC_SIZE);
    }   

};

TEST(uartreader, test_uartreader_init)
{
    uartreader_init_t init;
    init.txd_pin_no = 12;
    init.rxd_pin_no = 23;
    init.baudrate = UART_BAUDRATE_BAUDRATE_Baud38400;
    init.evt_handler = &uartreader_handler;

    uartreader_init(&init);

    LONGS_EQUAL(UART_ENABLE_ENABLE_Enabled << UART_ENABLE_ENABLE_Pos, NRF_UART0->ENABLE);
    LONGS_EQUAL(UART_BAUDRATE_BAUDRATE_Baud38400, NRF_UART0->BAUDRATE);
    LONGS_EQUAL(1, NRF_UART0->TASKS_STARTRX);
    LONGS_EQUAL(12, NRF_UART0->PSELTXD);
    LONGS_EQUAL(23, NRF_UART0->PSELRXD);
}

TEST(uartreader, test_uartreader_calls_handler_on_newline)
{
    uartreader_init_t init;
    init.txd_pin_no = 12;
    init.rxd_pin_no = 23;
    init.baudrate = UART_BAUDRATE_BAUDRATE_Baud38400;
    init.evt_handler = &uartreader_handler;

    uartreader_init(&init);

    CHECK_FALSE(is_handler_called);

    NRF_UART0->RXD = 0x01;
    NRF_UART0->EVENTS_RXDRDY = 1;
    UART0_IRQHandler();

    CHECK_FALSE(is_handler_called);

    NRF_UART0->RXD = '\n';
    NRF_UART0->EVENTS_RXDRDY = 1;
    UART0_IRQHandler();

    CHECK_TRUE(is_handler_called);
}

TEST(uartreader, test_uartreader_calls_handler_on_max_length)
{
    uartreader_init_t init;
    init.txd_pin_no = 12;
    init.rxd_pin_no = 23;
    init.baudrate = UART_BAUDRATE_BAUDRATE_Baud38400;
    init.evt_handler = &uartreader_handler;

    uartreader_init(&init);

    for (uint8_t i = 0; i < UARTREADER_MAX_LEN; i++)
    {
        CHECK_FALSE(is_handler_called);

        NRF_UART0->RXD = 0x01;
        NRF_UART0->EVENTS_RXDRDY = 1;
        UART0_IRQHandler();

    }
    CHECK_TRUE(is_handler_called);
}

TEST(uartreader, test_writes_data_to_txd)
{
	uartreader_send_reply(SUCCESS);

	LONGS_EQUAL(SUCCESS, NRF_UART0->TXD);
	LONGS_EQUAL(1, NRF_UART0->TASKS_STARTTX);
}
