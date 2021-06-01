/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2018-07-30
 * Description : a plugin to adjust items date-time.
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

#include "timeadjustplugin.h"

// Qt includes

#include <QPointer>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "timeadjustdialog.h"

namespace DigikamGenericTimeAdjustPlugin
{

TimeAdjustPlugin::TimeAdjustPlugin(QObject* const parent)
    : DPluginGeneric(parent)
{
}

TimeAdjustPlugin::~TimeAdjustPlugin()
{
}

QString TimeAdjustPlugin::name() const
{
    return i18nc("@title", "Time Adjust");
}

QString TimeAdjustPlugin::iid() const
{
    return QLatin1String(DPLUGIN_IID);
}

QIcon TimeAdjustPlugin::icon() const
{
    return QIcon::fromTheme(QLatin1String("appointment-new"));
}

QString TimeAdjustPlugin::description() const
{
    return i18nc("@info", "A tool to adjust items date-time");
}

QString TimeAdjustPlugin::details() const
{
    return i18nc("@info", "This tool allow to adjust date time-stamp of items in batch.\n\n"
                 "Many source of original time-stamp can be selected from original items, or a from a common file.\n\n"
                 "Many metadata time-stamp can be adjusted or left untouched. The adjustment can be an offset of time or a specific date.");
}

QList<DPluginAuthor> TimeAdjustPlugin::authors() const
{
    return QList<DPluginAuthor>()
            << DPluginAuthor(QString::fromUtf8("Jesper K. Pedersen"),
                             QString::fromUtf8("blackie at kde dot org"),
                             QString::fromUtf8("(C) 2003-2005"))
            << DPluginAuthor(QString::fromUtf8("Smit Mehta"),
                             QString::fromUtf8("smit dot meh at gmail dot com"),
                             QString::fromUtf8("(C) 2012"))
            << DPluginAuthor(QString::fromUtf8("Pieter Edelman"),
                             QString::fromUtf8("p dot edelman at gmx dot net"),
                             QString::fromUtf8("(C) 2008"))
            << DPluginAuthor(QString::fromUtf8("Maik Qualmann"),
                             QString::fromUtf8("metzpinguin at gmail dot com"),
                             QString::fromUtf8("(C) 2018-2021"))
            << DPluginAuthor(QString::fromUtf8("Gilles Caulier"),
                             QString::fromUtf8("caulier dot gilles at gmail dot com"),
                             QString::fromUtf8("(C) 2006-2021"))
            ;
}

void TimeAdjustPlugin::setup(QObject* const parent)
{
    DPluginAction* const ac = new DPluginAction(parent);
    ac->setIcon(icon());
    ac->setText(i18nc("@action", "Adjust Time && Date..."));
    ac->setObjectName(QLatin1String("timeadjust_edit"));
    ac->setActionCategory(DPluginAction::GenericMetadata);
    ac->setShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_D);

    connect(ac, SIGNAL(triggered(bool)),
            this, SLOT(slotTimeAdjust()));

    addAction(ac);
}

void TimeAdjustPlugin::slotTimeAdjust()
{
    QPointer<TimeAdjustDialog> dialog = new TimeAdjustDialog(nullptr, infoIface(sender()));
    dialog->setPlugin(this);
    dialog->exec();
    delete dialog;
}

} // namespace DigikamGenericTimeAdjustPlugin
