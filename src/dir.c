/*
 * @author erenon
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "genlib.h"
#include "dir.h"

/**
 * Max length of indexed file names
 */
#define DIR_MAX_LINE_LENGTH 1024


/**
 * Parses file name
 *
 * Explodes the full name at the last dot,
 * and stores the parts as the name and the extension
 * of the given file.
 * If no dot found, extension will be empty.
 *
 * @param *file
 * @param fname[] name to process
 */
static void parse_fname(File *file, char *fname) {
	int len,i;
	len = strlen(fname);

	for (i=len;i>=0;i--) {
		if (fname[i] == '.') {
			//copy name
			file->name = (char *)smalloc((i+2) * sizeof(char));
			strncpy(file->name, fname, i);
			file->name[i] = '\0';

			//copy extension
			file->extension = (char *)smalloc((len-i) * sizeof(char));
			strcpy(file->extension, fname+i+1);
			file->extension[len-i-1] = '\0';

			return;
		}
	}

	//no dot found
	file->name = (char *)smalloc((len+1) * sizeof(char));
	strcpy(file->name, fname);
	file->name[len] = '\0';

	file->extension = (char *)smalloc(1 * sizeof(char));
	file->extension[0] = '\0';

	return;
}

/**
 * Reads a file based on the given filename
 * into a File. Prints error if file not found.
 *
 * @param *fname Name of the file to read
 * @return File* or NULL if file not found.
 */
static File *read_file(char *fname) {
	FILE *fp = NULL;
	int filesize;
	File *file;

	//hack! trim newline;
	fname[strlen(fname)-1] = '\0';

	//TODO add error h
	fp = fopen(fname, "r");
	if (fp == NULL) {
		fprintf(stderr, "Failed to read file: '%s', file skipped.\n", fname);
		//*status = STATUS_CODE_FAILED;
		return NULL;
	}

	//get file length
	fseek(fp, 0, SEEK_END);
	filesize = ftell(fp);

	file = (File *)smalloc(sizeof(File));

	file->content = (char *)smalloc(filesize+1 * sizeof(char));

	if (filesize > 0) {
		rewind(fp);
		fread(file->content, filesize, 1, fp);
	}

	file->content[filesize] = '\0';

	parse_fname(file, fname);

	fclose(fp);

	return file;
}

/**
 * Reads the given dir index file,
 * and calls read_file to read the indexed files
 * as well.
 *
 * @warning Aborts the execution if no index found.
 *
 * @param *dir Dir to read
 */
static void read(Dir *dir) {
	FILE *index = NULL;
	char file_name[DIR_MAX_LINE_LENGTH];
	int linec=0, i;
	char dir_index[] = "index";
	File *cfile = NULL;

	//TODO handle path

	index = fopen(dir_index, "r");
	if (index == NULL) {
	    fprintf(stderr, "Failed to read index! Aborting...\n");
	    exit(EXIT_FAILURE);
	}

	while (fgets(file_name, DIR_MAX_LINE_LENGTH, index) != NULL) {
		linec++;
	}

	rewind(index);

	dir->file_to_process_count = linec;
	dir->files = (File **)smalloc(linec * sizeof(File *));

	i=0;
	while (fgets(file_name, DIR_MAX_LINE_LENGTH, index) != NULL) {
		cfile = read_file(file_name);
		if (cfile) {
			dir->files[i] = cfile;
			dir->files_count++;
			i++;
		}
	}


	fclose(index);
}

/**
 * Creates a Dir instance,
 * resets its parameters,
 * and call read to read the whole dir.
 *
 * @todo handle path
 * @param *path Path to read from. Not implemented.
 * @return Dir* dir instance
 */
Dir *dir_create(char *path) {
	Dir *dir = NULL;

	dir = (Dir *)smalloc(sizeof(Dir));

	dir->file_to_process_count = 0;
	dir->files_count = 0;

	read(dir);

	return dir;
}

/**
 * Prints the contents of the dir
 * Debug purposes only
 */
void dir_print(Dir *dir) {
	int i;

	//printf("path: %s\nfiles:\n\n", dir->path);

	for (i=0; i < dir->files_count; i++) {
		printf("filename: %s\n", dir->files[i]->name);
		printf("ext: %s\n", dir->files[i]->extension);
		printf("%s\n\n",dir->files[i]->content);
	}
}

/**
 * Frees the dynamic parts of the given file,
 * and finally the given file itself.
 *
 * @param *file file to free
 */
static void file_delete(File *file) {
	sfree(file->name);
	sfree(file->extension);
	sfree(file->content);
	sfree(file);
}

/**
 * Frees all the files of the given dir,
 * and the dir itself.
 *
 * @param *dir dir to free
 */
void dir_delete(Dir *dir) {
	int i;

	for (i=0; i < dir->files_count; i++) {
		file_delete(dir->files[i]);
	}
	sfree(dir->files);

	sfree(dir);
}
