#ifndef MICROSD_H
#define MICROSD_H
#include "ff.h"

/* This is a non-thread-safe "device driver" for the micro sd card and file
 * system, pretty much acts as a file system access layer.
 *
 * Note: Assumes _LFN_UNICODE == 0 in FatFS config (ie. sizeof(TCHAR) == 1).
 */

/* ------------------------------------------------------------------------- */

/* SDMODE takes the value of BYTE, which are the possible file opening modes.
 * FA_READ for read access, FA_WRITE for write access, etc.
 * See FatFS documentation for all possible modes, or look in
 * ChibiOS-RT/ext/fatfs/src/ff.h.
 */
typedef BYTE SDMODE;

/* SDRESULT takes the values of FRESULT, which are return codes for file system
 * interaction. Will return FR_OK if successful. For errors local to this
 * (microsd) code, we use FR_INT_ERR by default.
 * See FatFS documentation for all possible return codes, or look in
 * ChibiOS-RT/ext/fatfs/src/ff.h.
 */
typedef FRESULT SDRESULT;

/* File object */
typedef FIL SDFILE;

/* File system object */
typedef FATFS SDFS;

/* ------------------------------------------------------------------------- */

/* File system/SD card functions */

/* Open file in path <path> and opening mode <mode> to file object <fp>.
 * Blocking operation. Re-attempts indefinitely upon failure.
 * TODO: possibly implement timeout since there is no preemption
 * TODO: fix->do not open multiple files at once ... bad things might happen.
 */
SDRESULT microsd_open_file(SDFILE* fp, const char* path, SDMODE mode,
    SDFS* sd);

/* Open file under incremental naming scheme.
 */
SDRESULT microsd_open_file_inc(SDFILE* fp, const char* path, const char* ext,
    SDFS* sd);

/* Close file object <fp>. */
SDRESULT microsd_close_file(SDFILE* fp);

/* Assumes file is open.
 * Writes at most <btw> bytes from <buff> to <fp>.
 */
SDRESULT microsd_write(SDFILE* fp, const void* buff, unsigned int btw);

/* Assumes file is open.
 * Reads at most <btr> bytes from <fp> to <buf>
 */
SDRESULT microsd_read(SDFILE* fp, void* buf, unsigned int btr);

/* Assumes file is open.
 * Reads at most <size> chars from <fp> to <buf>, stopping at newlines/EOF.
 * Returns FR_OK if succesfully read, returns something else if error/EOF.
 */
SDRESULT microsd_gets(SDFILE* fp, char* buf, int size);

#endif
