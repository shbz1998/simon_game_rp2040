#include "simon.h"
#include <stdio.h>

int main() {

  stdio_init_all();

  // srand(time(NULL));

  gpio_config_t gpio_configs[] = {

      {BUTTON, INPUT, false, true},   {BUTTON2, INPUT, false, true},
      {BUTTON3, INPUT, false, true},  {BUTTON4, INPUT, false, true},
      {BUTTON5, INPUT, false, true},  {BUTTON6, INPUT, false, true},
      {RED, OUTPUT, false, false},    {GREEN, OUTPUT, false, false},
      {YELLOW, OUTPUT, false, false}, {BLUE, OUTPUT, false, false}};

  gpio_init_all(gpio_configs, sizeof(gpio_configs) / sizeof(gpio_configs[0]));

  int user_record[MAX_STEPS];
  int user_idx = 0;
  int user_steps = 0;
  int score = 0;
  int high_score = 0;

  // computer
  int steps = 1;
  int record[MAX_STEPS];
  bool enable = true;

  while (1) {
    tight_loop_contents();

    sleep_ms(50);

    game(user_record, &user_idx, &user_steps, &steps, record, &enable, &score,
         &high_score);
  }

  return 0;
}