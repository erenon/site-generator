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
			file->name = semalloc(i, file->name);
			strncpy(file->name, fname, i);
			file->name[i+1] = '\0';

			//copy extension
			file->extension = semalloc(len-i-1, file->extension);
			strcpy(file->extension, fname+i+1);

			return;
		}
	}

	//no . found
	file->name = semalloc(len, file->name);
	strcpy(file->name, fname);

	file->extension = semalloc(1, file->extension);
	file->extension[0] = '\0';

	return;
}

void read_file(File *file, char fname[]) {
	FILE *fp;
	int filesize;

	//hack! trim newline;
	fname[strlen(fname)-1] = '\0';

	//TODO add error h
	fp = fopen(fname, "r");

	//get file length
	fseek(fp, 0, SEEK_END);
	//TODO +1 for closing 0?
	filesize = ftell(fp)+1;

	file->content = semalloc(filesize, file->content);

	rewind(fp);
	fread(file->content, filesize-1, 1, fp);

	file->content[filesize] = '\0';

	parse_fname(file, fname);

}

void read(Dir *dir) {
	FILE *index;
	char file_name[DIR_MAX_LINE_LENGTH];
	int linec=0, i;
	char dir_index[] = "index";

	//TODO handle path

	//TODO add error h
	index = fopen(dir_index, "r");

	while (fgets(file_name, DIR_MAX_LINE_LENGTH, index) != NULL) {
		linec++;
	}

	rewind(index);

	dir->file_to_process_count = linec;
	dir->files = semalloc(linec, dir->files);

	i=0;
	while (fgets(file_name, DIR_MAX_LINE_LENGTH, index) != NULL) {
		dir->files[i] = (File *)semalloc(1, dir->files[i]);
		read_file(dir->files[i], file_name);
		i++;
	}


	fclose(index);
}

Dir *dir_create(char path[]) {
	Dir *dir = NULL;

	dir = semalloc(1, dir);

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

	sfree(dir);
}
