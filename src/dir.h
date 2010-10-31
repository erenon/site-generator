/*
 * @author erenon
 */

#ifndef DIR_H_
#define DIR_H_

/**
 * Max length of the dir path
 */
#define DIR_MAX_PATH_LENGTH 1024

/**
 * Represents a file on the filesystem
 */
typedef struct _File {
	char *name;		 /**< name of the file (extension excluded) */
	char *extension; /**< extension of the file */
	char *content; 	 /**< full content of the file */
} File;

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
} Dir;

Dir *dir_create(char path[]);
void dir_print(Dir *dir);
void dir_delete(Dir *dir);

#endif /* DIR_H_ */
