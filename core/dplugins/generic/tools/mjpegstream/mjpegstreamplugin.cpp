/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-07-24
 * Description : a plugin to share items with MJPEG Stream server.
 *
 * Copyright (C) 2021-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2021 by Quoc Hưng Tran <quochungtran1999 at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "mjpegstreamplugin.h"

// Qt includes

#include <QPointer>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "mjpegstreamdlg.h"
#include "mjpegservermngr.h"

namespace DigikamGenericMjpegStreamPlugin
{

MjpegStreamPlugin::MjpegStreamPlugin(QObject* const parent)
    : DPluginGeneric(parent)
{
    // Start the MJPEG Server if necessary

    MjpegServerMngr::instance()->loadAtStartup();
}

MjpegStreamPlugin::~MjpegStreamPlugin()
{
}

void MjpegStreamPlugin::cleanUp()
{
    // Stop the MJPEG Server if necessary

    MjpegServerMngr::instance()->saveAtShutdown();
}

QString MjpegStreamPlugin::name() const
{
    return i18n("MJPEG Stream Server");
}

QString MjpegStreamPlugin::iid() const
{
    return QLatin1String(DPLUGIN_IID);
}

QIcon MjpegStreamPlugin::icon() const
{
    return QIcon::fromTheme(QLatin1String("video-x-generic"));
}

QString MjpegStreamPlugin::description() const
{
    return i18n("A tool to export items as MJPEG Stream");
}

QString MjpegStreamPlugin::details() const
{
    return i18n(
        "<p>This tool allows users to share items on the local network through a MJPEG Stream server.</p>"
        "<p>Items to share can be selected one by one or by group through a selection of albums.</p>"
        "<p>Motion JPEG is a video compression format in which each video frame or interlaced field of "
        "a digital video sequence is compressed separately as a JPEG image. MJPEG streams is a standard "
        "which allows network clients to be connected without additional module. Most major web browsers "
        "and players support MJPEG stream.</p>"
        "<p>To access to stream from your browser, use http://address:port as url, with address the MJPEG address, "   // krazy:exclude=insecurenet
        "and port the MJPEG port set in config dialog. More than one computer can be connected to the MJPEG server "
        "at the same time.</p>");
}

QList<DPluginAuthor> MjpegStreamPlugin::authors() const
{
    return QList<DPluginAuthor>()
            << DPluginAuthor(QString::fromUtf8("Quoc Hưng Tran"),
                             QString::fromUtf8("quochungtran1999 at gmail dot com"),
                             QString::fromUtf8("(C) 2021"),
                             i18n("Developer"))
            << DPluginAuthor(QString::fromUtf8("Gilles Caulier"),
                             QString::fromUtf8("caulier dot gilles at gmail dot com"),
                             QString::fromUtf8("(C) 2021"),
                             i18n("Developer and Maintainer"))
            ;
}

void MjpegStreamPlugin::setup(QObject* const parent)
{
    DPluginAction* const ac = new DPluginAction(parent);
    ac->setIcon(icon());
    ac->setText(i18nc("@action", "Share as MJPEG Stream..."));
    ac->setObjectName(QLatin1String("mjpegstream"));
    ac->setActionCategory(DPluginAction::GenericTool);

    connect(ac, SIGNAL(triggered(bool)),
            this, SLOT(slotMjpegStream()));

    addAction(ac);
}

void MjpegStreamPlugin::slotMjpegStream()
{
    QPointer<MjpegStreamDlg> w = new MjpegStreamDlg(this, infoIface(sender()));
    w->setPlugin(this);
    w->exec();
    delete w;
}

} // namespace DigikamGenericMjpegStreamPlugin
