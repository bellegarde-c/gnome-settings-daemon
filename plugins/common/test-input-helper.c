/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*-
 *
 * Copyright (C) 2011 Bastien Nocera <hadess@hadess.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 */

#include "config.h"

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk/gdkx.h>

#include <sys/types.h>
#include <X11/Xatom.h>

#include "gsd-input-helper.h"

int main (int argc, char **argv)
{
	gboolean supports_xinput;
	gboolean has_touchpad, has_touchscreen;
        XDeviceInfo *device_info;
        char *dev_node;
        int devid;
        gint n_devices;
        guint i;

	gtk_init (&argc, &argv);

	supports_xinput = supports_xinput_devices ();
	if (supports_xinput) {
		g_print ("Supports XInput:\t\t\tyes\n");
	} else {
		g_print ("Supports XInput:\t\t\tno\n");
		return 0;
	}
	supports_xinput = supports_xinput2_devices ();
	if (supports_xinput) {
		g_print ("Supports XInput2:\t\t\tyes\n");
	} else {
		g_print ("Supports XInput2:\t\t\tno\n");
		return 0;
	}

	has_touchpad = touchpad_is_present ();
	g_print ("Has touchpad:\t\t\t\t%s\n", has_touchpad ? "yes" : "no");

	has_touchscreen = touchscreen_is_present ();
	g_print ("Has touchscreen:\t\t\t%s\n", has_touchscreen ? "yes" : "no");

	if (accelerometer_is_present (&dev_node, &devid)) {
		g_print ("Has accelerometer:\t\t\t%s (%d)\n", dev_node, devid);
		g_free (dev_node);
	} else {
		g_print ("Has accelerometer:\t\t\t%s\n", "no");
	}

        device_info = XListInputDevices (GDK_DISPLAY_XDISPLAY (gdk_display_get_default ()), &n_devices);
        if (device_info == NULL) {
		g_warning ("Has no input devices");
                return 1;
	}

        for (i = 0; i < n_devices; i++) {
                XDevice *device;

		if (device_info_is_touchscreen (&device_info[i])) {
			g_print ("Device %d is touchscreen:\t\t%s\n", (int) device_info[i].id, "yes");
			continue;
		}

                gdk_error_trap_push ();
                device = XOpenDevice (GDK_DISPLAY_XDISPLAY (gdk_display_get_default ()), device_info[i].id);
                if (gdk_error_trap_pop () || (device == NULL))
                        continue;

                if (device_is_touchpad (device))
			g_print ("Device %d is touchpad:\t\t%s\n", (int) device_info[i].id, "yes");
		else
			g_print ("Device %d is touchpad/touchscreen:\t%s\n", (int) device_info[i].id, "no");

                XCloseDevice (GDK_DISPLAY_XDISPLAY (gdk_display_get_default ()), device);
        }
        XFreeDeviceList (device_info);

	return 0;
}
