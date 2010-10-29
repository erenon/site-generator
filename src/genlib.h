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

#endif /* GENLIB_H_ */
