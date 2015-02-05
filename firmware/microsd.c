#include <stdbool.h>
#include "microsd.h"
#include "hal.h"
#include "ff.h"

/* TODO: Connect the sd, mount the file system
 */
static bool microsd_card_init(FATFS* fs)
{
    // connect the sd

    // mount the file system
    FRESULT res = f_mount(0, fs);

    return false;
}

/* TODO: Try initialising the sd until it works.
 * We keep a statically allocated FATFS in here, which is the one that is used
 * for all file system purposes.
 * Something something error handling/timeout
 */
void microsd_card_try_init(void)
{
    static FATFS fs;
    while (!microsd_card_init(fs)) {
        // do something
    };
}

/* TODO: Unmount the file system, disconnect the SD.
 */
void microsd_card_deinit(void)
{
    // unmount file system
    FRESULT res = f_mount(0, NULL);

    // disconnect sd
}


/* open file in <path> to <fp>.
 */
SDRESULT microsd_open_file(SDFILE* fp, const char* path, SDMODE mode)
{
    // default open mode is read only
    BYTE bmode =
        mode == SD_WRITE ? FR_WRITE :
        mode == SD_CREATE_NEW ? FR_CREATE_NEW :
        FR_READ;

    FRESULT res = f_open(fp, (TCHAR*)path, bmode);
    return res == FR_OK      ? SD_OK :
           res == FR_NO_FILE ? SD_NO_FILE :
           res == FR_EXIST   ? SD_EXIST :
                               SD_ERR;
}

/* close file in <fp>.
 */
SDRESULT microsd_close_file(SDFILE* fp)
{
    return f_close(fp) == FR_OK ? SD_OK : SD_ERR;
}

/* Write <btw> bytes from <buf> to <fp>.
 * Number of bytes written is currently not used for anything ...
 */
SDRESULT microsd_write(SDFILE* fp, const void* buf, unsigned int btw)
{
    unsigned int bytes_written;
    FRESULT res = f_write(fp, buf, btw, &bytes_written);
    return res == FR_OK ? SD_OK : SD_ERR;
}

/* Read <btr> bytes from <fp> to <buf>.
 * Number of bytes read is currently not used for anything ...
 */
SDRESULT microsd_read(SDFILE* fp, void* buf, unsigned int btr)
{
    unsigned int bytes_read;
    FRESULT res = f_read(fp, buf, btr, &bytes_read);
    return res == FR_OK ? SD_OK : SD_ERR;
}


SDRESULT microsd_gets(SDFILE* fp, char* buf, int size)
{
    TCHAR* res = f_gets((TCHAR*)buf, size, fp);
    return res != NULL ? SD_OK :
           f_eof(fp)   ? SD_EOF :
                         SD_ERR;
}
