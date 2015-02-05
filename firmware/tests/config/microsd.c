#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <errno.h>
#include "microsd.h"

/* This implementation uses the C standard library file streams to emulate file
 * access on an SD card.
 * Note that the signature of the microsd_open_file function requires that you
 * can pass in a file pointer and have it assigned by the function. Since this
 * is not actually possible in stdlib (but fine in FatFS), I've used a total
 * hack:
 * When a file is opened, assign an internal global variable to the current
 * "file", and use that in any other function call. This is of course not
 * thread-safe at all, but for config testing purposes it should be sufficient.
 */

static FILE* file = NULL;

void microsd_card_try_init(void) {}
void microsd_card_deinit(void) {}

// TODO: SD_CREATE_NEW should return error if file already exists
static const char* translate_mode(SDMODE mode)
{
    switch(mode) {
        case SD_READ: return "r";
        case SD_WRITE: return "w";
        case SD_CREATE_NEW: return "w+";
    }
    return "";
}

SDRESULT microsd_open_file(SDFILE* fp, const char* path, SDMODE mode)
{
    file = fopen(path, translate_mode(mode));
    if (file != NULL) return SD_OK;
    if (errno == EACCES) return SD_NO_FILE;
    return SD_ERR;
    // what about SD_EXIST?
}

SDRESULT microsd_close_file(SDFILE* fp)
{
    if (file == NULL) return SD_ERR;
    int res = fclose(file);
    file = NULL;
    return res == 0 ? SD_OK : SD_ERR;
}

SDRESULT microsd_write(SDFILE* fp, const void* buf, unsigned int btw)
{
    if (file == NULL) return SD_ERR;
    size_t written = fwrite(buf, btw, sizeof(char), file);
    fflush(file);
    return written == btw ? SD_OK : SD_ERR;
}

SDRESULT microsd_read(SDFILE* fp, void* buf, unsigned int btr)
{
    if (file == NULL) return SD_ERR;
    size_t read = fread(buf, btr, sizeof(char), file);
    if (read == btr || feof(file)) {
        return SD_OK; //note: EOF regarded as success
    }
    return SD_ERR;
}

SDRESULT microsd_gets(SDFILE* fp, char* buf, int size)
{
    if (file == NULL) return SD_ERR;
    char* res = fgets(buf, size, file);
    return res != NULL ? SD_OK :
           feof(file)  ? SD_EOF :
                         SD_ERR;
}
