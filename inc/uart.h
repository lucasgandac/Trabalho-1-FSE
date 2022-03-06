#ifndef UART_H
#define UART_H

#define GET_INTERNAL_TEMP 0xC1
#define GET_POTENTIOMETER 0xC2
#define GET_KEY_VALUE 0xC3
#define SEND_SIGNAL 0xD1
#define SEND_REFERENCE_SIGNAL 0xD2
#define SEND_SYSTEM_STATE 0xD3
#define SEND_CONTROL_MODE 0xD4

typedef struct Number_type {
  int int_value;
  float float_value;
} Number_type;


int connect_uart();
void write_uart_get(int filestream, unsigned char code);
void write_uart_send(int filestream, int value);
Number_type read_uart(int filestream, unsigned char code);
void close_uart(int filestream);

#endif /* UART_H */
