/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2022-10-03
 * Description : a BQM plugin to assign tags metadata.
 *
 * SPDX-FileCopyrightText: 2021-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "assigntagsplugin.h"

// Qt includes

#include <QPointer>
#include <QString>
#include <QApplication>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "assigntags.h"

namespace DigikamBqmAssignTagsPlugin
{

AssignTagsPlugin::AssignTagsPlugin(QObject* const parent)
    : DPluginBqm(parent)
{
}

AssignTagsPlugin::~AssignTagsPlugin()
{
}

QString AssignTagsPlugin::name() const
{
    return i18nc("@title", "Assign Tags");
}

QString AssignTagsPlugin::iid() const
{
    return QLatin1String(DPLUGIN_IID);
}

QIcon AssignTagsPlugin::icon() const
{
    return QIcon::fromTheme(QLatin1String("tag"));
}

QString AssignTagsPlugin::description() const
{
    return i18nc("@info", "A tool to assign tags metadata to images");
}

QString AssignTagsPlugin::details() const
{
    return xi18nc("@info", "<para>This Batch Queue Manager tool will assign keyword hierarchies metadata values to images.</para>");
}

QString AssignTagsPlugin::handbookSection() const
{
    return QLatin1String("batch_queue");
}

QString AssignTagsPlugin::handbookChapter() const
{
    return QLatin1String("metadata_tools");
}

QString AssignTagsPlugin::handbookReference() const
{
    return QLatin1String("bqm-assigntags");
}

QList<DPluginAuthor> AssignTagsPlugin::authors() const
{
    return QList<DPluginAuthor>()
            << DPluginAuthor(QString::fromUtf8("Gilles Caulier"),
                             QString::fromUtf8("caulier dot gilles at gmail dot com"),
                             QString::fromUtf8("(C) 2021-2022"))
            ;
}

void AssignTagsPlugin::setup(QObject* const parent)
{
    AssignTags* const tool = new AssignTags(parent);
    tool->setPlugin(this);

    addTool(tool);
}

} // namespace DigikamBqmAssignTagsPlugin
