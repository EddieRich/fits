#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "fits.h"

struct s_options
{
	int verbose;
	int show_header;
	char header[FITS_LINE_SIZE];
	char filepath[PATH_MAX];
} options = {0};

void show_options()
{
	printf("options:\n");
	printf("  show_header=%d", options.show_header);

	if (strlen(options.header) > 0)
		printf(" <%s>", options.header);

	printf("\n  verbose=%d\n", options.verbose);
	printf("  fits file=%s\n\n", options.filepath);
}

void show_usage(char *appname)
{
	printf("\nusage: %s [options] fits_file_full_path\noptions:\n", appname);
	printf("  -h<name> show header info. optional name is the single header name to show\n");
	printf("           note: the header, if specified, must immediately follow the -h option\n");
	printf("  -v       verbose, show lots of information\n");
	printf("\n\n");
	exit(1);
}

void fixFileName(char *filename)
{
	strnset(options.filepath, 0, PATH_MAX);
	char *cwd = getcwd(NULL, 0);
	strncpy(options.filepath, cwd, strlen(cwd));
	free(cwd);

	char *path_sep = "/";

#ifdef _WIN32
	path_sep = "\\";
#endif

	if (strncmp(path_sep, filename, 1) != 0)
		strcat(options.filepath, path_sep);

	strcat(options.filepath, filename);
	if (!strrchr(options.filepath, '.'))
		strcat(options.filepath, ".fits");
}

int main(int argc, char *argv[])
{
	int opt;

	while ((opt = getopt(argc, argv, "h::v")) != -1)
	{
		switch (opt)
		{
		case 'h':
			options.show_header = 1;
			strnset(options.header, 0, FITS_LINE_SIZE);
			if (optarg != NULL)
				strcpy(options.header, optarg);

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
		status = fitsShowHeader(options.header, options.verbose);
		if (status != E_SUCCESS)
		{
			show_error(status);
			exit(EXIT_FAILURE);
		}
	}

	closeFitsFile();
	exit(EXIT_SUCCESS);
}
