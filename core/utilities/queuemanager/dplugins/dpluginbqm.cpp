/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2018-07-30
 * Description : Batch Queue Manager digiKam plugin definition.
 *
 * SPDX-FileCopyrightText: 2018-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "dpluginbqm.h"

// Qt includes

#include <QApplication>

// Local includes

#include "digikam_version.h"
#include "digikam_debug.h"
#include "batchtoolsfactory.h"

namespace Digikam
{

class Q_DECL_HIDDEN DPluginBqm::Private
{
public:

    Private() = default;

    QList<BatchTool*> tools;
};

DPluginBqm::DPluginBqm(QObject* const parent)
    : DPlugin(parent),
      d      (new Private)
{
}

DPluginBqm::~DPluginBqm()
{
    delete d;
}

void DPluginBqm::setVisible(bool b)
{
    Q_EMIT signalVisible(b);
}

int DPluginBqm::count() const
{
    return d->tools.count();
}

QList<BatchTool*> DPluginBqm::tools(QObject* const parent) const
{
    QList<BatchTool*> list;

    Q_FOREACH (BatchTool* const t, d->tools)
    {
        if (t && (t->parent() == parent))
        {
            list << t;
        }
    }

    return list;
}

BatchTool* DPluginBqm::findToolByName(const QString& name, QObject* const parent) const
{
    Q_FOREACH (BatchTool* const t, tools(parent))
    {
        if (t && (t->objectName() == name))
        {   // cppcheck-suppress useStlAlgorithm
            return t;
        }
    }

    return nullptr;
}

void DPluginBqm::addTool(BatchTool* const t)
{
    t->setProperty("DPluginIId",      iid());
    t->setProperty("DPluginIfaceIId", ifaceIid());
    d->tools.append(t);
}

DInfoInterface* DPluginBqm::infoIface() const
{
    DInfoInterface* const iface = BatchToolsFactory::instance()->infoIface();

    if (iface)
    {
        return iface;
    }

    return nullptr;
}

QStringList DPluginBqm::categories() const
{
    QStringList list;

    Q_FOREACH (BatchTool* const t, d->tools)
    {
        QString cat = t->toolGroupToString();

        if (!list.contains(cat))
        {
            list << cat;
        }
    }

    list.sort();

    return list;
}

bool DPluginBqm::hasVisibilityProperty() const
{
    // NOTE: all BQM plugins are not configurable.
    // Code is missing in BQM to check workflow tools list validity if a plugin is disabled from setup dialog.

    return false;
}

QString DPluginBqm::ifaceIid() const
{
    return QLatin1String(DIGIKAM_DPLUGIN_BQM_IID);
}

} // namespace Digikam

#include "moc_dpluginbqm.cpp"
