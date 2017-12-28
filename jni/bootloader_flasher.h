#ifndef __bootloader_flasher__h
#define __bootloader_flasher__h

#include "typedefs.h"

extern uint32_t flasher_download(const unsigned char *filepath,uint32_t startaddr,flasher_progress_callback *callback);

#endif
