#include <stdio.h>
#include "fits_errors.h"
#include "error_table.h"

#define E(c, m) m,
const char *error_message[] =
		{
				ERROR_TABLE};
#undef E

void show_error(FITS_ERROR error_code)
{
	printf("error: %s\n", error_message[error_code]);
}
