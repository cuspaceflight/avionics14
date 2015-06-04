/* Exposes functions for datalogging.
 */

/* ------------------------------------------------------------------------- */

/* channel identifiers */
#define CHAN_INIT       0x00
#define CHAN_CAL_TFREQ  0x10
#define CHAN_CAL_LGA    0x11
#define CHAN_CAL_HGA    0x12
#define CHAN_CAL_BARO1  0x13
#define CHAN_CAL_BARO2  0x14
#define CHAN_IMU_LGA    0x20
#define CHAN_IMU_HGA    0x21
#define CHAN_IMU_BARO   0x22
#define CHAN_SENS_BAT   0x30
#define CHAN_SENS_SG    0x31
#define CHAN_SENS_TC    0x32
#define CHAN_SM_MISSION 0x40
#define CHAN_SE_P1      0x50
#define CHAN_SE_P2      0x51
#define CHAN_SE_U_P     0x52
#define CHAN_SE_U_A     0x53
#define CHAN_PYRO_C     0x60
#define CHAN_PYRO_F     0x61
#define CHAN_GPS_TIME   0x70
#define CHAN_GPS_POS    0x71
#define CHAN_GPS_ALT    0x72
#define CHAN_GPS_STATUS 0x73

/* max log counters before we draw a sample for radio transmission.
 * 0 means that it is not sampled for radio.
 * 1 means that it is sampled every time data is logged.
 * 2 means every second time, etc.
 */
#define LOG_INIT           1
#define LOG_CAL_TFREQ      1
#define LOG_CAL_LGA        1
#define LOG_CAL_HGA        1
#define LOG_CAL_BARO1      1
#define LOG_CAL_BARO2      1
#define LOG_IMU_LGA        400
#define LOG_IMU_HGA        400
#define LOG_IMU_BARO       100
#define LOG_SENS_BAT       0
#define LOG_SENS_SG        0
#define LOG_SENS_TC        0
#define LOG_SM_MISSION     1
#define LOG_SE_P1          200
#define LOG_SE_P2          200
#define LOG_SE_U_P         200
#define LOG_SE_U_A         200
#define LOG_PYRO_C         1
#define LOG_PYRO_F         1
#define LOG_GPS_TIME       1
#define LOG_GPS_POS        1
#define LOG_GPS_ALT        1
#define LOG_GPS_STATUS     1

/* logging codes to determine format of the data payload.
 * (not relevant for calling a logging function, this is just to make the
 * information public)
 */
#define L_CHAR    0
#define L_SINT64  1
#define L_UINT64  2
#define L_SINT32  3
#define L_UINT32  4
#define L_SINT16  5
#define L_UINT16  6
#define L_SINT8   7
#define L_UINT8   8
#define L_FLOAT   9
#define L_DOUBLE 10
#define L_ERROR  11

/* ------------------------------------------------------------------------- */

/* main datalogging thread that handles writing the data persistently.
 * it periodically fetches the data that is being logged by the below functions
 * and saves them to the microsd card.
 */
msg_t microsd_thread(void* arg);

/* log 8 characters */
void log_c(uint8_t channel, const char* data);

/* log one signed 64-bit integer */
void log_s64(uint8_t channel, int64_t data);

/* log one unsigned 64-bit integer */
void log_uint64(uint8_t channel, uint64_t data);

/* log two signed 32-bit integers */
void log_s32(uint8_t channel, int32_t data_a, int32_t data_b);

/* log two unsigned 32-bit integers */
void log_uint32(uint8_t channel, uint32_t data_a, uint32_t data_b);

/* log four signed 16-bit integers */
void log_s16(uint8_t channel, int16_t data_a, int16_t data_b,
    int16_t data_c, int16_t data_d);

/* log four unsigned 16-bit integers */
void log_u16(uint8_t channel, uint16_t data_a, uint16_t data_b,
    uint16_t data_c, uint16_t data_d);

/* log eight signed 8-bit integers */
void log_sint8(uint8_t channel,
    int8_t data_a, int8_t data_b, int8_t data_c, int8_t data_d,
    int8_t data_e, int8_t data_f, int8_t data_g, int8_t data_h);

/* log eight unsigned 8-bit integers */
void log_uint8(uint8_t channel,
    uint8_t data_a, uint8_t data_b, uint8_t data_c, uint8_t data_d,
    uint8_t data_e, uint8_t data_f, uint8_t data_g, uint8_t data_h);

/* log two 32-bit single precision floats */
void log_f(uint8_t channel, float data_a, float data_b);

/* log one 64-bit double precision float */
void log_double(uint8_t channel, double data);

/* error logging - is that a thing? */
void log_err(uint8_t channel, const char* data);
