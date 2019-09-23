/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2018-09-22
 * Description : configuration view for external DImg plugin
 *
 * Copyright (C) 2019 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "dpluginconfviewdimg.h"

// Local includes

#include "dplugindimg.h"
#include "dpluginloader.h"

namespace Digikam
{

DPluginConfViewDImg::DPluginConfViewDImg(QWidget* const parent)
    : DPluginConfView(parent)
{
    loadPlugins();
}

DPluginConfViewDImg::~DPluginConfViewDImg()
{
}

void DPluginConfViewDImg::loadPlugins()
{
    DPluginLoader* const loader = DPluginLoader::instance();

    if (loader)
    {
        foreach (DPlugin* const tool, loader->allPlugins())
        {
            DPluginDImg* const bqm = dynamic_cast<DPluginDImg*>(tool);

            if (bqm)
            {
                appendPlugin(bqm);
            }
        }
    }

    // Sort items by plugin names.
    sortItems(0, Qt::AscendingOrder);
}

} // namespace Digikam
