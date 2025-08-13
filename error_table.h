#define ERROR_TABLE                                        \
	E(E_SUCCESS, "success")                                  \
	E(E_NO_FITS_FILE_SPECIFIED, "No fits filespecified")     \
	E(E_FITS_FILE_OPEN, "could not open file for reading")   \
	E(E_BAD_BLOCK_READ, "attempt to read full block failed") \
	E(E_END_OF_BLOCK, "attempt to read past end of block")
