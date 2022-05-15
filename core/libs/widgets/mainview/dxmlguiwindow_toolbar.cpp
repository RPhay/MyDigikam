/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2013-04-29
 * Description : digiKam XML GUI window - Tool-bar methods.
 *
 * Copyright (C) 2013-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "dxmlguiwindow_p.h"

namespace Digikam
{

void DXmlGuiWindow::slotConfToolbars()
{
    KConfigGroup group = KSharedConfig::openConfig()->group(configGroupName());
    saveMainWindowSettings(group);

    KEditToolBar dlg(factory(), this);

    connect(&dlg, SIGNAL(newToolbarConfig()),
            this, SLOT(slotNewToolbarConfig()));

    dlg.exec();
}

void DXmlGuiWindow::slotNewToolbarConfig()
{
    KConfigGroup group = KSharedConfig::openConfig()->group(configGroupName());
    applyMainWindowSettings(group);
}

KToolBar* DXmlGuiWindow::mainToolBar() const
{
    QList<KToolBar*> toolbars = toolBars();
    KToolBar* mainToolbar     = nullptr;

    Q_FOREACH (KToolBar* const toolbar, toolbars)
    {
        if (toolbar && (toolbar->objectName() == QLatin1String("mainToolBar")))
        {
            mainToolbar = toolbar;
            break;
        }
    }

    return mainToolbar;
}

void DXmlGuiWindow::showToolBars(bool visible)
{
    // We will hide toolbars: store previous state for future restoring.

    if (!visible)
    {
        d->toolbarsVisibility.clear();

        Q_FOREACH (KToolBar* const toolbar, toolBars())
        {
            if (toolbar)
            {
                bool visibility = toolbar->isVisible();
                d->toolbarsVisibility.insert(toolbar, visibility);
            }
        }
    }

    // Switch toolbars visibility

    for (QMap<KToolBar*, bool>::const_iterator it = d->toolbarsVisibility.constBegin() ;
         it != d->toolbarsVisibility.constEnd() ; ++it)
    {
        KToolBar* const toolbar = it.key();
        bool visibility         = it.value();

        if (toolbar)
        {
            if (visible && visibility)
            {
                toolbar->show();
            }
            else
            {
                toolbar->hide();
            }
        }
    }

    // We will show toolbars: restore previous state.

    if (visible)
    {
        for (QMap<KToolBar*, bool>::const_iterator it = d->toolbarsVisibility.constBegin() ;
             it != d->toolbarsVisibility.constEnd() ; ++it)
        {
            KToolBar* const toolbar = it.key();
            bool visibility         = it.value();

            if (toolbar)
            {
                visibility ? toolbar->show()
                           : toolbar->hide();
            }
        }
    }
}

} // namespace Digikam
