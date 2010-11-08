/*
 * @author erenon
 *
 * @todo wrap ifdebug around dir_print
 */

#ifndef DIR_H_
#define DIR_H_

/**
 * Represents a file on the filesystem
 */
typedef struct _File {
	char *name;		 /**< name of the file (extension excluded) */
	char *extension; /**< extension of the file */
	char *content; 	 /**< full content of the file */
} File;

typedef enum _DIR_LAYOUT {
	UNINITIALIZED = -2,
	NO_LAYOUT = -1
} DIR_LAYOUT;

/**
 * Represents a directory on the filesystem
 * Should contain an index file
 * listing the contained file to read
 */
typedef struct _Dir {
	char *path;					/**< path to read from, relative to the execution */
	int file_to_process_count;	/**< line count of the index file */
	int files_count;		    /**< found indexed file count */
	File **files;				/**< pointers to the read files */
	int layout_index;			/**< index of the layout file in the files array */
} Dir;

#include "genlib.h"

Dir *dir_create(char path[], STATUS *s);
void dir_print(Dir *dir);
void dir_delete(Dir *dir);
void dir_map_by_ext(Dir *dir, char *ext, void (*callback)(File *));
STATUS dir_write(Dir *dir, char *path);

#endif /* DIR_H_ */
