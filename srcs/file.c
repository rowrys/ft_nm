# include "ft_nm.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <sys/mman.h>

bool
map_file(t_file* file, char* file_path) {
	int			fd;
	struct stat	stat;

	fd = open(file_path, O_RDONLY);
	if (fd == -1) {
		dprintf(STDERR_FILENO, "nm: '%s': No such file\n", file_path);
		return (1);
	}
	if (fstat(fd, &stat)) {
		dprintf(STDERR_FILENO, DEFAULT_ERROR_MSG);
		close(fd);
		return (1);
	}
	file->size = stat.st_size;
	file->buffer = mmap(NULL, stat.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	close(fd);
	if (file->buffer == MAP_FAILED) {
		dprintf(STDERR_FILENO, DEFAULT_ERROR_MSG);
		return (1);
	}
	return (0);
}
