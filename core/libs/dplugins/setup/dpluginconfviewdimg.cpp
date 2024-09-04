/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2018-09-22
 * Description : configuration view for external DImg plugin
 *
 * SPDX-FileCopyrightText: 2020-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "dpluginconfviewdimg.h"

// Qt includes

#include <QHeaderView>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "itempropertiestxtlabel.h"
#include "dplugindimg.h"
#include "dpluginloader.h"

namespace Digikam
{

DPluginConfViewDImg::DPluginConfViewDImg(QWidget* const parent)
    : DPluginConfView(parent)
{
    setColumnHidden(1, true);
    headerItem()->setText(2, i18n("Type-Mimes"));
    header()->setSectionResizeMode(2, QHeaderView::Stretch);
    this->loadPlugins();
}

void DPluginConfViewDImg::loadPlugins()
{
    DPluginLoader* const loader = DPluginLoader::instance();

    if (loader)
    {
        const auto tools = loader->allPlugins();

        for (DPlugin* const tool : tools)
        {
            DPluginDImg* const plug = dynamic_cast<DPluginDImg*>(tool);

            if (plug)
            {
                QTreeWidgetItem* const item = appendPlugin(plug);
                DTextBrowser* const tview   = new DTextBrowser(plug->typeMimes(), this);
                setItemWidget(item, 2, tview);
            }
        }
    }

    // Sort items by plugin names.

    sortItems(0, Qt::AscendingOrder);
}

} // namespace Digikam

#include "moc_dpluginconfviewdimg.cpp"
