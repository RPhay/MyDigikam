/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-03-20
 * Description : a tool to export images to iNaturalist web service
 *
 * Copyright (C) 2021      by Joerg Lohse <joergmlpts at gmail dot com>
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

#include "inatplugin.h"

// Qt includes

#include <QPointer>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "inatwindow.h"

namespace DigikamGenericINatPlugin
{

INatPlugin::INatPlugin(QObject* const parent)
    : DPluginGeneric(parent)
{
}

INatPlugin::~INatPlugin()
{
}

void INatPlugin::cleanUp()
{
    delete m_toolDlg;
}

QString INatPlugin::name() const
{
    return i18nc("@title", "iNaturalist");
}

QString INatPlugin::iid() const
{
    return QLatin1String(DPLUGIN_IID);
}

QIcon INatPlugin::icon() const
{
    return QIcon::fromTheme(QLatin1String("dk-inat"));
}

QString INatPlugin::description() const
{
    return i18nc("@info",
                 "A tool to export photos as iNaturalist observations.");
}

QString INatPlugin::details() const
{
    return i18nc("@info",
                 "This tool exports photos as iNaturalist observations.\n\n"
                 "See iNaturalist web site for details: %1.",
                 QLatin1String("<a href='https://www.inaturalist.org/'>"
                               "https://www.inaturalist.org/</a>"));
}

QList<DPluginAuthor> INatPlugin::authors() const
{
    return QList<DPluginAuthor>()
           << DPluginAuthor(QString::fromUtf8("Joerg Lohse"),
                            QString::fromUtf8("joergmlpts at gmail dot com"),
                            QString::fromUtf8("(C) 2021"));
}

void INatPlugin::setup(QObject* const parent)
{
    DPluginAction* const ac = new DPluginAction(parent);
    ac->setIcon(icon());
    ac->setText(i18nc("@action", "Export to &iNaturalist..."));
    ac->setObjectName(QLatin1String("export_inaturalist"));
    ac->setActionCategory(DPluginAction::GenericExport);
    ac->setShortcut(Qt::CTRL + Qt::ALT + Qt::SHIFT + Qt::Key_N);

    connect(ac, SIGNAL(triggered(bool)),
            this, SLOT(slotINat()));

    addAction(ac);
}

void INatPlugin::slotINat()
{
    if (!reactivateToolDialog(m_toolDlg))
    {
        delete m_toolDlg;
        m_toolDlg = new INatWindow(infoIface(sender()), nullptr);
        m_toolDlg->setPlugin(this);
        m_toolDlg->show();
    }
}

} // namespace DigikamGenericINatPlugin
