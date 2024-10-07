#ifndef USR_MAIN_H_INCLUDED
#define USR_MAIN_H_INCLUDED

#include "stm32h7xx_hal.h"


#ifdef __cplusplus
extern "C" {
#endif

void read_limits();
void nxt_pulse();
void uart_transmit(uint8_t *data, uint16_t len);
void parse_messages();
void do_command(uint8_t id, uint16_t command_signature, float value);
void set_limit_type(uint8_t id, uint8_t limit_type);
void clone_axis();
void control_loop();
void save_state_to_backup_ram();
void load_state_from_backup_ram();

int usr_main();

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
