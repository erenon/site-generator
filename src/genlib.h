/*
 * @author erenon
 */

#ifndef GENLIB_H_
#define GENLIB_H_

/**
 * Max length of the dir path
 */
#define MAX_PATH_LENGTH 1024

void *smalloc(size_t size);
void sfree(void *pointer);

/**
 * Status
 */
typedef enum _STATUS {
	STATUS_SUCC,
	STATUS_CODE_FAILED
} STATUS;

#include "dir.h"

typedef struct _CONFIG {
	Dir *dir;
	char img_dir[MAX_PATH_LENGTH];
} CONFIG;

/**
 * GLOBAL variable
 */
CONFIG g_cfg;

#endif /* GENLIB_H_ */
