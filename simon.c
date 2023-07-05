#include "simon.h"
#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include <stdio.h>
#define DEBOUNCE_TIME_MS 200 // Set the debounce time to 50 ms
#define BUTTON_COUNT 4

volatile uint status = USER_WAIT;

// debouncer variables
static const int DEBOUNCE = 500;
static bool pressed = false;
static int32_t alarm_id = 0;
bool enable = true;

// // user
// uint user_record[MAX_STEPS];
// uint user_idx = 0;
// uint user_steps = 0;
// uint score = 0;

// // computer
// uint steps = 1;
// uint record[MAX_STEPS];

int64_t enable_button(alarm_id_t alarm_id, void *user_data) {
  pressed = false;
  return 0;
}

void button_callback(uint gpio, uint32_t events) {

  if (pressed) {
    cancel_alarm(alarm_id);
  }

  else {

    pressed = true;

    if (gpio == BUTTON) { // if button 1 is pressed then change the counter type
      gpio_put(BLUE, ON);
      status = BLUE_PRESSED;
    }

    else if (gpio == BUTTON2) { // if button 2 is pressed then increase freq
      gpio_put(GREEN, ON);
      status = GREEN_PRESSED;
    }

    else if (gpio == BUTTON3) {
      gpio_put(YELLOW, ON);
      status = YELLOW_PRESSED;
    }

    else if (gpio == BUTTON4) {
      gpio_put(RED, ON);
      status = RED_PRESSED;
    }

    else if (gpio == BUTTON5) {
      status = RESET_GAME;
    }
  }

  alarm_id = add_alarm_in_ms(DEBOUNCE, enable_button, NULL, false);
}

void gpio_init_all(gpio_config_t *gpio_configs, size_t num_configs) {

  for (int i = 0; i < num_configs; i++) {
    gpio_init(gpio_configs[i].pin_num);

    if (gpio_configs[i].pin_mode) {
      gpio_set_dir(gpio_configs[i].pin_num, true);
    } else {
      gpio_set_dir(gpio_configs[i].pin_num, false);
    }

    if (!gpio_configs[i].pin_mode) {
      if (gpio_configs[i].pin_pullup) {
        gpio_pull_up(gpio_configs[i].pin_num);
      } else {
        gpio_pull_down(gpio_configs[i].pin_num);
      }
    }

    if (gpio_configs[i].pin_interrupt) {
      if (i == 0) {
        gpio_set_irq_enabled_with_callback(gpio_configs[i].pin_num,
                                           GPIO_IRQ_LEVEL_HIGH, true,
                                           button_callback);
      } else {
        gpio_set_irq_enabled(gpio_configs[i].pin_num, GPIO_IRQ_LEVEL_HIGH,
                             true);
      }
    }
  }
}

void pattern_gen(int *steps, int *record) {
  int pin_num[] = {RED, GREEN, YELLOW, BLUE};
  int idx = 0;

  for (int i = 0; i < *steps; i++) {
    idx = rand() % 4;

    gpio_put(RED, OFF);
    gpio_put(YELLOW, OFF);
    gpio_put(GREEN, OFF);
    gpio_put(BLUE, OFF);

    sleep_ms(100);

    gpio_put(pin_num[idx], ON); // turn on LED
    record[i] = pin_num[idx];   // store LED sequence in an array

    sleep_ms(1000);
  }

  gpio_put(RED, OFF);
  gpio_put(YELLOW, OFF);
  gpio_put(GREEN, OFF);
  gpio_put(BLUE, OFF);
}

void game(int *user_record, int *user_idx, int *user_steps, int *steps,
          int *record, bool *enable, int *score, int *high_score) {
  clear();

  printf("SCORE: %d\r\n", *score);
  printf("HIGHSCORE: %d\r\n", *high_score);

  if (*score == MAX_STEPS) {
    printf("Congrats you have won!\r\n");
    sleep_ms(500);
    *user_idx = 0;
    *user_steps = 0;
    *score = 0;
    *steps = 1;
    *enable = true;
    *high_score = MAX_STEPS;
  }

  if (*enable) { // generate pattern
    pattern_gen(steps, record);
    *enable = false;
  }

  else { // user turn
    gpio_put(RED, 0);
    gpio_put(GREEN, 0);
    gpio_put(YELLOW, 0);
    gpio_put(BLUE, 0);

    if (*user_steps == *steps) { // compare

      if (compare_arrays(record, user_record, *steps)) {
        ++(*score);
        ++(*steps);
      } else {
        if (*score > *high_score) {
          *high_score = *score;
        }
        *score = 0;
        *steps = 1;
        printf("WRONG!\r\n");
        sleep_ms(1000);
      }
      *enable = true;
      sleep_ms(300);
      *user_idx = 0;
      *user_steps = 0;
    }

    else {
      if (status == BLUE_PRESSED) {
        if (*user_idx > MAX_STEPS - 1) {
          *user_idx = 0;
        }
        user_record[(*user_idx)++] = BLUE;
        ++(*user_steps);
        printf("PRESSED: %d \r\n", *user_steps);
        status = 6;
      }

      else if (status == GREEN_PRESSED) {
        if (*user_idx > MAX_STEPS - 1) {
          *user_idx = 0;
        }
        user_record[(*user_idx)++] = GREEN;
        ++(*user_steps);
        printf("PRESSED: %d \r\n", *user_steps);
        status = 6;
      }

      else if (status == RED_PRESSED) {
        if (*user_idx > MAX_STEPS - 1) {
          *user_idx = 0;
        }
        user_record[(*user_idx)++] = RED;
        ++(*user_steps);
        printf("PRESSED: %d \r\n", *user_steps);
        status = 6;
      }

      else if (status == YELLOW_PRESSED) {
        if (*user_idx > MAX_STEPS - 1) {
          *user_idx = 0;
        }
        user_record[(*user_idx)++] = YELLOW;
        ++(*user_steps);
        printf("PRESSED: %d \r\n", *user_steps);
        status = 6;
      }

      else if (status == RESET_GAME) {
        *user_idx = 0;
        *user_steps = 0;
        *score = 0;
        *steps = 1;
        *enable = true;
        *high_score = 0;

        printf("GAME RESET!\r\n");
        sleep_ms(500);
        clear();

        status = 6;
      }

      else {
        printf("USER TURN!\r\n");
        sleep_ms(500);
        clear();
      }
    }
  }
}

bool compare_arrays(int *arr1, int *arr2, int len) {
  for (int i = 0; i < len; i++) {
    if (arr1[i] != arr2[i]) {
      return false;
    }
  }
  return true;
}
