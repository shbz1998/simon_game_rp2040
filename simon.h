#ifndef SIMON_H__
#define SIMON_H__

#include "hardware/i2c.h"
#include "hardware/uart.h"
#include "pico/multicore.h"
#include "pico/stdlib.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define OUTPUT true
#define INPUT false
#define OFF 0
#define ON 1
#define MAX_STEPS 10

// clear terminal
#define clear() printf("\033[H\033[J") // clearing the serial terminal

// buttons
#define BUTTON 16
#define BUTTON2 19
#define BUTTON3 17
#define BUTTON4 20
#define BUTTON5 21
#define BUTTON6 22

// leds
#define RED 13
#define GREEN 9
#define YELLOW 10
#define BLUE 18

#define LED_DELAY 150

#define BLUE_PRESSED 0
#define GREEN_PRESSED 1
#define RED_PRESSED 2
#define YELLOW_PRESSED 3
#define RESET_GAME 4
#define USER_WAIT 6

// 13 9 18

typedef struct {
  uint pin_num;
  uint pin_mode;
  bool pin_pullup;
  bool pin_interrupt;
} gpio_config_t;

// functions
void button_callback(uint gpio, uint32_t events);            // ISR
int64_t enable_button(alarm_id_t alarm_id, void *user_data); // DEBOUNCER
void gpio_init_all(gpio_config_t *gpio_configs,
                   size_t num_configs); // INIT GPIO
void pattern_gen(int *steps, int *record);
void game(int *user_record, int *user_idx, int *user_steps, int *steps,
          int *record, bool *enable, int *score, int *high_score);
bool compare_arrays(int *arr1, int *arr2, int len);
void led();
void my_callback(alarm_id_t id, void *user_data);

#endif