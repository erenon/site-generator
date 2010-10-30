/*
 * dir.c
 *
 *  Created on: 2010.10.24.
 *      Author: erenon
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "genlib.h"
#include "dir.h"

#define DIR_MAX_LINE_LENGTH 1024

struct File {
	char *name;
	char *extension;
	char *content;
};

struct Dir {
	//char *path;
	int file_to_process_count;
	File **files;
};

void parse_fname(File *file, char fname[]) {
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

	//no . found
	file->name = (char *)smalloc((len+1) * sizeof(char));
	strcpy(file->name, fname);
	file->name[len] = '\0';

	file->extension = (char *)smalloc(1 * sizeof(char));
	file->extension[0] = '\0';

	return;
}

void read_file(File *file, char fname[], STATUS *status) {
	FILE *fp = NULL;
	int filesize;

	//hack! trim newline;
	fname[strlen(fname)-1] = '\0';

	//TODO add error h
	fp = fopen(fname, "r");
	if (fp == NULL) {
		fprintf(stderr, "Failed to read file: '%s', file skipped.\n", fname);
		*status = STATUS_CODE_FAILED;
		return;
	}

	//get file length
	fseek(fp, 0, SEEK_END);
	filesize = ftell(fp);

	file->content = (char *)smalloc(filesize+1 * sizeof(char));

	if (filesize > 0) {
		rewind(fp);
		fread(file->content, filesize, 1, fp);
	}

	file->content[filesize] = '\0';

	parse_fname(file, fname);

	fclose(fp);

	*status = STATUS_CODE_SUCC;
}

void read(Dir *dir) {
	FILE *index = NULL;
	char file_name[DIR_MAX_LINE_LENGTH];
	int linec=0, i;
	char dir_index[] = "index";
	STATUS status;

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
		status = STATUS_CODE_FAILED;

		dir->files[i] = (File *)smalloc(sizeof(File));

		read_file(dir->files[i], file_name, &status);
		if (status == STATUS_CODE_SUCC) {
			i++;
		} else {
			sfree(dir->files[i]);
		}
	}


	fclose(index);
}

Dir *dir_create(char path[]) {
	Dir *dir = NULL;

	dir = (Dir *)smalloc(sizeof(Dir));

	read(dir);

	return dir;
}

/**
 * debug/test function
 * print dir contents
 */
void dir_print(Dir *dir) {
	int i;

	//printf("path: %s\nfiles:\n\n", dir->path);

	for (i=0;i<dir->file_to_process_count;i++) {
		printf("filename: %s\n", dir->files[i]->name);
		printf("ext: %s\n", dir->files[i]->extension);
		printf("%s\n\n",dir->files[i]->content);
	}
}

void file_delete(File *file) {
	sfree(file->name);
	sfree(file->extension);
	sfree(file->content);
	sfree(file);
}

void dir_delete(Dir *dir) {
	int i;

	for (i=0; i < dir->file_to_process_count; i++) {
		file_delete(dir->files[i]);
	}
	sfree(dir->files);

	sfree(dir);
}
