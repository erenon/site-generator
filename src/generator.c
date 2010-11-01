/*
 * @author erenon
 *
 * @todo a csereberélő fgvények megfektethetőek, ha
 * a csere is tartalmaz cserélendőt. (?)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "genlib.h"
#include "dir.h"

/**
 * Replaces paired bb code with html
 * int the given text. The change is in place.
 *
 * @param[in,out] **text Source and target in the same time
 * @param[in] bbstart Opening tag to replace
 * @param[in] bbend Closing tag to replace
 * @param[in] *htmlstart Opening html tag
 * @param[in] *htmlend Closing html tag
 */
static void format_text_replace_bb(char **text, char bbstart, char bbend, char *htmlstart, char *htmlend) {
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

/**
 * Makes emphasized text from underscore delimited text
 *
 * @param **text Source to search in
 *
 * @see format_text_replace_bb
 */
static void format_text_italic(char **text) {
	format_text_replace_bb(
			text,
			'_',
			'_',
			"<em>",
			"</em>"
	);
}

/**
 * Makes bold text from star (*) delimited text
 *
 * @param **text Source to search in
 *
 * @see format_text_replace_bb
 */
static void format_text_bold(char **text) {
	format_text_replace_bb(
		text,
		'*',
		'*',
		"<strong>",
		"</strong>"
	);
}

/**
 * Replaces bb coded links to html links.
 *
 * @param **text Source to search in
 *
 * @example format_text_link.c
 *
 * @todo handle local and remote links
 */
static void format_text_link(char **text) {
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

/**
 * Replaces bb coded img tags to html tags.
 *
 * @param **text Source to search in
 *
 * @todo handle predefined separated img dir
 */
static void format_text_img(char **text) {
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

/**
 * Applies the available text format methods to
 * the given text.
 *
 * @param **text
 */
static void format_text(char **text) {
	format_text_italic(text);
	format_text_bold(text);
	format_text_link(text);
	format_text_img(text);
}

/**
 * Process the given file as a widget
 *
 * @param *widget file to process
 */
static void process_widget(File *widget) {
	format_text(&widget->content);
}

/**
 * @todo use enum instead of magic numbers
 */
static int has_layout(Dir *dir) {
	int i;
	if (dir->layout_index == -2) {
		dir->layout_index = -1;

		for(i=0; i < dir->files_count; i++) {
			if (strcmp(dir->files[i]->name, "layout") == 0) {
				dir->layout_index = i;
			}
		}
	}

	return (dir->layout_index >= 0) ? 1 : 0;
}

static void replace_placeholder(char **layout, char *placeholder, char *replaceto) {
	int i, tlen, placelen, addlen;
	char *newlayout = NULL;

	tlen = strlen(*layout);
	placelen = strlen(placeholder);
	addlen = strlen(replaceto) - placelen;

	for (i=0; i < tlen; i++) {
		if ((*layout)[i] == placeholder[0]) {
			if (memcmp(*layout+i, placeholder, placelen) == 0) {
				//replace
				newlayout = (char *)smalloc((tlen+addlen+1) * sizeof(char));
				newlayout[0] = '\0';

				strncat(newlayout, *layout, i);
				strcat(newlayout, replaceto);
				strcat(newlayout, *layout+i+placelen);

				sfree(*layout);
				*layout = newlayout;
				newlayout = NULL;

				//reset
				tlen = strlen(*layout);
			}
		}
	}
}

/*
 * @todo handle {navigation}
 */
static void process_layout(Dir *dir) {
	int i;
	char **layout;
	char *placeholder;

	if (has_layout(dir)) {
		layout = &(dir->files[dir->layout_index])->content;
	} else {
		return;
	}

	format_text_link(layout);
	format_text_img(layout);

	for (i=0; i < dir->files_count; i++) {
		if (strcmp(dir->files[i]->extension, "widget") == 0) {
			placeholder = (char *)smalloc(
					(strlen("{widget:")+strlen(dir->files[i]->name)+2) *
					sizeof(char)
			);
			strcpy(placeholder, "{widget:");
			strcat(placeholder, dir->files[i]->name);
			strcat(placeholder, "}");

			replace_placeholder(
					layout,
					placeholder,
					dir->files[i]->content
			);

			sfree(placeholder);
		}
	}
}

static void process_page(File *page) {
	format_text(&page->content);
}

/**
 * Iterates over the given dir
 * and applies the given callback to the file
 * if it's extension matches to the given extension.
 *
 * @param *dir directory to iterate over
 * @param *ext extension to look for
 * @param *callback callback function
 */
static void dir_map_by_ext(Dir *dir, char *ext, void (*callback)(File *)) {
	int i;

	for (i=0; i < dir->files_count; i++) {
		if ( strcmp(dir->files[i]->extension, ext) == 0) {
			callback(dir->files[i]);
		}
	}
}

/**
 * Processes all the widget files in the given dir.
 *
 * @param *dir
 */
void generator_process_widgets(Dir *dir) {
	dir_map_by_ext(dir, "widget", process_widget);
}

void generator_process_layout(Dir *dir) {
	process_layout(dir);
}

void generator_process_pages(Dir *dir) {
	dir_map_by_ext(dir, "page", process_page);
}
