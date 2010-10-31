/*
 * @author erenon
 */

#include <stdio.h>
#include <stdlib.h>
#include "genlib.h"
#include "dir.h"
#include "generator.h"

int main(int argc, char *argv[]) {
	//read index
	Dir *dir;

	dir = dir_create("");

	generator_process_widgets(dir);

	//dir_print(dir);

	dir_delete(dir);

	return EXIT_SUCCESS;
}
