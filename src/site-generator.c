/*
 * site-generator.c
 *
 *  Created on: 2010.10.24.
 *      Author: erenon
 */

#include <stdio.h>
#include <stdlib.h>
#include "genlib.h"
#include "dir.h"

int main(int argc, char *argv[]) {
	//read index
	Dir *dir;

	dir = dir_create("");

	dir_print(dir);

	dir_delete(dir);

	return EXIT_SUCCESS;
}
