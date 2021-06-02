/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2018-07-30
 * Description : a plugin to export to Flickr web-service.
 *
 * Copyright (C) 2018-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "flickrplugin.h"

// Qt includes

#include <QPointer>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "flickrwindow.h"

namespace DigikamGenericFlickrPlugin
{

FlickrPlugin::FlickrPlugin(QObject* const parent)
    : DPluginGeneric(parent)
{
}

FlickrPlugin::~FlickrPlugin()
{
}

void FlickrPlugin::cleanUp()
{
    delete m_toolDlg;
}

QString FlickrPlugin::name() const
{
    return i18nc("@title", "Flickr");
}

QString FlickrPlugin::iid() const
{
    return QLatin1String(DPLUGIN_IID);
}

QIcon FlickrPlugin::icon() const
{
    return QIcon::fromTheme(QLatin1String("dk-flickr"));
}

QString FlickrPlugin::description() const
{
    return i18nc("@info", "A tool to export to Flickr web-service");
}

QString FlickrPlugin::details() const
{
    return i18nc("@info", "This tool allow to export items to Flickr web-service.\n\n"
                 "See Flickr web site for details: %1",
                 QLatin1String("<a href='https://www.flickr.com/'>https://www.flickr.com/</a>"));
}

QList<DPluginAuthor> FlickrPlugin::authors() const
{
    return QList<DPluginAuthor>()
            << DPluginAuthor(QString::fromUtf8("Vardhman Jain"),
                             QString::fromUtf8("vardhman at gmail dot com"),
                             QString::fromUtf8("(C) 2005-2008"))
            << DPluginAuthor(QString::fromUtf8("Luka Renko"),
                             QString::fromUtf8("lure at kubuntu dot org"),
                             QString::fromUtf8("(C) 2009"))
            << DPluginAuthor(QString::fromUtf8("Shourya Singh Gupta"),
                             QString::fromUtf8("shouryasgupta at gmail dot com"),
                             QString::fromUtf8("(C) 2015"))
            << DPluginAuthor(QString::fromUtf8("Maik Qualmann"),
                             QString::fromUtf8("metzpinguin at gmail dot com"),
                             QString::fromUtf8("(C) 2017-2021"))
            << DPluginAuthor(QString::fromUtf8("Gilles Caulier"),
                             QString::fromUtf8("caulier dot gilles at gmail dot com"),
                             QString::fromUtf8("(C) 2008-2021"))
            ;
}

void FlickrPlugin::setup(QObject* const parent)
{
    DPluginAction* const ac = new DPluginAction(parent);
    ac->setIcon(icon());
    ac->setText(i18nc("@action", "Export to &Flickr..."));
    ac->setObjectName(QLatin1String("export_flickr"));
    ac->setActionCategory(DPluginAction::GenericExport);
    ac->setShortcut(Qt::CTRL + Qt::ALT + Qt::SHIFT + Qt::Key_R);

    connect(ac, SIGNAL(triggered(bool)),
            this, SLOT(slotFlickr()));

    addAction(ac);
}

void FlickrPlugin::slotFlickr()
{
    if (!reactivateToolDialog(m_toolDlg))
    {
        delete m_toolDlg;
        m_toolDlg = new FlickrWindow(infoIface(sender()), nullptr);
        m_toolDlg->setPlugin(this);
        m_toolDlg->show();
    }
}

} // namespace DigikamGenericFlickrPlugin
