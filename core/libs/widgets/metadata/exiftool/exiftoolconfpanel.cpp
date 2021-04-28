/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-04-29
 * Description : ExifTool configuration panel.
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

#include "exiftoolconfpanel.h"

// Qt includes

#include <QVBoxLayout>
#include <QLabel>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "exiftoolbinary.h"
#include "dbinarysearch.h"
#include "metaenginesettings.h"

namespace Digikam
{

class Q_DECL_HIDDEN ExifToolConfPanel::Private
{
public:

    explicit Private()
      : exifToolBinWidget(nullptr)
    {
    }

public:

    DBinarySearch* exifToolBinWidget;
    ExifToolBinary exifToolBin;
};

// --------------------------------------------------------

ExifToolConfPanel::ExifToolConfPanel(QWidget* const parent)
    : QWidget(parent),
      d      (new Private)
{
    QVBoxLayout* const exifToolLayout = new QVBoxLayout;
    QLabel* const exifToolBinLabel    = new QLabel(i18nc("@info",
                                                   "ExifTool is an open-source software program for reading, writing, "
                                                   "and manipulating multimedia files. It is platform independent "
                                                   "available as a command-line application. ExifTool is commonly "
                                                   "incorporated into different types of digital workflows and supports "
                                                   "many types of metadata including Exif, IPTC, XMP, JFIF, GeoTIFF, ICC Profile, "
                                                   "Photoshop IRB, as well as the manufacturer-specific metadata formats of "
                                                   "many digital cameras.\n\n"
                                                   "Here you can configure location where ExifTool binary is located. "
                                                   "Application will try to find this binary automatically if they are "
                                                   "already installed on your computer."),
                                                   this);
    exifToolBinLabel->setWordWrap(true);

    d->exifToolBinWidget              = new DBinarySearch(this);
    d->exifToolBinWidget->addBinary(d->exifToolBin);

    foreach (const QString& path, MetaEngineSettings::instance()->settings().defaultExifToolSearchPaths())
    {
        d->exifToolBinWidget->addDirectory(path);
    }

    d->exifToolBinWidget->allBinariesFound();

    exifToolLayout->addWidget(exifToolBinLabel);
    exifToolLayout->addWidget(d->exifToolBinWidget);
    exifToolLayout->addStretch();
    setLayout(exifToolLayout);
}

ExifToolConfPanel::~ExifToolConfPanel()
{
    delete d;
}

QString ExifToolConfPanel::exifToolDirectory() const
{
    return d->exifToolBin.directory();
}

void ExifToolConfPanel::setExifToolDirectory(const QString& dir)
{
    d->exifToolBin.setup(dir);
}

} // namespace Digikam
