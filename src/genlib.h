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
 * Represents the resoult of a function call.
 */
typedef enum _STATUS {
	STATUS_SUCC,		/**< If the call was successful */
	STATUS_FAILED  /**< If the call has encountered a problem */
} STATUS;

#include "dir.h"

/**
 * Configuration struct
 * Contains all the global configuration fields
 * needed by the application.
 */
typedef struct _CONFIG {
	Dir *dir;	/**< Pointer to the current directory */
	char img_dir[MAX_PATH_LENGTH];	/**< Path of the separate image directory */
} CONFIG;

/**
 * GLOBAL variable
 * This global configuration variable holds
 * user defined configuration entries
 */
CONFIG g_cfg;

#endif /* GENLIB_H_ */
