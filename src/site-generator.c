/*
 * @author erenon
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "genlib.h"
#include "dir.h"
#include "generator.h"

int main(int argc, char *argv[]) {
	Dir *dir;
	char path[DIR_MAX_PATH_LENGTH];

	if (argc >= 2) {
		strcpy(path, argv[1]);
		path[strlen(argv[1])] = '\0';
	} else {
		strcpy(path, "./");
	}

	dir = dir_create(path);

	generator_process_widgets(dir);
	generator_process_layout(dir);
	generator_process_pages(dir);

	//dir_print(dir);

	dir_delete(dir);

	return EXIT_SUCCESS;
}
