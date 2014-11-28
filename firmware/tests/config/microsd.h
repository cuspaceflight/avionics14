/* This is a mockup microsd access layer for testing
 * uses standard lib functions for file system access instead of fatfs
 */

// ----------------------------------------------------------------------------
#ifndef MICRO_SD
#define MICROSD_H
#include <stdio.h>

typedef enum {
    SD_OK,
    SD_NO_FILE,
    SD_EXIST,
    SD_EOF,
    SD_ERR
} SDRESULT;

typedef enum {
    SD_READ,
    SD_WRITE,
    SD_CREATE_NEW
} SDMODE;

typedef FILE SDFILE;

void microsd_card_try_init(void);
void microsd_card_deinit(void);
SDRESULT microsd_open_file(SDFILE* fp, const char* path, SDMODE mode);
SDRESULT microsd_close_file(SDFILE* fp);
SDRESULT microsd_write(SDFILE* fp, const void* buff, unsigned int btw);
SDRESULT microsd_read(SDFILE* fp, void* buf, unsigned int btr);
SDRESULT microsd_gets(SDFILE* fp, char* buf, int size);
#endif
