// PFM format spec - https://netpbm.sourceforge.net/doc/pfm.html

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <float.h>
#include <locale.h>

#include "fits.h"

FILE* fits_fp = NULL;
char block[FITS_BLOCK_SIZE];
char line[FITS_LINE_SIZE + 1];
char* blptr = block;

FITS_ERROR openFitsFile(char* filepath)
{
	if (fits_fp != NULL)
		fclose(fits_fp);

	fits_fp = fopen(filepath, "rb");
	if (fits_fp == NULL)
		return E_FITS_FILE_OPEN;

	memset(block, 0, FITS_BLOCK_SIZE);
	memset(line, 0, FITS_LINE_SIZE + 1);

	return E_SUCCESS;
}

void closeFitsFile()
{
	if (fits_fp != NULL)
		fclose(fits_fp);
}

int lineStartsWith(char* name)
{
	return !strncmp(name, line, strlen(name));
}

FITS_ERROR readBlock()
{
	if (feof(fits_fp))
		return E_END_OF_FILE;

	size_t bytes_read = fread(block, 1, FITS_BLOCK_SIZE, fits_fp);
	if (bytes_read != FITS_BLOCK_SIZE)
	{
		if (feof(fits_fp))
			return E_END_OF_FILE;
		else
			return E_BAD_BLOCK_READ;
	}

	blptr = block;
	return E_SUCCESS;
}

FITS_ERROR nextLine()
{
	memset(line, 0, FITS_LINE_SIZE + 1);
	if (blptr > block + FITS_BLOCK_SIZE - FITS_LINE_SIZE)
		return E_END_OF_BLOCK;

	memcpy(line, blptr, FITS_LINE_SIZE);
	blptr += FITS_LINE_SIZE;

	return E_SUCCESS;
}

FITS_ERROR getLineIntValue(int* result)
{
	char strvalue[FITS_LINE_SIZE];

	memset(strvalue, 0, FITS_LINE_SIZE);

	if (strncmp(&line[8], "= ", 2) != 0)
		return E_NO_VALUE_INDICATOR;

	int i = 10;
	while (line[i] == ' ' && i < 80)
		i++;

	if (i == 80)
		return E_END_OF_LINE;

	if (line[i] == '/')
		return E_MISSING_VALUE;

	int s = i;
	while (i < 80 && line[i] != ' ' && line[i] != '/')
	{
		if (line[i] == '-' || line[i] == '+' || isdigit(line[i]))
			i++;
		else
			return E_INVALID_CHARACTER;
	}

	memcpy(strvalue, &line[s], i - s);
	*result = atoi(strvalue);
	return E_SUCCESS;
}

FITS_ERROR fitsShowHeader(int verbose)
{
	FITS_ERROR status;
	int bitpix;
	int naxis;
	int axis[6];

	rewind(fits_fp);

	while (1)
	{
		bitpix = 0;
		naxis = 0;
		memset(axis, 0, sizeof(int) * 6);

		status = readBlock();
		if (status == E_END_OF_FILE)
			break;
		if (status != E_SUCCESS)
			return status;

		status = nextLine();
		if (status != E_SUCCESS)
			return status;

		printf("\n%s\n", line);
		while (1)
		{
			status = nextLine();
			if (status == E_SUCCESS)
			{
				if (verbose)
					printf("%s\n", line);

				if (lineStartsWith("BITPIX"))
				{
					if (!verbose)
						printf("%s\n", line);

					status = getLineIntValue(&bitpix);
					if (status != E_SUCCESS)
						return status;
				}

				if (lineStartsWith("EXTNAME"))
				{
					if (!verbose)
						printf("%s\n", line);
				}

				// note the space at the end
				if (lineStartsWith("NAXIS "))
				{
					if (!verbose)
						printf("%s\n", line);

					status = getLineIntValue(&naxis);
					if (status != E_SUCCESS)
						return status;
				}
				// note there is no space (NAXIS1 NAXIS2 NAXIS3 etc.)
				else if (lineStartsWith("NAXIS"))
				{
					if (!verbose)
						printf("%s\n", line);

					int index = line[5] - '1';
					status = getLineIntValue(&axis[index]);
					if (status != E_SUCCESS)
						return status;
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

		if (naxis > 0)
		{
			int byte_count = abs(bitpix) / 8;
			for (int i = 0; i < naxis; i++)
				byte_count *= axis[i];

			int hangover = byte_count % FITS_BLOCK_SIZE;
			if (hangover)
				byte_count += FITS_BLOCK_SIZE - hangover;

			fseek(fits_fp, (long)byte_count, SEEK_CUR);
		}
	}

	return E_SUCCESS;
}

FITS_ERROR fitsGetImage(FITSImage_t* pfi, int verbose)
{
	FITS_ERROR status;
	int bitpix;
	int naxis;
	int axis[6];
	int found = 0;

	rewind(fits_fp);

	while (!found)
	{
		bitpix = 0;
		naxis = 0;
		memset(axis, 0, sizeof(int) * 6);

		status = readBlock();
		if (status == E_END_OF_FILE)
			break;
		if (status != E_SUCCESS)
			return status;

		status = nextLine();
		if (status != E_SUCCESS)
			return status;

		while (1)
		{
			status = nextLine();
			if (status == E_SUCCESS)
			{
				if (lineStartsWith("BITPIX"))
				{
					status = getLineIntValue(&bitpix);
					if (status != E_SUCCESS)
						return status;
				}

				if (lineStartsWith("EXTNAME"))
				{
					if (strncmp(&line[11], pfi->image_name, strlen(pfi->image_name)) == 0)
						found = 1;
				}

				// note the space at the end
				if (lineStartsWith("NAXIS "))
				{
					status = getLineIntValue(&naxis);
					if (status != E_SUCCESS)
						return status;
				}
				// note there is no space (NAXIS1 NAXIS2 NAXIS3 etc.)
				else if (lineStartsWith("NAXIS"))
				{
					int index = line[5] - '1';
					status = getLineIntValue(&axis[index]);
					if (status != E_SUCCESS)
						return status;
				}

				if (lineStartsWith("END"))
					break;
			}
			else if (status == E_END_OF_BLOCK)
			{
				status = readBlock();
				if (status != E_SUCCESS)
					return status;
			}
		}

		if (found && naxis == 2 && bitpix == -32)
		{
			pfi->width = axis[0];
			pfi->height = axis[1];
			pfi->pixels = pfi->width * pfi->height;
			pfi->bytes = pfi->pixels * sizeof(float);

			float* image = malloc(pfi->bytes);
			if (image == NULL)
				return E_IMAGE_BUFFER_CREATE;

			fread(image, sizeof(float), pfi->pixels, fits_fp);
			endian32(image, pfi->pixels);

			pfi->max = FLT_MIN;
			pfi->min = FLT_MAX;
			for (int i = 0; i < pfi->pixels; i++)
			{
				if (image[i] > pfi->max)
					pfi->max = image[i];
				if (image[i] < pfi->min)
					pfi->min = image[i];
			}

			// process here

			char outfile[FITS_MAX_PATH];
			sprintf(outfile, "%s_%s.pfm", pfi->filepath_noext, pfi->image_name);
			FILE* ofp = fopen(outfile, "wb");
			if (ofp == NULL)
				return E_IMAGE_FILE_OPEN;

			fprintf(ofp, "Pf\n%d %d\n-1.0\n", axis[0], axis[1]);
			fwrite(image, sizeof(float), pfi->pixels, ofp);
			fflush(ofp);
			fclose(ofp);
			free(image);

			if (verbose)
			{
				setlocale(LC_NUMERIC, "");
				printf("\nImage %s : %'d x %'d, %'d pixels, %'d bytes\n", pfi->image_name, pfi->width, pfi->height, pfi->pixels, pfi->bytes);
				printf("MAX = %f MIN = %f\n", pfi->max, pfi->min);
			}
			return E_SUCCESS;
		}
		else if (naxis > 0)
		{
			int byte_count = abs(bitpix) / 8;
			for (int i = 0; i < naxis; i++)
				byte_count *= axis[i];

			int hangover = byte_count % FITS_BLOCK_SIZE;
			if (hangover)
				byte_count += FITS_BLOCK_SIZE - hangover;

			fseek(fits_fp, (long)byte_count, SEEK_CUR);
		}
	}

	return E_SUCCESS;
}
