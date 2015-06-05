#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "hal.h"
#include "microsd.h"
#include "datalogging.h"
#include "config.h"
#include "chprintf.h"
#include "rfm69.h"

/* ------------------------------------------------------------------------- */

#define LOG_MEMPOOL_ITEMS 1024  // 1K
#define LOG_CACHE_SIZE 16384    // 16KB
#define CHANNEL_NUM 256         // 1-byte channel number -> 256 channel numbers

/* 8 bytes of data to be logged */
typedef union data_ {
    char       c[8];
    int8_t    s8[8];
    uint8_t   u8[8];
    int16_t  s16[4];
    uint16_t u16[4];
    int32_t  s32[2];
    uint32_t u32[2];
    float      f[2];
    int64_t  s64[1];
    uint64_t u64[1];
    double     d[1];
} data_t;

/* 16 byte packet: 8 bytes of packet metadata + 8 bytes of logging data */
typedef struct packet_ {
    uint32_t timestamp;
    uint8_t       type;
    uint8_t    channel;
    uint16_t  checksum;
    data_t        data;
} packet_t;

static void mem_init(void);
static uint16_t checksum(packet_t packet);
static void _log(uint8_t channel, uint8_t type, data_t data);

/* ------------------------------------------------------------------------- */
/* STATIC VARIABLES */
/* ------------------------------------------------------------------------- */

/* memory pool for allocating space for incoming data to be queued. */
static MemoryPool log_mempool;

/* mailbox (queue) for storing/accessing data asynchronously. Will be storing
 * pointers to data (packets).
 */
static Mailbox log_mailbox;

/* data cache to ensure SD writes are done LOG_CACHE_SIZE bytes at a time. */
static volatile char log_cache[LOG_CACHE_SIZE];

/* statically allocated memory used for the memory pool */
static volatile char mempool_buffer[LOG_MEMPOOL_ITEMS * sizeof(packet_t)];

/* statically allocated memory used for the queue in mailbox */
static volatile msg_t mailbox_buffer[LOG_MEMPOOL_ITEMS];

/* counters indexed by channel number; used to sample data for radio */
static uint16_t counter[CHANNEL_NUM];

/* contains the counter numbers that decide when we sample the data */
static const uint16_t log_counter[CHANNEL_NUM] = {
    [CHAN_INIT] = LOG_INIT,
    [CHAN_CAL_TFREQ] = LOG_CAL_TFREQ,
    [CHAN_CAL_LGA] = LOG_CAL_LGA,
    [CHAN_CAL_HGA] = LOG_CAL_HGA,
    [CHAN_CAL_BARO1] = LOG_CAL_BARO1,
    [CHAN_CAL_BARO2] = LOG_CAL_BARO2,
    [CHAN_IMU_LGA] = LOG_IMU_LGA,
    [CHAN_IMU_HGA] = LOG_IMU_HGA,
    [CHAN_IMU_BARO] = LOG_IMU_BARO,
    [CHAN_SENS_BAT] = LOG_SENS_BAT,
    [CHAN_SENS_SG] = LOG_SENS_SG,
    [CHAN_SENS_TC] = LOG_SENS_TC,
    [CHAN_SM_MISSION] = LOG_SM_MISSION,
    [CHAN_SE_P1] = LOG_SE_P1,
    [CHAN_SE_P2] = LOG_SE_P2,
    [CHAN_SE_U_P] = LOG_SE_U_P,
    [CHAN_SE_U_A] = LOG_SE_U_A,
    [CHAN_PYRO_C] = LOG_PYRO_C,
    [CHAN_PYRO_F] = LOG_PYRO_F,
    [CHAN_GPS_TIME] = LOG_GPS_TIME,
    [CHAN_GPS_POS] = LOG_GPS_POS,
    [CHAN_GPS_ALT] = LOG_GPS_ALT,
    [CHAN_GPS_STATUS] = LOG_GPS_STATUS
};

/* ------------------------------------------------------------------------- */
/* MAIN THREAD FUNCTIONS */
/* ------------------------------------------------------------------------- */

/* Main datalogging thread. Continuously checks for data added through the
 * logging functions, and persists it to an SD card/sends some of it to radio.
 */
msg_t datalogging_thread(void* arg)
{
    static const int packet_size = sizeof(packet_t);
    volatile char* cache_ptr = log_cache; // pointer to keep track of cache

    SDFS file_system;        // struct that encapsulates file system state
    SDFILE file;             // file struct thing
    msg_t mailbox_res;       // mailbox fetch result
    intptr_t data_msg;       // buffer to store the fetched mailbox item
    char* packet_msg;        // data_msg to be logged cast to char*
    SDRESULT write_res;      // result of writing data to file system
    bool cache_not_full;     // true if still space in cache (write when full)
    (void)arg;

    // initialise stuff
    chRegSetThreadName("Datalogging");
    mem_init();
    while (microsd_open_file_inc(&file, "log", "bin", &file_system) != FR_OK) ;

    if (STAGE == 1) {
        log_c(CHAN_INIT, "B3STAGE1");
    } else if (STAGE == 2) {
        log_c(CHAN_INIT, "B3STAGE2");
    }

    while (true) {

        mailbox_res = chMBFetch(&log_mailbox, (msg_t*)&data_msg, TIME_INFINITE);

        // mailbox was reset while waiting/fetch failed ... try again!
        if (mailbox_res != RDY_OK || data_msg == 0) continue;

        // put packet in the static cache and free it from the memory pool
        packet_msg = (char*)data_msg;
        memcpy((void*)cache_ptr, packet_msg, packet_size);
        chPoolFree(&log_mempool, (void*)data_msg);

        // if the cache is full, write it all to the sd card
        cache_not_full = cache_ptr + packet_size < log_cache + LOG_CACHE_SIZE;
        if (cache_not_full) {

            cache_ptr += packet_size;

        } else {

            write_res = microsd_write(&file, (char*)log_cache, LOG_CACHE_SIZE);

            // if the microsd write fails, we just skip this data ...
            if (write_res != FR_OK) {
                microsd_close_file(&file);
                microsd_open_file_inc(&file, "log", "bin", &file_system);
            }

            // reset cache pointer to beginning of cache
            cache_ptr = log_cache;

        }
    }
}

/* Initialise memory management structures used to keep the data temporarily
 * in memory.
 */
static void mem_init(void)
{
    int i;

    // initialise counters to max value to start radio transmission immediately
    for (i = 0; i < CHANNEL_NUM; i++) {
        counter[i] = log_counter[i];
    }

    chMBInit(&log_mailbox, (msg_t*)mailbox_buffer, LOG_MEMPOOL_ITEMS);
    chPoolInit(&log_mempool, sizeof(packet_t), NULL);

    // fill the memory pool with statically allocated bits of memory
    // ie. prevent dynamic core memory allocation (which cannot be freed), we
    // just want the "bookkeeping" that memory pools provide
    chPoolLoadArray(&log_mempool, (void*)mempool_buffer, LOG_MEMPOOL_ITEMS);
}

/* ------------------------------------------------------------------------- */
/* LOGGING FUNCTIONS */
/* ------------------------------------------------------------------------- */

/* log 8 characters. truncates data */
void log_c(uint8_t channel, const char* data)
{
    data_t datatype;
    memcpy((void*)&datatype.c, data, 8);
    _log(channel, (uint8_t)L_CHAR, datatype);
}

/* log one signed 64-bit integer */
void log_s64(uint8_t channel, int64_t data)
{
    data_t datatype = { .s64 = { data } };
    _log(channel, (uint8_t)L_SINT64, datatype);
}

/* log one unsigned 64-bit integer */
void log_u64(uint8_t channel, uint64_t data)
{
    data_t datatype = { .u64 = { data } };
    _log(channel, (uint8_t)L_UINT64, datatype);
}

/* log two signed 32-bit integers */
void log_s32(uint8_t channel, int32_t a, int32_t b)
{
    data_t datatype = { .s32 = { a, b } };
    _log(channel, (uint8_t)L_SINT32, datatype);
}

/* log two unsigned 32-bit integers */
void log_u32(uint8_t channel, uint32_t a, uint32_t b)
{
    data_t datatype = { .u32 = { a, b } };
    _log(channel, (uint8_t)L_UINT32, datatype);
}

/* log four signed 16-bit integers */
void log_s16(uint8_t channel,
    int16_t a, int16_t b, int16_t c, int16_t d)
{
    data_t datatype = { .s16 = { a, b, c, d } };
    _log(channel, (uint8_t)L_SINT16, datatype);
}

/* log four unsigned 16-bit integers */
void log_u16(uint8_t channel,
    uint16_t a, uint16_t b, uint16_t c, uint16_t d)
{
    data_t datatype = { .u16 = { a, b, c, d } };
    _log(channel, (uint8_t)L_UINT16, datatype);
}

/* log eight signed 8-bit integers */
void log_s8(uint8_t channel,
    int8_t a, int8_t b, int8_t c, int8_t d,
    int8_t e, int8_t f, int8_t g, int8_t h)
{
    data_t datatype = { .s8 = { a, b, c, d, e, f, g, h } };
    _log(channel, (uint8_t)L_SINT8, datatype);
}

/* log eight unsigned 8-bit integers */
void log_u8(uint8_t channel,
    uint8_t a, uint8_t b, uint8_t c, uint8_t d,
    uint8_t e, uint8_t f, uint8_t g, uint8_t h)
{
    data_t datatype = { .u8 = { a, b, c, d, e, f, g, h } };
    _log(channel, (uint8_t)L_UINT8, datatype);
}

/* log two 32-bit single precision floats */
void log_f(uint8_t channel, float a, float b)
{
    data_t datatype = { .f = { a, b } };
    _log(channel, (uint8_t)L_FLOAT, datatype);
}

/* log one 64-bit double precision float */
void log_d(uint8_t channel, double data)
{
    data_t datatype = { .d = { data } };
    _log(channel, (uint8_t)L_DOUBLE, datatype);
}

/* log tiny error msg of 8 chars. might make more sense to have error codes. */
void log_err(uint8_t channel, const char* data)
{
    data_t datatype;
    memcpy((void*)&datatype.c, data, 8);
    _log(channel, (uint8_t)L_ERROR, datatype);
}

/* CRC16-CCCITT checksum. 64 bits to 16 bits.
 * Initial value 0x0000, polynomial 0x1021.
 */
static uint16_t checksum(packet_t packet)
{
    uint16_t checksum = 0x0000;
    uint8_t* p = (uint8_t*)&packet;
    int i, j;
    for(i=0; i<16; i++) {
        checksum ^= ((uint16_t)*p << 8);
        for(j=0; j<8; j++) {
            if(checksum & 0x8000) {
                checksum = (checksum << 1) ^ 0x1021;
            } else {
                checksum <<= 1;
            }
        }
    }
    return checksum;
}

/* Allocate and post a formatted packet containing metadata + data to mailbox.
 * Use counter arrays to determine if this data should be sampled to radio.
 * (it's called _log because log conflicts with a library function)
 */
static void _log(uint8_t channel, uint8_t type, data_t data)
{
    void* msg;
    msg_t retval;

    packet_t packet = {
        .timestamp = halGetCounterValue(),
        .type = type | (STAGE + 4) << 4,
        .channel = channel,
        .checksum = 0,
        .data = data
    };

    packet.checksum = checksum(packet);

    // update counter; if reached max value, we sample this data for radio
    // assumption: only one thread per channel (otherwise needs lock)
    if (log_counter[channel] != 0) {

        counter[channel]++;
        if (counter[channel] >= log_counter[channel]) {

            rfm69_log_packet((uint8_t*)&packet);
            counter[channel] = 0;
        }
    }

    // allocate space for the packet and copy it into a mailbox message
    msg = chPoolAlloc(&log_mempool);
    if (msg == NULL) return;
    memcpy(msg, (void*)&packet, sizeof(packet_t));

    // put it in the mailbox buffer
    retval = chMBPost(&log_mailbox, (intptr_t)msg, TIME_IMMEDIATE);
    if (retval != RDY_OK) {
        chPoolFree(&log_mempool, msg);
        return;
    }
}
