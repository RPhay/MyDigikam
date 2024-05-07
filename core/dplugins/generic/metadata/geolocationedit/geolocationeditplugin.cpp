/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2018-07-30
 * Description : a plugin to edit items geolocation.
 *
 * SPDX-FileCopyrightText: 2018-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "geolocationeditplugin.h"

// Qt includes

#include <QPointer>
#include <QApplication>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "geolocationedit.h"

namespace DigikamGenericGeolocationEditPlugin
{

GeolocationEditPlugin::GeolocationEditPlugin(QObject* const parent)
    : DPluginGeneric(parent)
{
}

QString GeolocationEditPlugin::name() const
{
    return i18nc("@title", "Geolocation Edit");
}

QString GeolocationEditPlugin::iid() const
{
    return QLatin1String(DPLUGIN_IID);
}

QIcon GeolocationEditPlugin::icon() const
{
    return QIcon::fromTheme(QLatin1String("globe"));
}

QString GeolocationEditPlugin::description() const
{
    return i18nc("@info", "A tool to edit items geolocation");
}

QString GeolocationEditPlugin::details() const
{
    return i18nc("@info", "This tool allows users to change geolocation information from items.\n\n"
                 "This tool can edit GPS data, manually or over a map. Reverse geo-coding is also available through web services.\n\n"
                 "This tool has also an export function to KML to store map traces in Google format.\n\n"
                 "Finally, this tool is able to read a GPS trace from a device to synchronize geo-location of items if you camera do not have an embedded GPS device.");
}

QString GeolocationEditPlugin::handbookSection() const
{
    return QLatin1String("geolocation_editor");
}

QString GeolocationEditPlugin::handbookChapter() const
{
    return QLatin1String("geoeditor_overview");
}

QList<DPluginAuthor> GeolocationEditPlugin::authors() const
{
    return QList<DPluginAuthor>()
            << DPluginAuthor(QString::fromUtf8("Michael G. Hansen"),
                             QString::fromUtf8("mike at mghansen dot de"),
                             QString::fromUtf8("(C) 2008-2012"))
            << DPluginAuthor(QString::fromUtf8("Gabriel Voicu"),
                             QString::fromUtf8("ping dot gabi at gmail dot com"),
                             QString::fromUtf8("(C) 2010-2012"))
            << DPluginAuthor(QString::fromUtf8("Justus Schwartz"),
                             QString::fromUtf8("justus at gmx dot li"),
                             QString::fromUtf8("(C) 2014"))
            << DPluginAuthor(QString::fromUtf8("Gilles Caulier"),
                             QString::fromUtf8("caulier dot gilles at gmail dot com"),
                             QString::fromUtf8("(C) 2006-2024"))
            ;
}

void GeolocationEditPlugin::setup(QObject* const parent)
{
    DPluginAction* const ac = new DPluginAction(parent);
    ac->setIcon(icon());
    ac->setText(i18nc("@action", "Edit Geolocation..."));
    ac->setObjectName(QLatin1String("geolocation_edit"));
    ac->setActionCategory(DPluginAction::GenericMetadata);
    ac->setShortcut(Qt::CTRL | Qt::SHIFT | Qt::Key_G);

    connect(ac, SIGNAL(triggered(bool)),
            this, SLOT(slotEditGeolocation()));

    addAction(ac);
}

void GeolocationEditPlugin::slotEditGeolocation()
{
    QPointer<GeolocationEdit> dialog = new GeolocationEdit(qApp->activeWindow(), infoIface(sender()));
    dialog->setPlugin(this);
    dialog->exec();
    delete dialog;
}

} // namespace DigikamGenericGeolocationEditPlugin

#include "moc_geolocationeditplugin.cpp"
