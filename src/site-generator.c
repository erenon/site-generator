/*
 * @author erenon
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "genlib.h"
#include "dir.h"
#include "generator.h"

/**
 * Looks for input path in the given string array,
 * and stores it in the given string.
 * Uses default "./" if no input path found.
 *
 * @param[out] *input_path
 * @param[in] argc size of argv
 * @param[in] *argv[]
 */
static void set_input_path(char *input_path, int argc, char *argv[]) {
	if (argc >= 2) {
		strcpy(input_path, argv[1]);
	} else {
		strcpy(input_path, "./");
	}
}

/**
 * Looks for output path in the given string array,
 * and stores it in the given string.
 * Uses default "./" if no output path found.
 *
 * @param[out] *output_path
 * @param[in] argc size of argv
 * @param[in] *argv[]
 */
static void set_output_path(char *output_path, int argc, char *argv[]) {
	if (argc >= 3) {
		strcpy(output_path, argv[2]);
	} else {
		strcpy(output_path, "./");
	}
}

/**
 * Looks for the path of the separate image directory
 * in the given string array,
 * and stores it in the given string.
 * Uses default empty string if no image path found.
 *
 * @param[out] *img_dir
 * @param[in] argc size of argv
 * @param[in] *argv[]
 */
static void set_img_dir(char *img_dir, int argc, char *argv[]) {
	int i;

	for (i=0; i < argc; i++) {
		if (strcmp(argv[i], "--img") == 0) {
			if (i+1 < argc) { /* there is param after the switch */
				strcpy(img_dir, argv[i+1]);
				return;
			}
		}
	}

	/* no switch and path found
	 * use default */
	strcpy(img_dir, "");
	return;
}

/**
 * Site generator
 *
 * Creates formatted HTML pages from plaintext files.
 *
 * USAGE:
 * site-generator input_directory/ output_directory/ [--img img_directory/]
 */
int main(int argc, char *argv[]) {
	Dir *dir;
	char input_path[MAX_PATH_LENGTH],
		 output_path[MAX_PATH_LENGTH];
	STATUS s;

	set_input_path(input_path, argc, argv);
	set_output_path(output_path, argc, argv);
	set_img_dir(g_cfg.img_dir, argc, argv);


	dir = dir_create(input_path, &s);

	if (s == STATUS_FAILED) {
		fprintf(stderr, "Failed to read index! Aborting...\n");
		return EXIT_FAILURE;
	}

	g_cfg.dir = dir;

	generator_process_widgets(dir);
	generator_process_layout(dir);
	generator_process_pages(dir);

	s = dir_write(dir, output_path);

	if (s == STATUS_FAILED) {
		fprintf(
			stderr,
			"Unable to write one or more file. Does the given dir '%s' exist?\n",
			output_path
		);
	}

	dir_delete(dir);

	return EXIT_SUCCESS;
}
