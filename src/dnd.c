/*
 *  Leafpad - GTK+ based simple text editor
 *  Copyright (C) 2004-2006 Tarot Osuji
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
#include "string.h"

#define DV(x)

static void dnd_drag_data_recieved_handler(GtkWidget *widget,
	GdkDragContext *context, gint x, gint y,
	GtkSelectionData *selection_data, guint info, guint time);
static gboolean dnd_drag_motion_handler(GtkWidget *widget,
	GdkDragContext *context, gint x, gint y, guint time);

enum {
	TARGET_SELF,
	TARGET_UTF8_STRING,
	TARGET_COMPOUND_TEXT,
	TARGET_PLAIN,
	TARGET_URI_LIST,
};

static GtkTargetEntry drag_types[] =
{
#if !GTK_CHECK_VERSION(2, 10, 0)
	{ "GTK_TEXT_BUFFER_CONTENTS", GTK_TARGET_SAME_WIDGET, TARGET_SELF },
#endif
	{ "UTF8_STRING", 0, TARGET_UTF8_STRING },
	{ "COMPOUND_TEXT", 0, TARGET_COMPOUND_TEXT },
	{ "text/plain", 0, TARGET_PLAIN },
	{ "text/uri-list", 0, TARGET_URI_LIST }
};

static gint n_drag_types = sizeof(drag_types) / sizeof(drag_types[0]);

void dnd_init(GtkWidget *widget)
{
	gtk_drag_dest_set(widget, GTK_DEST_DEFAULT_ALL,
		drag_types, n_drag_types, GDK_ACTION_COPY);
	g_signal_connect(G_OBJECT(widget), "drag_data_received",
		G_CALLBACK(dnd_drag_data_recieved_handler), NULL);
	g_signal_connect(G_OBJECT(widget), "drag_motion",
		G_CALLBACK(dnd_drag_motion_handler), NULL);
}


static void dnd_open_first_file(gchar *filename)
{
	FileInfo *fi;
	
	if (check_text_modification())
		return;
	fi = g_malloc(sizeof(FileInfo));
	fi->filename = g_strdup(filename);
	fi->charset = pub->fi->charset_flag ? g_strdup(pub->fi->charset) : NULL;
	fi->charset_flag = pub->fi->charset_flag;
	fi->lineend = LF;
	if (file_open_real(pub->mw->view, fi))
		g_free(fi);
	else {
		g_free(pub->fi);
		pub->fi = fi;
		undo_clear_all(pub->mw->buffer);
		set_main_window_title();
	}
}	

static void dnd_drag_data_recieved_handler(GtkWidget *widget,
	GdkDragContext *context, gint x, gint y,
	GtkSelectionData *selection_data, guint info, guint time)
{
	static gboolean flag_called_once = FALSE;
	static guint last_time = 0;
	gchar **files;
	gchar *filename;
	gchar *comline;
	gint i = 0, j = 0;
	gchar *filename_sh;
	gchar **strs;
#ifdef ENABLE_CSDI
	j = 1;
#endif
DV(g_print("DND start!\n"));
	
	// Prevent duplicate processing of the same drag event
	if (time == last_time) {
		last_time = 0;
		g_signal_stop_emission_by_name(widget, "drag_data_received");
		return;
	}
	last_time = time;
	
#if GTK_CHECK_VERSION(2, 10, 0)
	if (g_ascii_strcasecmp(gdk_atom_name(context->targets->data),
	    "GTK_TEXT_BUFFER_CONTENTS") != 0) {
#else
	if (info != TARGET_SELF) {
#endif
		if (flag_called_once) {
			flag_called_once = FALSE;
			g_signal_stop_emission_by_name(widget, "drag_data_received");
			return;
		} else
			flag_called_once = TRUE;
	}
	
	if (selection_data->data && info == TARGET_URI_LIST) {
		files = g_strsplit((gchar *)selection_data->data, "\n" , -1);
		while (files[i]) {
			if (strlen(files[i]) == 0)
				break;
			filename = g_strstrip(parse_file_uri(files[i]));
			if (i + j == 0)
				dnd_open_first_file(filename);
			else {
				if (i + j == 1)
					save_config_file();
				if (strstr(filename, " ")) {
					strs = g_strsplit(filename, " ", -1);
					filename_sh = g_strjoinv("\\ ", strs);
					g_strfreev(strs);
				} else
					filename_sh = g_strdup(filename);
				comline = g_strdup_printf("%s %s", PACKAGE, filename_sh);
DV(g_print(">%s\n", comline));
				g_free(filename_sh);
				g_spawn_command_line_async(comline, NULL);
				g_free(comline);
			}
			g_free(filename);
			i++;
		}
		g_strfreev(files);
	}
	else {
		clear_current_keyval();
		undo_set_sequency(FALSE);
#if GTK_CHECK_VERSION(2, 10, 0)
		if (info == TARGET_UTF8_STRING) {
#else
		if (info == TARGET_SELF) {
#endif
			undo_set_sequency_reserve();
			context->action = GDK_ACTION_MOVE;
		} else if (info == TARGET_PLAIN 
			&& g_utf8_validate((gchar *)selection_data->data, -1, NULL)) {
			selection_data->type =
				gdk_atom_intern("UTF8_STRING", FALSE);
		}
	}
	
	return;
}

static gboolean dnd_drag_motion_handler(GtkWidget *widget,
	GdkDragContext *context, gint x, gint y, guint time)
{
	GList *targets;
	gchar *name;
	gboolean flag = FALSE;
	
	targets = context->targets;
	while (targets) {
		name = gdk_atom_name(targets->data);
		if (g_ascii_strcasecmp(name, "text/uri-list") == 0)
			flag = TRUE;
		g_free(name);
		targets = targets->next;
	}
	return flag;
}

