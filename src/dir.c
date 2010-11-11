/*
 * @author erenon
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "genlib.h"
#include "dir.h"

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
	len = (int)strlen(fname);

	for (i=len;i>=0;i--) {
		if (fname[i] == '.') {
			/*copy name*/
			file->name = (char *)smalloc((i+2) * sizeof(char));
			strncpy(file->name, fname, (size_t)i);
			file->name[i] = '\0';

			/*copy extension*/
			file->extension = (char *)smalloc((len-i) * sizeof(char));
			strcpy(file->extension, fname+i+1);
			file->extension[len-i-1] = '\0';

			return;
		}
	}

	/*no dot found*/
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
 * @param *path Path to the file
 * @param *fname Name of the file to read
 * @return File* or NULL if file not found.
 */
static /*@null@*/ File *read_file(char *path, char *fname) {
	char file_path[MAX_PATH_LENGTH];
	FILE *fp = NULL;
	long int filesize;
	File *file;

	/*hack! trim newline;*/
	fname[strlen(fname)-1] = '\0';

	strcpy(file_path, path);
	strcat(file_path, fname);

	fp = fopen(file_path, "rt");
	if (fp == NULL) {
		fprintf(stderr, "Failed to read file: '%s', file skipped.\n", fname);
		return NULL;
	}

	/*get file length*/
	(void)fseek(fp, 0, SEEK_END);
	filesize = ftell(fp);

	/*create file*/
	file = (File *)smalloc(sizeof(File));

	file->content = (char *)smalloc((size_t)filesize+1 * sizeof(char));

	if (filesize > 0) {
		rewind(fp);
		(void)fread(file->content, (size_t)filesize, 1, fp);
	}

	file->content[filesize] = '\0';

	parse_fname(file, fname);

	(void)fclose(fp);

	return file;
}

/**
 * Reads the given dir index file,
 * and calls read_file to read the indexed files
 * as well.
 *
 * @param *dir Dir to read
 *
 * @return STATUS_SUCC or STATUS_FAILED if failed to read index
 */
static STATUS read(Dir *dir) {
	FILE *index = NULL;
	char file_name[DIR_MAX_LINE_LENGTH];
	int linec=0, i;
	char dir_index[MAX_PATH_LENGTH];
	File *cfile = NULL;

	strcpy(dir_index, dir->path);
	strcat(dir_index, "index");

	index = fopen(dir_index, "rt");
	if (index == NULL) {
	    return STATUS_FAILED;
	}

	while (fgets(file_name, DIR_MAX_LINE_LENGTH, index) != NULL) {
		linec++;
	}

	rewind(index);

	dir->file_to_process_count = linec;
	dir->files = (File **)smalloc(linec * sizeof(File *));

	i=0;
	while (fgets(file_name, DIR_MAX_LINE_LENGTH, index) != NULL) {
		cfile = read_file(dir->path, file_name);
		if (cfile) {
			dir->files[i] = cfile;
			dir->files_count++;
			i++;
		}
	}

	(void)fclose(index);

	return STATUS_SUCC;
}

/**
 * Creates a Dir instance,
 * resets its parameters,
 * and call read to read the whole dir.
 *
 * @param *path Path to read from.
 * @param *s
 *
 * @return Dir* dir instance
 */
Dir *dir_create(char *path, STATUS *s) {
	Dir *dir = NULL;

	dir = (Dir *)smalloc(sizeof(Dir));

	dir->path = (char *)smalloc((strlen(path)+1) * sizeof(char));
	strcpy(dir->path, path);

	dir->file_to_process_count = 0;
	dir->files_count = 0;
	dir->layout_index = UNINITIALIZED;

	*s = read(dir);

	return dir;
}

#if DEBUG
/**
 * Prints the contents of the dir
 * Debug purposes only
 */
void dir_print(Dir *dir) {
	int i;

	printf("path: %s\nfiles:\n\n", dir->path);

	for (i=0; i < dir->files_count; i++) {
		printf("filename: %s\n", dir->files[i]->name);
		printf("ext: %s\n", dir->files[i]->extension);
		printf("%s\n\n",dir->files[i]->content);
	}
}
#endif

/**
 * Writes the given file to the given path
 *
 * @param *file File to write out
 * @param *path Target path
 */
static STATUS file_write(File *file, char *path) {
	FILE *fp;
	char *file_name, *ext = ".html";

	file_name = (char *)smalloc(
			(strlen(path) + strlen(file->name) + strlen(ext) + 1)
			* sizeof(char)
	);

	strcpy(file_name, path);
	strcat(file_name, file->name);
	strcat(file_name, ext);

	fp = fopen(file_name, "wt");
	if (fp == NULL) {
		fprintf(stderr, "Failed to write '%s', file skipped.\n", file_name);
		sfree(file_name);
		return STATUS_FAILED;
	}

	(void)fwrite(file->content, sizeof(char), strlen(file->content), fp);

	(void)fclose(fp);
	sfree(file_name);

	return STATUS_SUCC;
}

/**
 * Writes the given dir to the filesystem
 *
 * @see file_write
 *
 * @param[in] *dir Dir to write out
 * @param[in] *path Target path
 * @return STATUS STATUS_SUCC if write of all the
 * contained files was successful, STATUS_FAILED otherwise
 */
STATUS dir_write(Dir *dir, char *path) {
	int i;
	STATUS curr_s, s = STATUS_SUCC;

	for (i=0; i < dir->files_count; i++) {
		if ( strcmp(dir->files[i]->extension, "page") == 0) {
			curr_s = file_write(dir->files[i], path);
			if (curr_s == STATUS_FAILED) {
				s = STATUS_FAILED;
			}
		}
	}

	return s;
}

/**
 * Determines whether the given name is an
 * indexed page or not.
 *
 * @param *file_name The name of the file, without extension
 * @return 1 if registered page, 0 otherwise
 */
int dir_is_page(char *file_name) {
	Dir *dir;
	int i;

	dir = g_cfg.dir;

	for (i = 0; i < dir->files_count; i++) {
		if (
		   (strcmp(dir->files[i]->name, file_name) == 0)
		&& (strcmp(dir->files[i]->extension, "page") == 0)
	) {
			return 1;
	  }
	}

	return 0;
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

	sfree(dir->path);

	for (i=0; i < dir->files_count; i++) {
		file_delete(dir->files[i]);
	}
	sfree(dir->files);

	sfree(dir);
}
