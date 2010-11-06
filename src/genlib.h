/*
 * @author erenon
 */

#ifndef GENLIB_H_
#define GENLIB_H_

void *smalloc(size_t size);
void *sfree(void *pointer);
void *semalloc(size_t count, void *t);

/**
 * Status
 */
typedef enum _STATUS {
	STATUS_SUCC,
	STATUS_CODE_FAILED
} STATUS;

#endif /* GENLIB_H_ */
