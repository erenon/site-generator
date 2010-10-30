/*
 * dir.h
 *
 *  Created on: 2010.10.24.
 *      Author: erenon
 */

#ifndef DIR_H_
#define DIR_H_

typedef struct _File {
	char *name;
	char *extension;
	char *content;
} File;

typedef struct _Dir {
	//char *path;
	int file_to_process_count;
	int files_count;
	File **files;
} Dir;



Dir *dir_create(char path[]);
Dir dir_sort(Dir *dir);
void dir_print(Dir *dir);
void dir_delete(Dir *dir);

#endif /* DIR_H_ */
