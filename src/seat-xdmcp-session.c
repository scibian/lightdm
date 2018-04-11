/*
 * Copyright (C) 2010-2011 Robert Ancell.
 * Author: Robert Ancell <robert.ancell@canonical.com>
 * 
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version. See http://www.gnu.org/copyleft/gpl.html the full text of the
 * license.
 */

#include <string.h>

#include "seat-xdmcp-session.h"
#include "x-server-remote.h"

struct SeatXDMCPSessionPrivate
{
    /* Session being serviced */
    XDMCPSession *session;
};

G_DEFINE_TYPE (SeatXDMCPSession, seat_xdmcp_session, SEAT_TYPE);

SeatXDMCPSession *
seat_xdmcp_session_new (XDMCPSession *session)
{
    SeatXDMCPSession *seat;

    seat = g_object_new (SEAT_XDMCP_SESSION_TYPE, NULL);
    seat->priv->session = g_object_ref (session);

    return seat;
}

static DisplayServer *
seat_xdmcp_session_create_display_server (Seat *seat, Session *session)
{
    XAuthority *authority;
    gchar *host;
    XServerRemote *x_server;

    if (strcmp (session_get_session_type (session), "x") != 0)
        return NULL;

    authority = xdmcp_session_get_authority (SEAT_XDMCP_SESSION (seat)->priv->session);
    host = g_inet_address_to_string (xdmcp_session_get_address (SEAT_XDMCP_SESSION (seat)->priv->session));
    x_server = x_server_remote_new (host, xdmcp_session_get_display_number (SEAT_XDMCP_SESSION (seat)->priv->session), authority);
    g_free (host);

    return DISPLAY_SERVER (x_server);
}

static void
seat_xdmcp_session_init (SeatXDMCPSession *seat)
{
    seat->priv = G_TYPE_INSTANCE_GET_PRIVATE (seat, SEAT_XDMCP_SESSION_TYPE, SeatXDMCPSessionPrivate);
}

static void
seat_xdmcp_session_finalize (GObject *object)
{
    SeatXDMCPSession *self;

    self = SEAT_XDMCP_SESSION (object);

    g_object_unref (self->priv->session);

    G_OBJECT_CLASS (seat_xdmcp_session_parent_class)->finalize (object);
}

static void
seat_xdmcp_session_class_init (SeatXDMCPSessionClass *klass)
{
    SeatClass *seat_class = SEAT_CLASS (klass);
    GObjectClass *object_class = G_OBJECT_CLASS (klass);

    seat_class->create_display_server = seat_xdmcp_session_create_display_server;
    object_class->finalize = seat_xdmcp_session_finalize;

    g_type_class_add_private (klass, sizeof (SeatXDMCPSessionPrivate));
}
