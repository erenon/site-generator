/*
 * generator.c
 *
 *  Created on: 2010.10.30.
 *      Author: erenon
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libgen.h"
#include "dir.h"

void format_text_replace_bb(char **text, char bbstart, char bbend, char *htmlstart, char *htmlend) {
	int i, start=0, tlength, addlen;
	char *newtext = NULL;

	tlength = strlen(*text);

	//additional length with each pair of html tags
	addlen = strlen(htmlstart)+ strlen(htmlend);

	for(i = 0; i < tlength; i++) {
		if ((*text)[i] == bbstart) {
			if (start) {
				//replace
				newtext = (char *)malloc((tlength+addlen-1) * sizeof(char));
				newtext[0] = '\0';
				strncat(newtext, *text, start);
				strcat(newtext, htmlstart);
				strncat(newtext, *text+start+1, i-start-1);
				strcat(newtext, htmlend);
				strcat(newtext, *text+i+1);

				//newtext[tlength+addlen-2] = '\0';

				sfree(*text);
				*text = (char *)malloc((strlen(newtext)+1) * sizeof(char));
				*text[0] = '\0';
				strcpy(*text, newtext);
				sfree(newtext);

				//reset
				tlength = strlen(*text);
				start = 0;
			} else {
				start = i;
			}
		}
	}
}

void format_text_italic(char **text) {
	format_text_replace_bb(
			text,
			'_',
			'_',
			"<em>",
			"</em>"
	);
}

void format_text_bold(char **text) {
	format_text_replace_bb(
		text,
		'*',
		'*',
		"<strong>",
		"</strong>"
	);
}

void format_text(char **text) {
	format_text_italic(text);
	format_text_bold(text);
}

void process_widget(File *widget) {
	printf("%s\n", widget->name);
	format_text(&widget->content);
	printf("%s\n", widget->content);
}

void dir_map_by_ext(Dir *dir, char *ext, void (*callback)(File *)) {
	int i;

	for (i=0; i < dir->files_count; i++) {
		if ( strcmp(dir->files[i]->extension, ext) == 0) {
			callback(dir->files[i]);
		}
	}
}

void generator_process_widgets(Dir *dir) {
	//void *callback = process_widget;
	dir_map_by_ext(dir, "widget", process_widget);
}
