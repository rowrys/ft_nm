
#include "ft_nm.h"
#include "file.h"

#include <elf.h>

int
main(void) {
	uchar*	file;
	char*	file_path;

	file_path = DEFAULT_FILE_PATH;
	file = map_file(file_path);
	(void)file;
	return (0);
}
