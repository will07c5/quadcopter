#ifndef COMMAND_H_
#define COMMAND_H_

#include <stdint.h>

typedef void (*cmd_func_t)(const uint8_t*, uint8_t);

extern void command_btconfig(void);

extern void command_init(void);
extern void command_process(void);
extern void command_register(unsigned char cmd, cmd_func_t func);
extern void command_send(unsigned char cmd, const uint8_t *data, unsigned int length);
extern int command_connected(void);

#endif /* COMMAND_H_ */
