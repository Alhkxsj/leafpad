/*
 *  Leafpad - GTK+ based simple text editor
 *  Copyright (C) 2004-2025 Tarot Osuji
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "leafpad.h"
#include <string.h>
#include <stdio.h>

static GList *recent_files = NULL;

// Add file to recent list
void recent_add_file(const gchar *filename)
{
	GList *l;

	if (!filename || strlen(filename) == 0)
		return;

	// Remove if already exists
	for (l = recent_files; l; l = l->next) {
		if (strcmp((gchar *)l->data, filename) == 0) {
			g_free(l->data);
			recent_files = g_list_delete_link(recent_files, l);
			break;
		}
	}

	// Add to front
	recent_files = g_list_prepend(recent_files, g_strdup(filename));

	// Limit to MAX_RECENT_FILES
	while (g_list_length(recent_files) > MAX_RECENT_FILES) {
		GList *last = g_list_last(recent_files);
		g_free(last->data);
		recent_files = g_list_delete_link(recent_files, last);
	}
}

// Get recent files list
GList *recent_get_list(void)
{
	return recent_files;
}

// Clear recent files
void recent_clear(void)
{
	GList *l;
	for (l = recent_files; l; l = l->next) {
		g_free(l->data);
	}
	g_list_free(recent_files);
	recent_files = NULL;
}

// Save recent files to config
void recent_save(void)
{
	FILE *fp;
	gchar *path;
	GList *l;
	gint i = 0;

#if GLIB_CHECK_VERSION(2, 6, 0)
	path = g_build_filename(g_get_user_config_dir(), "leafpad", NULL);
	if (!g_file_test(path, G_FILE_TEST_IS_DIR)) {
# if GLIB_CHECK_VERSION(2, 8, 0)
		g_mkdir_with_parents(path, 0700);
# else
		g_mkdir(g_get_user_config_dir(), 0700);
		g_mkdir(path, 0700);
# endif
	}
	g_free(path);
	path = g_build_filename(g_get_user_config_dir(),
	    "leafpad", "recentrc", NULL);
#else
	path = g_build_filename(g_get_home_dir(), ".leafpad_recent", NULL);
#endif
	fp = fopen(path, "w");
	if (!fp) {
		g_free(path);
		return;
	}

	for (l = recent_files; l && i < MAX_RECENT_FILES; l = l->next, i++) {
		fprintf(fp, "%s\n", (gchar *)l->data);
	}

	fclose(fp);
	g_free(path);
}

// Load recent files from config
void recent_load(void)
{
	FILE *fp;
	gchar *path;
	gchar buf[PATH_MAX];
	gint i = 0;

#if GLIB_CHECK_VERSION(2, 6, 0)
	path = g_build_filename(g_get_user_config_dir(),
	    "leafpad", "recentrc", NULL);
#else
	path = g_build_filename(g_get_home_dir(), ".leafpad_recent", NULL);
#endif
	fp = fopen(path, "r");
	g_free(path);
	if (!fp)
		return;

	while (fgets(buf, sizeof(buf), fp) && i < MAX_RECENT_FILES) {
		// Remove trailing newline
		g_strstrip(buf);
		if (strlen(buf) > 0) {
			recent_files = g_list_append(recent_files, g_strdup(buf));
			i++;
		}
	}

	fclose(fp);
}