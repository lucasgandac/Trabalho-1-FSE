#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "bme280.h"
#include "display.h"
#include "gpio.h"
#include "pid.h"
#include "thermometer.h"
#include "uart.h"

#define true 1
#define false 0

int use_terminal = false;
struct bme280_dev bme_connection;
int uart_filesystem, key_gpio = 1;

void shutdown_program() {
  system("clear");
  printf("Programa encerrado\n");
  turn_resistance_off();
  turn_fan_off();
  close_uart(uart_filesystem);
  exit(0);
}

void toggle_routine() {
  if (use_terminal) {
    if (key_gpio == 1) {
      key_gpio = 0;
    } else
      key_gpio = 1;
  }
  else 
    printf("\nComando inválido para opção chave, reinicie o programa\n");
}


void pid_routine(int key) {
  system("clear");
  float hysteresis, TI, TR, TE;
  int value_to_send = 0;
  printf("\n------------------ Rotina PID iniciada ------------------n");
  pid_constant(5, 1, 5);
  do {
    write_uart_get(uart_filesystem, GET_INTERNAL_TEMP);
    TI = read_uart(uart_filesystem, GET_INTERNAL_TEMP).float_value;

    double value_to_send = pid_control(TI);

    pwm_control(value_to_send);

    write_uart_get(uart_filesystem, GET_POTENTIOMETER);
    TR = read_uart(uart_filesystem, GET_POTENTIOMETER).float_value;

    pid_reference(TR);

    TE = get_current_temperature(&bme_connection);
    printf("\tTI: %.2f⁰C - TR: %.2f⁰C - TE: %.2f⁰C\n", TI, TR, TE);
    print_display(TI, TR, TE);

    if (!use_terminal) {
      write_uart_get(uart_filesystem, GET_KEY_VALUE);
      key_gpio = read_uart(uart_filesystem, GET_KEY_VALUE).int_value;
    }

    write_uart_send(uart_filesystem, value_to_send);
  } while (key_gpio == key);
  printf("----------------------------------------------------------\n");
}

void init() {
  wiringPiSetup();
  turn_resistance_off();
  turn_fan_off();
  connect_display();
  bme_connection = connect_bme();
  uart_filesystem = connect_uart();
  system("clear");
}

void menu() {
  int option, key;
  printf("Qual método utilizará para controlar o programa?\n\t1) Terminal\n\t2) Chave\n");
  scanf("%d", &option);
  switch (option) {
    case 1:
      system("clear");
      printf("Selecione uma rotina:\n\t1) On/Off\n\t2) PID\n");
      scanf("%d", &key);
      key--;
      use_terminal = true;
      break;
    case 2:
      getchar();
      system("clear");
      write_uart_get(uart_filesystem, GET_KEY_VALUE);
      key = read_uart(uart_filesystem, GET_KEY_VALUE).int_value;
      printf("O valor inicial da chave é: %d\n", key);
      printf("\nQuando a chave == 0:\n");
      printf("\tIniciar rotina ON/OFF\n");
      printf("\nQuando a chave == 1:\n");
      printf("\tIniciar rotina PID\n");
      printf("\nPrecione Enter para continuar\n");
      char enter = 0;
      while (enter != '\r' && enter != '\n') {
        enter = getchar();
      }
      break;
    default:
      system("clear");
      printf("Opção invalida\n");
      menu();
      break;
  }
    pid_routine(key);
}

int main() {
  init();
  signal(SIGINT, shutdown_program);
  signal(SIGQUIT, toggle_routine);
  printf(
      "Lucas Ganda Carvalho - 17/0039668\n"
      "Trabalho 1 de FSE\n");

  menu();
  return 0;
}
