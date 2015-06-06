#pragma once
#include <stdint.h>
#define CHAN_SM_MISSION 0x40
void log_s32(uint8_t ch, int32_t cur_state, int32_t new_state);
