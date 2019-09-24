#ifndef INTR_H_
#define INTR_H_

#include "handlers.h"

#include <stdint.h>

#define INTR_GPT3_IRQ 37
#define INTR_GPT4_IRQ 38
#define INTR_GPT9_IRQ 39
#define INTR_GPT11_IRQ 40

void intr_enable(int num);
void intr_disable(int num);
void intr_set_handler(int num, handler_t handler);
void intr_set_priority(int num, uint8_t priority);
void intr_set_systick_priority(uint8_t priority);

/* XXX removed for the moment
 * see comment in intr.c
 */
#if 0
void intr_enable_systick(void);
void intr_disable_systick(void);
#endif

#endif
