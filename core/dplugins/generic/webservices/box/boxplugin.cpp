/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2018-07-30
 * Description : a plugin to export to Box web-service.
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

#include "boxplugin.h"

// Qt includes

#include <QPointer>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "boxwindow.h"

namespace DigikamGenericBoxPlugin
{

BoxPlugin::BoxPlugin(QObject* const parent)
    : DPluginGeneric(parent)
{
}

BoxPlugin::~BoxPlugin()
{
}

void BoxPlugin::cleanUp()
{
    delete m_toolDlg;
}

QString BoxPlugin::name() const
{
    return i18nc("@title", "Box");
}

QString BoxPlugin::iid() const
{
    return QLatin1String(DPLUGIN_IID);
}

QIcon BoxPlugin::icon() const
{
    return QIcon::fromTheme(QLatin1String("dk-box"));
}

QString BoxPlugin::description() const
{
    return i18nc("@info", "A tool to export to Box web-service");
}

QString BoxPlugin::details() const
{
    return i18nc("@info", "This tool allow to export items to Box web-service.\n\n"
                "See Box web site for details: %1", QLatin1String("<a href='https://box.com/'>https://box.com/</a>"));
}

QList<DPluginAuthor> BoxPlugin::authors() const
{
    return QList<DPluginAuthor>()
            << DPluginAuthor(QString::fromUtf8("Tarek Talaat"),
                             QString::fromUtf8("tarektalaat93 at gmail dot com"),
                             QString::fromUtf8("(C) 2018"))
            ;
}

void BoxPlugin::setup(QObject* const parent)
{
    DPluginAction* const ac = new DPluginAction(parent);
    ac->setIcon(icon());
    ac->setText(i18nc("@action", "Export to &Box..."));
    ac->setObjectName(QLatin1String("export_box"));
    ac->setActionCategory(DPluginAction::GenericExport);
    ac->setShortcut(Qt::CTRL + Qt::ALT + Qt::SHIFT + Qt::Key_B);

    connect(ac, SIGNAL(triggered(bool)),
            this, SLOT(slotBox()));

    addAction(ac);
}

void BoxPlugin::slotBox()
{
    if (!reactivateToolDialog(m_toolDlg))
    {
        delete m_toolDlg;
        m_toolDlg = new BOXWindow(infoIface(sender()), nullptr);
        m_toolDlg->setPlugin(this);
        m_toolDlg->show();
    }
}

} // namespace DigikamGenericBoxPlugin
