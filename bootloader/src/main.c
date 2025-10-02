#include "stm32f4xx.h"

typedef void (*pFunction)(void);

#define SLOT_A_BASE 0x08010000U
#define SLOT_B_BASE 0x08070000U
#define METADATA_BASE 0x080D0000U

#define BOOT_MAGIC 0xB00710AD

typedef struct {
    uint32_t magic;
    uint32_t active_slot; // 0 = A, 1 = B
    uint32_t next_slot;
    uint32_t version_a;
    uint32_t version_b;
} bootctrl_t;

static int image_valid(uint32_t base) {
    uint32_t sp = *(uint32_t*)base;
    uint32_t reset = *(uint32_t*)(base + 4);
    if (sp < 0x20000000 || sp > 0x20020000) return 0;
    if ((reset & 1U) == 0) return 0;
    return 1;
}

static void jump_to_app(uint32_t base) {
    uint32_t sp = *(uint32_t*)base;
    uint32_t reset = *(uint32_t*)(base + 4);

    __disable_irq();
    SysTick->CTRL = 0;
    SCB->VTOR = base;    // set vector table offset
    __set_MSP(sp);
    ((pFunction)reset)();
}

int main(void) {
    // Optionally init clock
    SystemInit();

    const bootctrl_t *ctrl = (const bootctrl_t*) METADATA_BASE;
    uint32_t desired = 0; // default Slot A

    if (ctrl->magic == BOOT_MAGIC) {
        desired = ctrl->next_slot;
    }

    uint32_t base = (desired == 0) ? SLOT_A_BASE : SLOT_B_BASE;

    if (image_valid(base)) {
        jump_to_app(base);
    }

    // fallback
    base = (desired == 0) ? SLOT_B_BASE : SLOT_A_BASE;
    if (image_valid(base)) {
        jump_to_app(base);
    }

    // no valid app → blink LED
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
    GPIOC->MODER |= (1 << (13*2));

    while (1) {
        GPIOC->ODR ^= (1 << 13);
        for (volatile int i = 0; i < 1000000; i++);
    }
}
