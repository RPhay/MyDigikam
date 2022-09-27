/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2018-12-31
 * Description : configuration view for external generic plugin
 *
 * SPDX-FileCopyrightText: 2018-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "dpluginconfviewgeneric.h"

// Local includes

#include "dplugingeneric.h"
#include "dpluginloader.h"

namespace Digikam
{

DPluginConfViewGeneric::DPluginConfViewGeneric(QWidget* const parent)
    : DPluginConfView(parent)
{
    this->loadPlugins();
}

DPluginConfViewGeneric::~DPluginConfViewGeneric()
{
}

void DPluginConfViewGeneric::loadPlugins()
{
    DPluginLoader* const loader = DPluginLoader::instance();

    if (loader)
    {
        Q_FOREACH (DPlugin* const tool, loader->allPlugins())
        {
            DPluginGeneric* const gene = dynamic_cast<DPluginGeneric*>(tool);

            if (gene)
            {
                appendPlugin(gene);
            }
        }
    }

    // Sort items by plugin names.

    sortItems(0, Qt::AscendingOrder);
}

} // namespace Digikam
