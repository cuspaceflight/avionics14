/* Exposes functions for datalogging
 * For now, function signatures are shamelessly stolen from m2-electronics.
 * TODO: define channels
 */

void log_c(uint8_t channel, const char* data);

void log_s64(uint8_t channel, int64_t data);

void log_s32(uint8_t channel, int32_t data_a, int32_t data_b);

void log_s16(uint8_t channel, int16_t data_a, int16_t data_b,
int16_t data_c, int16_t data_d);

void log_u16(uint8_t channel, uint16_t data_a, uint16_t data_b,
uint16_t data_c, uint16_t data_d);

void log_f(uint8_t channel, float data_a, float data_b);

/* error logging - is that a thing? */
void log_err(uint8_t channel, const char* data);

/* micro sd logging thread */
msg_t datalogging_thread(void* arg);
