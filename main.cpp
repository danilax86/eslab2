#include "hal.h"
#include <bitset>

int step_counter = 0;

unsigned int leds_num[] = {GPIO_PIN_3,
                           GPIO_PIN_4,
                           GPIO_PIN_5,
                           GPIO_PIN_6,
                           GPIO_PIN_8,
                           GPIO_PIN_9,
                           GPIO_PIN_11,
                           GPIO_PIN_12};

unsigned int sw_num[4] = {GPIO_PIN_12,
                          GPIO_PIN_10,
                          GPIO_PIN_8,
                          GPIO_PIN_4};

int steps[12][8] = {
        {0, 0, 0, 0, 0, 0, 1, 1},
        {0, 0, 0, 0, 0, 1, 1, 0},
        {0, 0, 0, 0, 1, 1, 0, 0},
        {0, 0, 0, 1, 1, 0, 0, 0},
        {0, 0, 1, 1, 0, 0, 0, 0},
        {0, 1, 1, 0, 0, 0, 0, 0},
        {1, 1, 0, 0, 0, 0, 0, 0},
        {0, 1, 1, 0, 0, 0, 0, 0},
        {0, 0, 1, 1, 0, 0, 0, 0},
        {0, 0, 0, 1, 1, 0, 0, 0},
        {0, 0, 0, 0, 1, 1, 0, 0},
        {0, 0, 0, 0, 0, 1, 1, 0}
};

std::bitset<4> sw;


void set_speed() {
    for (int i = 0; i < 4; i++) {
        GPIO_PinState state = HAL_GPIO_ReadPin(GPIOE, sw_num[i]);
        sw[i] = state == GPIO_PIN_SET;
    }
}

void set_leds(const int *num) {
    for (int i = 0; i < 8; i++) {
        if (num[i] == 1) {
            HAL_GPIO_WritePin(GPIOD, leds_num[i], GPIO_PIN_SET);
        }
    }
}

void unset_leds(const int *num) {
    for (int i = 0; i < 8; i++) {
        if (num[i] == 1) {
            HAL_GPIO_WritePin(GPIOD, leds_num[i], GPIO_PIN_RESET);
        }
    }
}

void TIM6_IRQ_Handler() {
    set_leds(steps[step_counter]);
}

void TIM7_IRQ_Handler() {
    unset_leds(steps[step_counter]);
    step_counter++;
    if (step_counter == 13) step_counter = 0;
    set_speed();
    WRITE_REG(TIM6_ARR, 500 + sw.to_ulong() * 150);
    WRITE_REG(TIM7_ARR, 500 + sw.to_ulong() * 150);
}

int umain() {
    step_counter = 0;

    registerTIM6_IRQHandler(TIM6_IRQ_Handler);
    registerTIM7_IRQHandler(TIM7_IRQ_Handler);

    __enable_irq();

    WRITE_REG(TIM6_ARR, 500);
    WRITE_REG(TIM6_DIER, TIM_DIER_UIE);
    WRITE_REG(TIM6_PSC, 0);

    WRITE_REG(TIM7_ARR, 500);
    WRITE_REG(TIM7_DIER, TIM_DIER_UIE);
    WRITE_REG(TIM7_PSC, 1);

    WRITE_REG(TIM6_CR1, TIM_CR1_CEN);
    WRITE_REG(TIM7_CR1, TIM_CR1_CEN);

    return 0;
}