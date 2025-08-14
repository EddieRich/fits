#ifndef __FITS__
#define __FITS__

#include "fits_errors.h"

#define FITS_MAX_PATH 512
#define FITS_BLOCK_SIZE 2880
#define FITS_LINE_SIZE 80

FITS_ERROR openFitsFile(char* filepath);
void closeFitsFile();

FITS_ERROR fitsShowHeader(int verbose);

#endif // __FITS__
