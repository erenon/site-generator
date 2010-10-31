/*
 * generator.c
 *
 *  Created on: 2010.10.30.
 *      Author: erenon
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "genlib.h"
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
				newtext = (char *)smalloc((tlength+addlen-1) * sizeof(char));
				newtext[0] = '\0';
				strncat(newtext, *text, start);
				strcat(newtext, htmlstart);
				strncat(newtext, *text+start+1, i-start-1);
				strcat(newtext, htmlend);
				strcat(newtext, *text+i+1);

				sfree(*text);
				*text = newtext;
				newtext = NULL;

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

void format_text_link(char **text) {
	char *bbss = "[link:",
		 bbse = ']',
		 *bbe = "[/l]",
		 *htmlss = "<a href=\"",
		 *htmlse = "\">",
		 *htmle = "</a>",
		 *newtext = NULL;
	int ss = 0, se = 0, i,
		bbsslen, bbelen, htmlsslen, htmlselen, htmlelen,
		tlen, addlen;

	bbsslen = strlen(bbss);
	bbelen = strlen(bbe);

	htmlsslen = strlen(htmlss);
	htmlselen = strlen(htmlse);
	htmlelen = strlen(htmle);

	tlen = strlen(*text);
	addlen = htmlsslen + htmlselen + htmlelen - bbsslen - 1 - bbelen;

	for (i=0; i < tlen; i++) {
		if (
			!ss &&
		    memcmp(*text+i, bbss, bbsslen) == 0
		) {
			//opening tag first part found
			ss = i;
		} else if (
			ss &&
			!se &&
			(*text)[i] == bbse
		) {
			//end of the opening tag
			se = i;
		} else if (
			ss &&
			se &&
			memcmp(*text+i, bbe, bbelen) == 0
		) {
			//closing tag found
			//replace
			newtext = (char *)smalloc((tlen + addlen) * sizeof(char *));
			newtext[0] = '\0';

			strncat(newtext, *text, ss);
			strcat(newtext, htmlss);
			strncat(newtext, *text+ss+bbsslen, se-ss-bbsslen);
			strcat(newtext, htmlse);
			strncat(newtext, *text+se+1, i-se-1);
			strcat(newtext, htmle);
			strcat(newtext, *text+i+bbelen);

			newtext[tlen + addlen - 1] = '\0';

			sfree(*text);
			*text = newtext;
			newtext = NULL;

			//reset
			ss = se = 0;
			tlen = strlen(*text);
		}
	}
}

void format_text_img(char **text) {
	char *bbs = "[img:",
		 bbe = ']',
		 *htmls = "<img src=\"",
		 *htmle = "\" />",
		 *newtext = NULL;
	int i, bbslen, htmlslen, htmlelen, tlen, addlen,
		start;

	bbslen = strlen(bbs);
	htmlslen = strlen(htmls);
	htmlelen = strlen(htmle);
	tlen = strlen(*text);

	addlen = bbslen - 1 + htmlslen + htmlelen;

	start = 0;

	for (i = 0; i < tlen; i++) {
		if (
			!start &&
			memcmp(*text+i, bbs, bbslen) == 0
		) {
			//opening tag found
			start = i;
		} else if (
			start &&
			(*text)[i] == bbe
		) {
			//closing tag
			//replace
			newtext = (char *)smalloc((tlen + addlen) * sizeof(char *));
			newtext[0] = '\0';

			strncat(newtext, *text, start);
			strcat(newtext, htmls);
			strncat(newtext, *text+start+bbslen, i-start-bbslen-1);
			strcat(newtext, htmle);
			strcat(newtext, *text+i+1);

			newtext[tlen + addlen - 1] = '\0';

			sfree(*text);
			*text = newtext;
			newtext = NULL;

			//reset
			start = 0;
			tlen = strlen(*text);
		}
	}

}

void format_text(char **text) {
	format_text_italic(text);
	format_text_bold(text);
	format_text_link(text);
	format_text_img(text);
}

void process_widget(File *widget) {
	printf("%s\n", widget->name);
	format_text(&widget->content);
	printf("%s\n\t===\t\n", widget->content);
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
	dir_map_by_ext(dir, "widget", process_widget);
}
