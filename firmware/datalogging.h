/* Exposes functions for datalogging
 * For now, function signatures are shamelessly stolen from m2-electronics.
 * TODO: define channels
 */

void microsd_log_c(uint8_t channel, const char* data);

void microsd_log_s64(uint8_t channel, int64_t data);

void microsd_log_s32(uint8_t channel, int32_t data_a, int32_t data_b);

void microsd_log_s16(uint8_t channel, int16_t data_a, int16_t data_b,
int16_t data_c, int16_t data_d);

void microsd_log_u16(uint8_t channel, uint16_t data_a, uint16_t data_b,
uint16_t data_c, uint16_t data_d);

void microsd_log_f(uint8_t channel, float data_a, float data_b);

/* micro sd logging thread */
msg_t microsd_thread(void* arg);
