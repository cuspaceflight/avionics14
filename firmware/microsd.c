#include "microsd.h"
#include "hal.h"
#include "ff.h"

/* TODO: Connect the sd, mount the file system
 * btw, static is C's answer to the private modifier */
static bool_t microsd_card_init(FATFS* fs) {
    return FALSE;
}

/* TODO: Try initialising the sd until it works.
 * We keep a statically allocated FATFS in here, which is the one that is used
 * for all file system purposes.
 * Something something error handling/timeout
 */
void microsd_card_try_init(void) { static FATFS fs; }

/* TODO: Unmount the file system, disconnect the SD.
 */
void microsd_card_deinit(void) {}

/* TODO: open file in <path> to <fp>.
 * Probably need to find a way to make this generic enough to be useful for
 * either reading or writing, but not support as many modes/error states as
 * the FatFS one does.
 */
SDRESULT microsd_open_file(FIL* fp, const char* path, BYTE mode) {
    // convert char* path to TCHAR* path (should be the same by default)
    return 1;
}

/* TODO: close file in <fp>.
 */
SDRESULT microsd_close_file(FIL* fp) {
    return false;
}

/* TODO: Write <btw> bytes from <buf> to <fp>.
 */
SDRESULT microsd_write(FIL* fp, const void* buf, unsigned int btw) {
    // convert unsigned int to UINT
    return FALSE;
}

/* TODO: Read <btr> bytes from <fp> to <buf>.
 */
SDRESULT microsd_read(FIL* fp, void* buf, unsigned int btr) {
    // convert unsigned int to UINT
    return FALSE;
}
