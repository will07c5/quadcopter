#ifndef HANDLERS_H_
#define HANDLERS_H_

typedef void (*handler_t)(void);

/* interrupt handlers */
void reset_handler(void);
void nmi_handler(void);
void hard_fault_handler(void);
void mpu_handler(void);
void bus_fault_handler(void);
void usage_fault_handler(void);
void svcall_handler(void);
void debug_handler(void);
void pendsv_handler(void);
void systick_handler(void);
void default_handler(void);

#endif
