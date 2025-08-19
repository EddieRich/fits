#ifndef __FITS__
#define __FITS__

#include "fits_errors.h"

#define FITS_MAX_PATH 512
#define FITS_BLOCK_SIZE 2880
#define FITS_LINE_SIZE 80

typedef struct s_fits_image
{
	char image_name[FITS_LINE_SIZE];
	char filepath_noext[FITS_MAX_PATH];
} FITSImage_t;

FITS_ERROR openFitsFile(char *filepath);
void closeFitsFile();

FITS_ERROR fitsShowHeader(int verbose);
FITS_ERROR fitsGetImage(FITSImage_t fits_image);

void endian32(void *ptr, int count);

#endif // __FITS__
