#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "fits.h"

#ifdef _WIN32
#define PATH_SEPARATOR '\\'
#else
#define PATH_SEPARATOR '/'
#endif

struct s_options
{
	int verbose;
	int info;
	char filepath[PATH_MAX];
} options = {0};

void show_options()
{
	printf("options:\n");
	printf("  info=%d\n", options.info);
	printf("  verbose=%d\n", options.verbose);
	printf("  fits file=%s\n\n", options.filepath);
}

void show_usage(char *appname)
{
	printf("\nusage: %s [options] fits_file_full_path\noptions:\n", appname);
	printf("  -i       show file info\n");
	printf("  -v       verbose, show lots of information\n");
	printf("\n\n");
	exit(1);
}

int findfile(char *filename)
{
	char fullname[PATH_MAX];
	strnset(options.filepath, 0, PATH_MAX);
	strnset(fullname, 0, PATH_MAX);

	strcpy(fullname, filename);
	if (!strrchr(fullname, '.'))
		strcat(fullname, ".fits");

	if (access(fullname, F_OK) == 0)
	{
		strcpy(options.filepath, fullname);
		return 0;
	}

	// check the users "Pictures" directory
	char *home_dir = getenv("HOME");
	if (home_dir == NULL)
	{
		printf("Error: HOME environment variable not found.\n");
		return -1;
	}

	char pictures_path[_MAX_PATH];
	strnset(pictures_path, 0, PATH_MAX);
	sprintf(pictures_path, "%s%cPictures%c", home_dir, PATH_SEPARATOR, PATH_SEPARATOR);

	char search_name[PATH_MAX];
	strnset(search_name, 0, PATH_MAX);
	strncpy(search_name, pictures_path, strlen(pictures_path));
	strncat(search_name, fullname, strlen(fullname));

	if (options.verbose)
		printf("Searching %s", search_name);

	if (access(search_name, F_OK) == 0)
	{
		if (options.verbose)
			printf(" : found\n");

		strcpy(options.filepath, search_name);
		return 0;
	}

	return -1;
}

int main(int argc, char *argv[])
{
	int opt;

	while ((opt = getopt(argc, argv, "iv")) != -1)
	{
		switch (opt)
		{
		case 'i':
			options.info = 1;
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
		printf("\n!!! No FITS file specified\n");
		show_usage(argv[0]);
	}

	if (findfile(argv[optind]) < 0)
	{
		printf("\nCould not find file %s\n", argv[optind]);
		exit(EXIT_FAILURE);
	}

	printf("\nFITS file processor\n");

	if (options.verbose)
		show_options();

	exit(EXIT_SUCCESS);
}
