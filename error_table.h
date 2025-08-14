#define ERROR_TABLE                                        \
	E(E_SUCCESS, "success")                                  \
	E(E_NO_FITS_FILE_SPECIFIED, "No fits filespecified")     \
	E(E_FITS_FILE_OPEN, "could not open file for reading")   \
	E(E_END_OF_FILE, "end of file reached") \
	E(E_BAD_BLOCK_READ, "attempt to read full block failed") \
	E(E_END_OF_BLOCK, "attempt to read past end of block") \
	E(E_NO_VALUE_INDICATOR, "record is missing the value indicator") \
	E(E_END_OF_LINE, "the end of the record was reached") \
	E(E_INVALID_CHARACTER, "invalid character found") \
	E(E_MISSING_VALUE, "value is missing from the record")
