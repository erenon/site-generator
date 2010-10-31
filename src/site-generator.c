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

	if (argc < 2) {
		fprintf(stderr, "No input directory specified! Aborting...\n");
		exit(EXIT_FAILURE);
	}

	dir = dir_create(argv[1]);

	generator_process_widgets(dir);

	//dir_print(dir);

	dir_delete(dir);

	return EXIT_SUCCESS;
}
