/*
 * @author erenon
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
 * @param[in] bbsep BBCode separator
 * @param[in] *htmlstart Opening html tag
 * @param[in] *htmlend Closing html tag
 *
 */
static void format_text_replace_bb(char **text, char bbsep, char *htmlstart, char *htmlend) {
	int i, start=-1, tlength, addlen;
	char *newtext = NULL;

	tlength = strlen(*text);

	/*additional length with each pair of html tags*/
	addlen = strlen(htmlstart)+ strlen(htmlend);

	for(i = 0; i < tlength; i++) {
		if ((*text)[i] == bbsep) {
			if (start != -1) {
				/*replace*/
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

				/*reset*/
				tlength = strlen(*text);
				start = -1;
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
 */
static void format_text_link(char **text) {
	char *bbss = "[link:",
		 bbse = ']',
		 *bbe = "[/l]",
		 *htmlss = "<a href=\"",
		 *prefix = "http://",
		 *suffix = ".html",
		 *htmlse = "\">",
		 *htmle = "</a>",
		 *newtext = NULL;
	char target[DIR_MAX_LINE_LENGTH];
	int ss = 0, se = 0, i,
		bbsslen, bbelen, htmlsslen, htmlselen, htmlelen,
		tlen, addlen, prefixlen, suffixlen, cfixlen, is_local_page;

	bbsslen = strlen(bbss);
	bbelen = strlen(bbe);

	htmlsslen = strlen(htmlss);
	prefixlen = strlen(prefix);
	suffixlen = strlen(suffix);
	htmlselen = strlen(htmlse);
	htmlelen = strlen(htmle);

	tlen = strlen(*text);
	addlen = htmlsslen + htmlselen + htmlelen - bbsslen - 1 - bbelen;

	for (i=0; i < tlen; i++) {
		if (
			!(ss != 0) &&
		    memcmp(*text+i, bbss, bbsslen) == 0
		) {
			/*opening tag first part found*/
			ss = i;
		} else if (
			(ss != 0) &&
			(se == 0) &&
			(*text)[i] == bbse
		) {
			/*end of the opening tag*/
			se = i;
		} else if (
			(ss != 0) &&
			(se != 0) &&
			memcmp(*text+i, bbe, bbelen) == 0
		) {
			/*closing tag found*/

			/*is local?*/
			target[0] = '\0';
			strncat(target, *text+ss+bbsslen, se-ss-bbsslen);
			is_local_page = dir_is_page(target);

			/*if local, add .html, else add http:// */
			cfixlen = (is_local_page) ? suffixlen : prefixlen;

			/*replace*/
			newtext = (char *)smalloc(
					(tlen + addlen + cfixlen + 1) * sizeof(char *)
			);
			newtext[0] = '\0';

			strncat(newtext, *text, ss);
			strcat(newtext, htmlss);
			/*add http:// if remote target*/
			if (!is_local_page) { strcat(newtext, prefix); }
			strncat(newtext, *text+ss+bbsslen, se-ss-bbsslen);
			/*add .html if local target*/
			if (is_local_page) { strcat(newtext, suffix); }
			strcat(newtext, htmlse);
			strncat(newtext, *text+se+1, i-se-1);
			strcat(newtext, htmle);
			strcat(newtext, *text+i+bbelen);

			newtext[tlen + addlen + cfixlen] = '\0';

			sfree(*text);
			*text = newtext;
			newtext = NULL;

			/*reset*/
			ss = se = 0;
			tlen = strlen(*text);
		}
	}
}

/**
 * Replaces bb coded img tags to html tags.
 *
 * @param **text Source to search in
 */
static void format_text_img(char **text) {
	char *bbs = "[img:",
		 bbe = ']',
		 *htmls,
		 *htmle = "\" />",
		 *newtext = NULL;
	int i, bbslen, htmlslen, htmlelen, tlen, addlen,
		start;

	htmls = (char *)smalloc(
			(strlen("<img src=\"") + strlen(g_cfg.img_dir) + 1) *
			sizeof(char)
	);

	strcpy(htmls, "<img src=\"");
	strcat(htmls, g_cfg.img_dir);

	bbslen = strlen(bbs);
	htmlslen = strlen(htmls);
	htmlelen = strlen(htmle);
	tlen = strlen(*text);

	addlen = bbslen - 1 + htmlslen + htmlelen;

	start = -1;

	for (i = 0; i < tlen; i++) {
		if (
			(start == -1) &&
			memcmp(*text+i, bbs, bbslen) == 0
		) {
			/*opening tag found*/
			start = i;
		} else if (
			(start != -1) &&
			(*text)[i] == bbe
		) {
			/*closing tag*/
			/*replace*/
			newtext = (char *)smalloc((tlen + addlen) * sizeof(char *));
			newtext[0] = '\0';

			strncat(newtext, *text, start);
			strcat(newtext, htmls);
			strncat(newtext, *text+start+bbslen, i-start-bbslen);
			strcat(newtext, htmle);
			strcat(newtext, *text+i+1);

			newtext[tlen + addlen - 1] = '\0';

			sfree(*text);
			*text = newtext;
			newtext = NULL;

			/*reset*/
			start = -1;
			tlen = strlen(*text);
		}
	}

	sfree(htmls);
}

static void format_text_header(char **text) {
	char *h2 = "<h2>", *h2e = "</h2>";
	char *h3 = "<h3>", *h3e = "</h3>";
	char *ch = NULL, *che = NULL;
	char *newtext = NULL;
	int i, lastnl, addlen, tlen;
	const int bblen = 4;

	tlen = strlen(*text);
	/*assuming the length of the header tags are the same*/
	addlen = strlen(h2) + strlen(h2e) - bblen;

	lastnl = 0;
	for (i=0; i < tlen-3; i++) {
		if (memcmp(*text+i, "\n===", bblen) == 0) {
			ch = h2;
			che = h2e;
		} else if (memcmp(*text+i, "\n---", bblen) == 0){
			ch = h3;
			che = h3e;
		}

		if (ch != NULL && che != NULL) {
			newtext = (char *)smalloc((tlen + addlen +1) * sizeof(char));
			newtext[0] = '\0';

			/*replace*/
			strncat(newtext, *text, lastnl);
			strcat(newtext, ch);
			strncat(newtext, *text+lastnl, i-lastnl);
			strcat(newtext, che);
			strcat(newtext, *text+i+bblen);

			sfree(*text);
			*text = newtext;
			newtext = NULL;

			/*reset*/
			ch = NULL;
			che = NULL;
			tlen = strlen(*text);
		} else if ((*text)[i] == '\n') {
			lastnl = i;
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
	format_text_header(text);
}

/**
 * Determines whether the given dir has a
 * layout or doesn't. Searches for the layout,
 * if the dir's layout index is UNINITIALIZED.
 *
 * @param *dir
 * @return 1 if layout found, 0 otherwise
 */
static int has_layout(Dir *dir) {
	int i;
	if (dir->layout_index == UNINITIALIZED) {
		dir->layout_index = NO_LAYOUT;

		for(i=0; i < dir->files_count; i++) {
			if (strcmp(dir->files[i]->name, "layout") == 0) {
				dir->layout_index = i;
			}
		}
	}

	return (dir->layout_index >= 0) ? 1 : 0;
}

/**
 * Replaces the given placeholder to the given replacement
 * in the given layout.
 *
 * @param[in,out] **layout Text to search in
 * @param[in] *placeholder to replace
 * @param[in] *replaceto replacement
 */
static void replace_placeholder(char **layout, char *placeholder, char *replaceto) {
	int i, tlen, placelen, addlen;
	char *newlayout = NULL;

	tlen = strlen(*layout);
	placelen = strlen(placeholder);
	addlen = strlen(replaceto) - placelen;

	for (i=0; i < tlen; i++) {
		if ((*layout)[i] == placeholder[0]) {
			if (memcmp(*layout+i, placeholder, placelen) == 0) {
				/*replace*/
				newlayout = (char *)smalloc((tlen+addlen+1) * sizeof(char));
				newlayout[0] = '\0';

				strncat(newlayout, *layout, i);
				strcat(newlayout, replaceto);
				strcat(newlayout, *layout+i+placelen);

				sfree(*layout);
				*layout = newlayout;
				newlayout = NULL;

				/*reset*/
				tlen = strlen(*layout);

				/*skip included content*/
				i += strlen(replaceto);
			}
		}
	}
}

/**
 * Calculates the total length of a navigation item
 * based on the given name.
 *
 * @warning This function contains magic numbers defined by get_navigation_item.
 * The length of the wrapper tags may change!
 *
 * @see get_navigation_item
 *
 * @param *page_name The name of the item (occurs twice)
 * @return length of the navigation item
 */
static int get_navigation_item_length(char *page_name) {
	return 13 + strlen(page_name)*2 + 7 + 9;
}

/**
 * Creates a navigation item based on the given name
 *
 * Suffixes the name with .html if the given name matches
 * to a local page. Prefixes the name with http:// if the
 * name points to a remote target.
 *
 * @verbatim
Example input: about_me
Example output: <li><a href="about_me.html">about_me</a></li>
@endverbatim
 *
 * @param *page_name The name of the item
 * @return The assembled navigation item
 */
static char *get_navigation_item(char *page_name) {
	char *itemss = "<li><a href=\"";
	char *itemse = ".html\">";
	char *iteme = "</a></li>";
	char *item;

	int sslen, selen, elen, namelen;

	sslen = strlen(itemss);
	selen = strlen(itemse);
	elen = strlen(iteme);
	namelen = strlen(page_name);

	item = (char *)smalloc(
			(sslen + selen + elen + namelen*2 + 1) *
			sizeof(char)
	);

	strcpy(item, itemss);
	strcat(item, page_name);
	strcat(item, itemse);
	strcat(item, page_name);
	strcat(item, iteme);

	return item;
}

/**
 * Searches for {navigation} placeholder and
 * replaces it to a navigation list in the given text.
 *
 * @see get_navigation_item_length
 * @see get_navigation_item
 * @see replace_placeholder
 *
 * @param **text Text to search in
 */
static void embed_navigation(char **text) {
	char *navboxs = "<ul>", *navboxe = "</ul>", *navigation, *item;
	int i, navboxlen, itemslen;

	navboxlen = strlen(navboxs) + strlen(navboxe);

	/*measure items length*/
	itemslen = 0;
	for (i=0; i < g_cfg.dir->files_count; i++) {
		if (dir_is_page(g_cfg.dir->files[i]->name)) {
			itemslen += get_navigation_item_length(
					g_cfg.dir->files[i]->name
			);
		}
	}

	/*allocate navigation*/
	navigation = (char *)smalloc((itemslen+navboxlen+1) * sizeof(char));

	strcpy(navigation, navboxs);
	/*embed items*/
	for (i=0; i < g_cfg.dir->files_count; i++) {
		if (dir_is_page(g_cfg.dir->files[i]->name)) {
			item = get_navigation_item(g_cfg.dir->files[i]->name);
			strcat(navigation, item);
			sfree(item);
		}
	}

	strcat(navigation, navboxe);

	replace_placeholder(text, "{navigation}", navigation);

	sfree(navigation);
}

/*
 * Generates the backbone of the pages.
 *
 * Formats the links, image references,
 * embeds navigation, and widgets
 *
 * @param *dir Container dir of the layout to process
 */
static void process_layout(Dir *dir) {
	int i;
	char **layout;
	char *placeholder;

	if (has_layout(dir) != 0) {
		layout = &(dir->files[dir->layout_index])->content;
	} else {
		return;
	}

	format_text_link(layout);
	format_text_img(layout);
	embed_navigation(layout);

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

/**
 * Copies the given content into the given layout
 * at the position of the {content} placeholder,
 * and stores the resoult in the content.
 * This way the content will be embeded into the layout
 * and no modification will be done on the layout string.
 *
 * @param **content The content to wrap
 * @param *layout The layout to embed into
 */
static void embed_into_layout(char **content, char *layout) {
	char *placeholder = "{content}", *newcontent = NULL;
	int i, laylen, placelen, addlen;

	laylen = strlen(layout);
	placelen = strlen(placeholder);
	addlen = strlen(*content) - placelen;

	for (i=0; i < laylen; i++) {
		if(layout[i] == placeholder[0]) {
			if (memcmp(layout+i, placeholder, placelen) == 0) {
				/*replace*/
				newcontent = (char *)smalloc((laylen+addlen+1) * sizeof(char));
				newcontent[0] = '\0';

				strncat(newcontent, layout, i);
				strcat(newcontent, *content);
				strcat(newcontent, layout+i+placelen);

				sfree(*content);
				*content = newcontent;

				return;
			}
		}
	}
}

/**
 * Processes all the widget files in the given dir.
 *
 * @see format_text
 *
 * @param *dir
 */
void generator_process_widgets(Dir *dir) {
	int i;

	for (i=0; i < dir->files_count; i++) {
		if (strcmp(dir->files[i]->extension, "widget") == 0) {
			format_text(&dir->files[i]->content);
		}
	}
}

/**
 * Simple exposed wrapper around process_layout.
 *
 * @see process_layout
 *
 * @param *dir Directory that contains the layout to process
 */
void generator_process_layout(Dir *dir) {
	process_layout(dir);
}

/**
 * Processes all the pages in the given directory
 *
 * @see format text
 * @see embed_into_layout
 *
 * @param *dir Directory to process
 */
void generator_process_pages(Dir *dir) {
	int i;
	char *layout = NULL;

	if (has_layout(dir) != 0) {
		layout = dir->files[dir->layout_index]->content;
	}

	for (i=0; i < dir->files_count; i++) {
		if ( strcmp(dir->files[i]->extension, "page") == 0) {
			/*format*/
			format_text(&(dir->files[i])->content);

			/*embed into layout*/
			if (layout != NULL) {
				embed_into_layout(
					&(dir->files[i])->content,
					layout
				);
			}
		}
	}
}
