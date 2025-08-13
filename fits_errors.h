#ifndef __FITS_ERRORS__
#define __FITS_ERRORS__

#include "error_table.h"
#define E(c, m) c,
typedef enum
{
	ERROR_TABLE
			NUM_ERRORS
} FITS_ERROR;
#undef E

void show_error(FITS_ERROR error_code);

#endif // __FITS_ERRORS__
