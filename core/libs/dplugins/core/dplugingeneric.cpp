/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2018-07-30
 * Description : Generic digiKam plugin definition.
 *
 * SPDX-FileCopyrightText: 2018-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "dplugingeneric.h"

// Qt includes

#include <QApplication>

// Local includes

#include "digikam_version.h"
#include "digikam_debug.h"
#include "dxmlguiwindow.h"
#include "dplugindialog.h"

namespace Digikam
{

class Q_DECL_HIDDEN DPluginGeneric::Private
{
public:

    explicit Private()
    {
    }

    QList<DPluginAction*> actions;
};

DPluginGeneric::DPluginGeneric(QObject* const parent)
    : DPlugin(parent),
      d      (new Private)
{
}

DPluginGeneric::~DPluginGeneric()
{
    delete d;
}

DInfoInterface* DPluginGeneric::infoIface(QObject* const ac) const
{
    DPluginAction* const pac = dynamic_cast<DPluginAction*>(ac);

    if (pac)
    {
        DXmlGuiWindow* const gui = dynamic_cast<DXmlGuiWindow*>(pac->parent());

        if (gui)
        {
            return gui->infoIface(pac);
        }

        DInfoInterface* const iface = dynamic_cast<DInfoInterface*>(pac->parent());

        if (iface)
        {
            return iface;
        }
    }

    return nullptr;
}

int DPluginGeneric::count() const
{
    int count       = 0;
    QObject* parent = nullptr;

    Q_FOREACH (DPluginAction* const ac, d->actions)
    {
        if (ac)
        {
            // NOTE: we will return the count of actions registered with the same parents,
            //       as each parent registered the same list of actions through setup().

            if      (!count)
            {
                parent = ac->parent(),
                ++count;
            }
            else if (ac->parent() == parent)
            {
                ++count;
            }
        }
    }

    return count;
}

void DPluginGeneric::setVisible(bool b)
{
    Q_FOREACH (DPluginAction* const ac, d->actions)
    {
        if (ac)
        {
            ac->setVisible(b);
        }
    }
}

QList<DPluginAction*> DPluginGeneric::actions(QObject* const parent) const
{
    QList<DPluginAction*> list;

    Q_FOREACH (DPluginAction* const ac, d->actions)
    {
        if (ac && (ac->parent() == parent))
        {
            list << ac;
        }
    }

    std::sort(list.begin(), list.end(), DPluginAction::pluginActionLessThan);

    return list;
}

DPluginAction* DPluginGeneric::findActionByName(const QString& name, QObject* const parent) const
{
    Q_FOREACH (DPluginAction* const ac, actions(parent))
    {
        if (ac && (ac->objectName() == name))
        {    // cppcheck-suppress useStlAlgorithm
            return ac;
        }
    }

    return nullptr;
}

void DPluginGeneric::addAction(DPluginAction* const ac)
{
    ac->setProperty("DPluginIId",      iid());
    ac->setProperty("DPluginIfaceIId", ifaceIid());
    d->actions.append(ac);
}

QStringList DPluginGeneric::categories() const
{
    QStringList list;

    Q_FOREACH (DPluginAction* const ac, d->actions)
    {
        QString cat = ac->actionCategoryToString();

        if (!list.contains(cat))
        {
            list << cat;
        }
    }

    list.sort();

    return list;
}

bool DPluginGeneric::reactivateToolDialog(QWidget* const dlg) const
{
    if (dlg && (dlg->isMinimized() || !dlg->isHidden()))
    {
        dlg->showNormal();       // krazy:exclude=qmethods
        dlg->activateWindow();
        dlg->raise();

        return true;
    }

    return false;
}

} // namespace Digikam
