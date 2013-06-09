#ifndef UARTREADER_H_INCLUDED
#define UARTREADER_H_INCLUDED

#include <stdbool.h>

#define UARTREADER_MAX_LEN 48

typedef enum
{
    NOP = 0x00,
    ERASE_APP,
    WRITE_LINE,
    RESET_AND_RUN,
} uartreader_cmd_t;

typedef enum
{
    SUCCESS = 0x01,
    FAILURE = 0x02,
} uartreader_response_t;

typedef struct
{
    uartreader_cmd_t cmd;
    uint8_t len;
    uint8_t data[UARTREADER_MAX_LEN];
} uartreader_evt_t;

typedef void (*uartreader_handler_t)(uartreader_evt_t * event);

typedef struct 
{
    uint8_t txd_pin_no;
    uint8_t rxd_pin_no;
    uint8_t rts_pin_no;
    uint8_t cts_pin_no;
    bool hwfc;
    uint32_t baudrate;
    uartreader_handler_t evt_handler;
} uartreader_init_t;

void uartreader_init(uartreader_init_t * init);
void uartreader_send_response(uartreader_response_t rsp);

#endif
