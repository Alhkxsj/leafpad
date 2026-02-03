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