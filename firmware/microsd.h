/* This is an abstracted "device driver" for the micro sd card.
 * (mostly just a layer upon the ChibiOS SDC driver)
 */

/* Connect to sd card, mount file system
 * (keeps trying until it succeeds ... implement time out?)
 * <fs> is the file system object to be mounted (does this have to be the same
 * across usages, or is it fine to have one for datalogging, one for control?)
 */
void microsd_card_try_init(FATFS* fs);

/* Unmount file system, disconnect sd, assumes no open files.
 */
void microsd_card_deinit(void);

/* Assumes sd is connected and mounted, file is open
 * (possibly needs some indicator that it isn't, ie. in the return value)
 * <fp> is the file to be written to
 * <buff> is the buffer containing the data to be written
 * <btw> is the number of bytes to write
 */
bool_t microsd_write(FIL* fp, const void* buff, UINT btw); 

/* Assumes sd is connected and mounted, file is open
 * <fp> is the file being read from
 * <buf> is the buffer to write to
 * <btr> is the max number of bytes to read
 */
bool_t microsd_read(FIL* fp, void* buf, UINT btr);
