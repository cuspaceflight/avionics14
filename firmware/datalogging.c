#include <stdint.h>
#include "microsd.h"
#include "datalogging.h"

static void mem_init(void);
static bool_t open_file(FIL* fp);

/* 1st TODO:
 * We need to define our protocol here.
 * The packet format? Needs to include at least the channel, timestamp, data.
 * The packet size? 16 bytes seems neat.
 *
 * 2nd TODO:
 * Figure out how to do it. I vote for totally plagiarising m2:
 * 1) When datalogging functions are called, we need to store the data
 *    somewhere in memory so we don't accidentally lose it in case we can't
 *    write it fast enough. MemoryPools seem most reliable for memory
 *    management according to the ChibiOS docs.
 * 2) To allow ease of retrieval, use a queue. The queue also has to support
 *    mutual exclusion (because all kinds of different channels might try to
 *    log at the same time) and condition synchronisation (so nothing dies if
 *    we try to retrieve data from an empty queue). m2 uses ChibiOS Mailbox
 *    which conveniently handles all these things.
 * 3) We want to write larger blocks of data to the sd card at a time, because
 *    it is faster. A way to do that would be using a write buffer (16kb cache
 *    in m2).
 * 4) Suppose we now have a pointer to a buffer filled with loggable data.
 *    Use the functions in microsd.
 *      a) We need to INITIALISE the sd card through the driver
 *      b) then OPEN a file according to some naming scheme (log<number>)
 *      c) then WRITE to the file through the driver.
 */

static void mem_init(void) {}

/* TODO: looks for/creates suitable log file to write to.
 * Create new log file for each write? So we don't get one superlong file.
 */
static bool_t open_file(FIL* fp) {}


/*** TOOODOOO: LOGGING FUNCTIONS TO BE USED BY SENSORS ***/
void log_c(uint8_t channel, const char* data) {}

void log_s64(uint8_t channel, int64_t data) {}

void log_s32(uint8_t channel, int32_t data_a, int32_t data_b) {}

void log_s16(uint8_t channel, int16_t data_a, int16_t data_b,
int16_t data_c, int16_t data_d) {}

void log_u16(uint8_t channel, uint16_t data_a, uint16_t data_b,
uint16_t data_c, uint16_t data_d) {}

void log_f(uint8_t channel, float data_a, float data_b) {}

/* the function that makes it all happen
 */
msg_t datalog_thread(void* arg) {}
