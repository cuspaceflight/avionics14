/* This is an abstracted "device driver" for the micro sd card and file system,
 * mostly acting as a file system access layer
 * (based on the ChibiOS SDC driver).
 *
 * It would be nice to have this module be the only dependency on SDC driver
 * and FatFS, ie. full encapsulation. Currently, it is only FIL that requires
 * users of this module to include ff.h, but that should be fixable ...
 */

/* Literally FRESULT, with irrelevant (?) error states grouped into one */
typedef enum {
    SD_OK,
    SD_NO_FILE,
    SD_EXIST,
    SD_ERR
} SDRESULT;

typedef enum {
    SD_READ,
    SD_WRITE,
    SD_CREATE_NEW
} SDMODE;

/* Connect to sd card, mount file system
 * (keeps trying until it succeeds ... implement time out?)
 */
void microsd_card_try_init(void);

/* Unmount file system, disconnect sd, assumes no open files.
 */
void microsd_card_deinit(void);

/* Open file on file system. For now, exact same signature as the FatFS one
 * because we need to be able to specify mode and see error results...
 */
SDRESULT microsd_open_file(FIL* fp, const char* path, SDMODE mode);

/* Closes file on file system. */
SDRESULT microsd_close_file(FIL* fp);

/* Assumes sd is connected and mounted, file is open
 * (possibly needs some indicator that it isn't, ie. in the return value)
 * <fp> is the file to be written to
 * <buff> is the buffer containing the data to be written
 * <btw> is the number of bytes to write. TODO: possibly smaller type?
 */
SDRESULT microsd_write(FIL* fp, const void* buff, unsigned int btw);

/* Assumes sd is connected and mounted, file is open
 * <fp> is the file being read from
 * <buf> is the buffer to write to
 * <btr> is the max number of bytes to read. TODO: possibly smaller type?
 */
SDRESULT microsd_read(FIL* fp, void* buf, unsigned int btr);
