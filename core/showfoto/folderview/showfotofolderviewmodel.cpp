/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-08-27
 * Description : Showfoto folder view model.
 *
 * Copyright (C) 2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "showfotofolderviewmodel.h"

// Qt includes

#include <QIODevice>
#include <QModelIndex>

// Local includes

#include "digikam_debug.h"
#include "digikam_globals.h"
#include "showfotofolderviewiconprovider.h"

using namespace Digikam;

namespace ShowFoto
{

class Q_DECL_HIDDEN ShowfotoFolderViewModel::Private
{

public:

    explicit Private()
        : provider(nullptr)
    {
    }

    ShowfotoFolderViewIconProvider* provider;
};

ShowfotoFolderViewModel::ShowfotoFolderViewModel(QObject* const parent)
    : QFileSystemModel(parent),
      d               (new Private)
{
    setObjectName(QLatin1String("ShowfotoFolderModel"));

    // --- Populate the model

    setRootPath(QDir::rootPath());

    QString filter;
    QStringList mimeTypes = supportedImageMimeTypes(QIODevice::ReadOnly, filter);
    QString patterns      = filter.toLower();
    patterns.append(QLatin1Char(' '));
    patterns.append(filter.toUpper());
    setNameFilters(patterns.split(QLatin1Char(' ')));

    // If an item fails the filter, hide it

    setNameFilterDisables(false);

    d->provider           = new ShowfotoFolderViewIconProvider();
    setIconProvider(d->provider);
}

ShowfotoFolderViewModel::~ShowfotoFolderViewModel()
{
    delete d->provider;
    delete d;
}

} // namespace ShowFoto
