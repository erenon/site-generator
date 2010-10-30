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
	char *path;
	int file_to_process_count;
	File **files;
};

void process_fname(File *file, char fname[]) {

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

	process_fname(file, fname);

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

	printf("path: %s\nfiles:\n\n", dir->path);

	for (i=0;i<dir->file_to_process_count;i++) {
		//printf("filename: %s\n", dir->files[i]->name);
		printf("%s\n\n",dir->files[i]->content);
	}
}
