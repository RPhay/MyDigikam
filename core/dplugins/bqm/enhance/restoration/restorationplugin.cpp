/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2018-07-30
 * Description : a BQM plugin to restore images
 *
 * Copyright (C) 2018-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "restorationplugin.h"

// Qt includes

#include <QPointer>
#include <QString>
#include <QApplication>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "restoration.h"

namespace Digikam
{

RestorationPlugin::RestorationPlugin(QObject* const parent)
    : DPluginBqm(parent)
{
}

RestorationPlugin::~RestorationPlugin()
{
}

QString RestorationPlugin::name() const
{
    return i18nc("@title", "Restoration");
}

QString RestorationPlugin::iid() const
{
    return QLatin1String(DPLUGIN_IID);
}

QIcon RestorationPlugin::icon() const
{
    return QIcon::fromTheme(QLatin1String("colorfx"));
}

QString RestorationPlugin::description() const
{
    return i18nc("@info", "A tool to restore images using Greystoration algorithm");
}

QString RestorationPlugin::details() const
{
    return xi18nc("@info", "<para>This Batch Queue Manager tool can restore images using Greystoration algorithm.</para>");
}

QList<DPluginAuthor> RestorationPlugin::authors() const
{
    return QList<DPluginAuthor>()
            << DPluginAuthor(QString::fromUtf8("Gilles Caulier"),
                             QString::fromUtf8("caulier dot gilles at gmail dot com"),
                             QString::fromUtf8("(C) 2009-2022"))
            ;
}

void RestorationPlugin::setup(QObject* const parent)
{
    Restoration* const tool = new Restoration(parent);
    tool->setPlugin(this);

    addTool(tool);
}

} // namespace Digikam
