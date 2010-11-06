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
	char input_path[DIR_MAX_PATH_LENGTH],
		 output_path[DIR_MAX_PATH_LENGTH];
	STATUS s;

	/*set input path*/
	if (argc >= 2) {
		strcpy(input_path, argv[1]);
	} else {
		strcpy(input_path, "./");
	}

	/*set output path*/
	if (argc >= 3) {
		strcpy(output_path, argv[2]);
	} else {
		strcpy(output_path, "./");
	}

	dir = dir_create(input_path, &s);

	if (s == STATUS_CODE_FAILED) {
		fprintf(stderr, "Failed to read index! Aborting...\n");
		return EXIT_FAILURE;
	}

	generator_process_widgets(dir);
	generator_process_layout(dir);
	generator_process_pages(dir);

	dir_write(dir, output_path);

	dir_delete(dir);

	return EXIT_SUCCESS;
}
