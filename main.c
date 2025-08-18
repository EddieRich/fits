#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

#include "fits.h"

struct s_options
{
	int verbose;
	int show_header;
	int extract_image;
	char image_name[FITS_LINE_SIZE];
	char filepath[FITS_MAX_PATH];
	char filepath_noext[FITS_MAX_PATH];
} options = { 0 };

void show_options()
{
	printf("options:\n");
	printf("  extract_image=%d <%s>\n", options.extract_image, options.image_name);
	printf("  show_header=%d\n", options.show_header);
	printf("  verbose=%d\n", options.verbose);
	printf("  fits file=%s\n\n", options.filepath);
}

void show_usage(char* appname)
{
	if (appname[0] == '.' && appname[1] == '/')
		appname += 2;

	printf("\nusage: %s [options] fits_file\noptions:\n", appname);
	printf("  -i<name> extract an image, default name is 'SCI'\n");
	printf("           note: the name, if specified, must immediately follow the -i option\n");
	printf("           image output is Pf format\n");
	printf("  -h       show headers info\n");
	printf("  -v       verbose, show lots of information\n");
	printf("\n\n");
	exit(1);
}

void fixFileName(char* filename)
{
	memset(options.filepath, 0, FITS_MAX_PATH);
	memset(options.filepath_noext, 0, FITS_MAX_PATH);
	char* cwd = getcwd(NULL, 0);
	memcpy(options.filepath, cwd, strlen(cwd));
	free(cwd);

	char* path_sep = "/";
	if (strncmp(path_sep, filename, 1) != 0)
		strcat(options.filepath, path_sep);

	strcat(options.filepath, filename);
	char* pext = strrchr(options.filepath, '.');
	if (pext == NULL)
	{
		strncpy(options.filepath_noext, options.filepath, strlen(options.filepath));
		strcat(options.filepath, ".fits");
	}
	else
	{
		strncpy(options.filepath_noext, options.filepath, pext - options.filepath);
	}
}

int main(int argc, char* argv[])
{
	int opt;

	while ((opt = getopt(argc, argv, "i::hv")) != -1)
	{
		switch (opt)
		{
		case 'i':
			options.extract_image = 1;
			memset(options.image_name, 0, FITS_LINE_SIZE);
			strcpy(options.image_name, "SCI");
			if (optarg != NULL)
				strcpy(options.image_name, optarg);

			break;

		case 'h':
			options.show_header = 1;
			break;

		case 'v':
			options.verbose = 1;
			break;

		default:
			show_usage(argv[0]);
		}
	}

	if (optind >= argc)
	{
		show_error(E_NO_FITS_FILE_SPECIFIED);
		show_usage(argv[0]);
	}

	fixFileName(argv[optind]);

	printf("\nFITS file processor\n");

	if (options.verbose)
		show_options();

	FITS_ERROR status = openFitsFile(options.filepath);
	if (status != E_SUCCESS)
	{
		show_error(status);
		exit(EXIT_FAILURE);
	}

	if (options.show_header)
	{
		status = fitsShowHeader(options.verbose);
		if (status != E_SUCCESS)
		{
			show_error(status);
			exit(EXIT_FAILURE);
		}
	}

	if (options.extract_image)
	{
		status = fitsGetImage(options.image_name, options.filepath_noext);
		if (status != E_SUCCESS)
		{
			show_error(status);
			exit(EXIT_FAILURE);
		}
	}

	closeFitsFile();
	exit(EXIT_SUCCESS);
}
