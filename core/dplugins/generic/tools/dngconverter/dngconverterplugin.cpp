/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-09-24
 * Description : a plugin to convert Raw files to DNG
 *
 * Copyright (C) 2008-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "dngconverterplugin.h"

// Qt includes

#include <QPointer>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "dngconverterdialog.h"

namespace DigikamGenericDNGConverterPlugin
{

DNGConverterPlugin::DNGConverterPlugin(QObject* const parent)
    : DPluginGeneric(parent)
{
}

DNGConverterPlugin::~DNGConverterPlugin()
{
}

QString DNGConverterPlugin::name() const
{
    return i18nc("@title", "DNG Converter");
}

QString DNGConverterPlugin::iid() const
{
    return QLatin1String(DPLUGIN_IID);
}

QIcon DNGConverterPlugin::icon() const
{
    return QIcon::fromTheme(QLatin1String("image-x-adobe-dng"));
}

QString DNGConverterPlugin::description() const
{
    return i18nc("@info", "A tool to batch convert RAW camera images to DNG");
}

QString DNGConverterPlugin::details() const
{
    return i18nc("@info", "This Generic tool can convert RAW images data to DNG format.\n\n"
                 "The Digital Negative is a lossless RAW image format created by Adobe.\n\n"
                 "See details on this wikipedia entry about this format: %1",
                 QLatin1String("<a href='https://en.wikipedia.org/wiki/Digital_Negative'>https://en.wikipedia.org/wiki/Digital_Negative</a>"));
}

QList<DPluginAuthor> DNGConverterPlugin::authors() const
{
    return QList<DPluginAuthor>()
            << DPluginAuthor(QString::fromUtf8("Smit Mehta"),
                             QString::fromUtf8("smit dot meh at gmail dot com"),
                             QString::fromUtf8("(C) 2012"))
            << DPluginAuthor(QString::fromUtf8("Jens Mueller"),
                             QString::fromUtf8("tschenser at gmx dot de"),
                             QString::fromUtf8("(C) 2010-2011"))
            << DPluginAuthor(QString::fromUtf8("Gilles Caulier"),
                             QString::fromUtf8("caulier dot gilles at gmail dot com"),
                             QString::fromUtf8("(C) 2008-2021"))
            << DPluginAuthor(QString::fromUtf8("Maik Qualmann"),
                             QString::fromUtf8("metzpinguin at gmail dot com"),
                             QString::fromUtf8("(C) 2020-2021"))
            ;
}

void DNGConverterPlugin::setup(QObject* const parent)
{
    DPluginAction* const ac = new DPluginAction(parent);
    ac->setIcon(icon());
    ac->setText(i18nc("@action", "DNG Converter..."));
    ac->setObjectName(QLatin1String("dngconverter"));
    ac->setActionCategory(DPluginAction::GenericTool);

    connect(ac, SIGNAL(triggered(bool)),
            this, SLOT(slotDNGConverter()));

    addAction(ac);
}

void DNGConverterPlugin::slotDNGConverter()
{
    QPointer<DNGConverterDialog> dialog = new DNGConverterDialog(nullptr, infoIface(sender()));
    dialog->setPlugin(this);
    dialog->exec();
    delete dialog;
}

} // namespace DigikamGenericDNGConverterPlugin
