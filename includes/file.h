#ifndef FILE_H
# define FILE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct s_file {
	void*	buffer;
	size_t	size;
}	t_file;

bool	map_file(t_file* file, char* file_path);

#endif
