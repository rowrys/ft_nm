
#include "ft_nm.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include <sys/mman.h>

void*
map_file(char* file_path) {
	uchar*		result;
	int			fd;
	struct stat	stat;

	fd = open(file_path, O_RDONLY);
	if (fd == -1) {
		dprintf(STDERR_FILENO, "nm: '%s': No such file", file_path);
		exit(1);
	}
	if (fstat(fd, &stat)) {
		dprintf(STDERR_FILENO, DEFAULT_ERROR_MSG);
		close(fd);
		exit(1);
	}
	result = mmap(NULL, stat.st_size, PROT_READ, MAP_SHARED, fd, 0);
	close(fd);
	if (result == MAP_FAILED) {
		dprintf(STDERR_FILENO, DEFAULT_ERROR_MSG);
		exit(1);
	}
	return (result);
}
