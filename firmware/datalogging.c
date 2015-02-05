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
 * m2 format for ref (the nothing is presumably padding to get 16 bytes):
 * | Timestamp(4) | Stage + logtype(1) | Channel(1) | Nothing??(2) | Data(8) |
 *
 * If pressed, we could maybe squeeze stage+logtype+channel into one byte, but
 * if we want nicely aligned packets and min. 8 bytes of data, we can't go
 * lower than 16 anyway.
 * If we're going to log matrices, we can use the 2 unused bytes to keep track,
 * but it'll not be very fun to unparse ...
 *
 *
 * 2nd TODO:
 * Figure out how to do it. I vote for totally plagiarising m2:
 * 0) Initialise stuff: sd card connect, mount file system, open file, (init
 *    memory pool), (init mailbox), stuff like that.
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
 *    in m2). Note that FatFS most probably likely definitely uses a write
 *    buffer to do its writes, the reason to implement it ourselves would be to
 *    flush the write buffer immediately after writing to ensure minimal loss
 *    in the event of a crash.
 * 4) Suppose we now have a pointer to a buffer filled with loggable data.
 *    Use the write function in microsd.c to write to the driver. (what to do
 *    if it fails?) REPEAT AD LIBITUM (or until the thread dies)
 * n) Unmount, deinitialise, etc. although it'll probably happen automatically
 *    since the controller closes down at this point.
 */

/* Initialise memory management structures used to keep the data temporarily
 * in memory.
 */
static void mem_init(void) {}

/* TODO: looks for/creates suitable log file to write to.
 * Is there any instability associated with superlong files?
 */
static bool_t open_file(FIL* fp) {}


/*** TOOODOOO: LOGGING FUNCTIONS TO BE USED BY SENSORS ***/
/* TODO: discuss what sort of data we can accept. Matrices??? */

void log_c(uint8_t channel, const char* data) {}

void log_s64(uint8_t channel, int64_t data) {}

void log_s32(uint8_t channel, int32_t data_a, int32_t data_b) {}

void log_s16(uint8_t channel, int16_t data_a, int16_t data_b,
int16_t data_c, int16_t data_d) {}

void log_u16(uint8_t channel, uint16_t data_a, uint16_t data_b,
uint16_t data_c, uint16_t data_d) {}

void log_f(uint8_t channel, float data_a, float data_b) {}

void log_err(uint8_t channel, const char* data) {}

/* the function that makes it all happen
 */
msg_t datalogging_thread(void* arg) {}
