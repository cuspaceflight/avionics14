#include "microsd.h"
#include "hal.h"
#include "ff.h"

/* TODO: Connect the sd, mount the file system
 * btw, static is C's answer to the private modifier */
static bool_t microsd_card_init(FATFS* fs) {
    return FALSE;
}

/* TODO: Try initialising the sd until it works.
 * Something something error handling/timeout
 */
void microsd_card_try_init(FATFS* fs) {}

/* TODO: Unmount the file system, disconnect the SD.
 */
void microsd_card_deinit(void) {}

/* TODO: open file in <path> to <fp>.
 * Probably need to find a way to make this generic enough to be useful for
 * either reading or writing, but not support as many modes/error states as
 * the FatFS one does.
 */
FRESULT microsd_open_file(FIL* fp, const TCHAR* path, BYTE mode) {
    return 1;
}

/* TODO: close file in <fp>.
 */
bool_t microsd_close_file(FIL* fp) {
    return false;
}

/* TODO: Write <btw> bytes from <buf> to <fp>.
 */
bool_t microsd_write(FIL* fp, const void* buf, UINT btw) {
    return FALSE;
}

/* TODO: Read <btr> bytes from <fp> to <buf>.
 */
bool_t microsd_read(FIL* fp, void* buf, UINT btr) {
    return FALSE;
}
