#include <stdbool.h>
#include "microsd.h"
#include "hal.h"
#include "ff.h"
#include "chprintf.h"

/* ------------------------------------------------------------------------- */

static bool microsd_card_init(FATFS* fs);
static void microsd_card_try_init(FATFS* fs);
static void microsd_card_deinit(void);

/* ------------------------------------------------------------------------- */

static bool microsd_card_init(FATFS* fs)
{
    FRESULT err;

    /* Initialise the SDC interface */
    sdcStart(&SDCD1, NULL);

    /* Attempt to connect to the SD card */
    int i = sdcConnect(&SDCD1);
    if (i) {
        return false;
    }

    /* Attempt to mount the filesystem */
    err = f_mount(0, fs);
    return err == FR_OK;
}

static void microsd_card_try_init(FATFS* fs)
{
    while(!microsd_card_init(fs)) {
        /* TODO: report sadness up the chain */
        microsd_card_deinit();
        chThdSleepMilliseconds(200); // removed: led flash
    }
}

static void microsd_card_deinit(void)
{
    /* Unmount FS */
    f_mount(0, NULL);

    /* Disconnect from card */
    sdcDisconnect(&SDCD1);

    /* Disable SDC peripheral */
    sdcStop(&SDCD1);
}

/* ------------------------------------------------------------------------- */

/* open file in <path> to <fp>.
 * mounts the file system and everything; blocking (see try_init function).
 * NOTE!!! Assumes only one file opened at a time - might want to check if the
 * sd card/file system is already connected/mounted when opening.
 */
SDRESULT microsd_open_file(SDFILE* fp, const char* path, SDMODE mode,
    SDFS* sd)
{
    SDRESULT retval;
    microsd_card_try_init((FATFS*)sd);
    retval = f_open((FIL*)fp, (TCHAR*)path, (BYTE)mode);
    return retval;
}

/* Open/create file using incremental naming scheme that follows the format
 * <filename>_<5-digit number>.<extension>.
 * E.g. if log_00001.bin exists, try log_00002.bin until we find one that
 * doesn't already exist or we reach the limit of 99999.
 */
SDRESULT microsd_open_file_inc(FIL* fp, const char* path, const char* ext,
    SDFS* sd)
{
    uint32_t file_idx = 0;
    char fname[25];
    FRESULT err;
    BYTE mode = FA_WRITE | FA_CREATE_NEW;

    microsd_card_try_init((FATFS*)sd);

    while (TRUE) {

        // try to open file with number file_idx
        file_idx++;
        chsnprintf(fname, 25, "%s_%05d.%s", path, file_idx, ext);
        err = f_open(fp, fname, mode);

        // if it doesn't exist, success! otherwise try the next, or return error
        if (err == FR_OK) return FR_OK;
        if (file_idx > 99999) return FR_INT_ERR; // internal error
        if (err == FR_EXIST) continue;

        // some unspecified error
        return err;
    }
}

/* close file in <fp>.
 * unmounts the file system and disconnects from the sd as well.
 */
SDRESULT microsd_close_file(SDFILE* fp)
{
    SDRESULT retval;
    retval = f_close(fp);
    microsd_card_deinit();
    return retval;
}

/* Write <btw> bytes from <buf> to <fp>.
 * Number of bytes written is currently not used for anything ...
 */
SDRESULT microsd_write(SDFILE* fp, const void* buf, unsigned int btw)
{
    unsigned int bytes_written;
    FRESULT res = f_write(fp, buf, btw, &bytes_written);
    f_sync(fp); // flush write buffer immediately for safety
    return res;
}

/* Read <btr> bytes from <fp> to <buf>.
 * Number of bytes read is currently not used for anything ...
 */
SDRESULT microsd_read(SDFILE* fp, void* buf, unsigned int btr)
{
    unsigned int bytes_read;
    FRESULT res = f_read(fp, buf, btr, &bytes_read);
    return res;
}


SDRESULT microsd_gets(SDFILE* fp, char* buf, int size)
{
    TCHAR* res = f_gets((TCHAR*)buf, size, fp);
    return res != NULL ? FR_OK : FR_INT_ERR;
    // if res is null then either an error occurred (check with f_error(fp))
    // or it reached end of file (check with f_eof(fp))
}
