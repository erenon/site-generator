/*
 * dir.h
 *
 *  Created on: 2010.10.24.
 *      Author: erenon
 */

#ifndef DIR_H_
#define DIR_H_

typedef struct Dir Dir;
typedef struct File File;

Dir *dir_create(char path[]);
Dir dir_sort(Dir *dir);
void dir_print(Dir *dir);
void dir_delete(Dir *dir);

#endif /* DIR_H_ */
