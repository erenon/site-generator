/*
 * genlib.h
 *
 *  Created on: 2010.10.29.
 *      Author: erenon
 */

#ifndef GENLIB_H_
#define GENLIB_H_

void *smalloc(size_t size);
void *sfree(void *pointer);
void *semalloc(size_t count, void *t);

typedef enum _STATUS {
	STATUS_CODE_SUCC,
	STATUS_CODE_FAILED
} STATUS;

#endif /* GENLIB_H_ */
