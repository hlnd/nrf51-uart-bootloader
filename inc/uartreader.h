#ifndef UARTREADER_H_INCLUDED
#define UARTREADER_H_INCLUDED

#define UARTREADER_MAX_LEN 32

typedef enum
{
    NOP = 0x00,
    ERASE_ALL,
    ERASE_PAGE,
    START_WRITE,
    WRITE_LINE,
    RESET_AND_RUN,
} uartreader_cmd_t;

typedef enum
{
    SUCCESS = 0x00,
} uartreader_rsp_t;

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
    uint32_t baudrate;
    uartreader_handler_t evt_handler;
} uartreader_init_t;

void uartreader_init(uartreader_init_t * init);
void uartreader_send_reply(uartreader_rsp_t rsp);

#endif
