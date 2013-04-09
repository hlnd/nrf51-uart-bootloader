#include "nrf51.h"
#include "uartreader.h"

void uart_handler()
{
    /*switch (evt.type)
    {
        case LINE_RECEIVED:
            break;
    }*/
}

int main(void)
{
    //uartreader_init();

    while (1)
    {
        __WFI();
    }

}
