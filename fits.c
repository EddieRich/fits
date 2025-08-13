#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fits.h"

FILE *fits_fp = NULL;
char block[FITS_BLOCK_SIZE];
char line[FITS_LINE_SIZE + 1];
char *blptr = block;

FITS_ERROR openFitsFile(char *filepath)
{
	if (fits_fp != NULL)
		fclose(fits_fp);

	fits_fp = fopen(filepath, "rb");
	if (fits_fp == NULL)
		return E_FITS_FILE_OPEN;

	strnset(block, 0, FITS_BLOCK_SIZE);
	strnset(line, 0, FITS_LINE_SIZE + 1);

	return E_SUCCESS;
}

void closeFitsFile()
{
	if (fits_fp != NULL)
		fclose(fits_fp);
}

int lineStartsWith(char *name)
{
	return !strncmp(name, line, strlen(name));
}

FITS_ERROR readBlock()
{
	size_t bytes_read = fread(block, 1, FITS_BLOCK_SIZE, fits_fp);
	if (bytes_read != FITS_BLOCK_SIZE)
		return E_BAD_BLOCK_READ;

	blptr = block;
	return E_SUCCESS;
}

FITS_ERROR nextLine()
{
	strnset(line, 0, FITS_LINE_SIZE + 1);
	if (blptr > block + FITS_BLOCK_SIZE - FITS_LINE_SIZE)
		return E_END_OF_BLOCK;

	strncpy(line, blptr, FITS_LINE_SIZE);
	blptr += FITS_LINE_SIZE;

	return E_SUCCESS;
}

FITS_ERROR fitsShowHeader(char *header, int verbose)
{
	FITS_ERROR status;
	while (1)
	{
		status = readBlock();
		if (status != E_SUCCESS)
			return status;

		status = nextLine();
		if (status != E_SUCCESS)
			return status;

		if (strlen(header) == 0 || lineStartsWith(header))
		{
			printf("\n%s\n", line);

			while (1)
			{
				status = nextLine();
				if (status == E_SUCCESS)
				{
					if (verbose)
					{
						printf("%s\n", line);
					}
					else
					{
						if (lineStartsWith("EXTNAME") || lineStartsWith("BITPIX") || lineStartsWith("NAXIS"))
							printf("%s\n", line);
					}

					if (lineStartsWith("END"))
					{
						printf("\n------------------------\n");
						break;
					}
				}
				else if (status == E_END_OF_BLOCK)
				{
					status = readBlock();
					if (status != E_SUCCESS)
						return status;
				}
			}

			if (strlen(header) > 0)
				break;
		}
	}

	return E_SUCCESS;
}
