/* This is an abstracted "device driver" for the micro sd card and file system,
 * pretty much acts as an access layer.
 */

// ----------------------------------------------------------------------------

/* Purely to hide dependencies to ff.h in here */

typedef enum {
    SD_OK,         // all is good
    SD_NO_FILE,    // couldn't find the file
    SD_EXIST,      // file already exists (when opened in CREATE_NEW mode)
    SD_EOF,        // reached end of file (when reading)
    SD_ERR         // some kind of error!!! this may need expansion
} SDRESULT;

typedef enum {
    SD_READ,       // open file for reading
    SD_WRITE,      // open file for writing
    SD_CREATE_NEW  // create file and open it (return SD_EXIST if it exists)
} SDMODE;

typedef FIL SDFILE;

// ----------------------------------------------------------------------------

/* File system/SD card functions */

/* Connect to sd card, mount file system. Implement timeout? */
void microsd_card_try_init(void);

/* Unmount file system, disconnect sd, assumes no open files. */
void microsd_card_deinit(void);

/* Open file in path <path> and opening mode <mode> to file object <fp>. */
SDRESULT microsd_open_file(SDFILE* fp, const char* path, SDMODE mode);

/* Close file object <fp>. */
SDRESULT microsd_close_file(SDFILE* fp);

/* Assumes sd is connected and mounted, file is open.
 * Writes at most <btw> bytes from <buff> to <fp>.
 */
SDRESULT microsd_write(SDFILE* fp, const void* buff, unsigned int btw);

/* Assumes sd is connected and mounted, file is open.
 * Reads at most <btr> bytes from <fp> to <buf>
 */
SDRESULT microsd_read(SDFILE* fp, void* buf, unsigned int btr);

/* Assumes sd is connected and mounted, file is open.
 * Reads at most <size> bytes from <fp> to <buf>, stopping at newlines/EOF.
 */
SDRESULT microsd_gets(SDFILE* fp, void* buf, unsigned int size);
