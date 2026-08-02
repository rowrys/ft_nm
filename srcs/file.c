
# include "ft_nm.h"
# include "file.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include <sys/mman.h>

void
map_file(t_file* file, char* file_path) {
	int			fd;
	struct stat	stat;

	fd = open(file_path, O_RDONLY);
	if (fd == -1) {
		dprintf(STDERR_FILENO, "nm: '%s': No such file\n", file_path);
		exit(1);
	}
	if (fstat(fd, &stat)) {
		dprintf(STDERR_FILENO, DEFAULT_ERROR_MSG);
		close(fd);
		exit(1);
	}
	file->size = stat.st_size;
	file->buffer = mmap(NULL, stat.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	close(fd);
	if (file->buffer == MAP_FAILED) {
		dprintf(STDERR_FILENO, DEFAULT_ERROR_MSG);
		exit(1);
	}
}
