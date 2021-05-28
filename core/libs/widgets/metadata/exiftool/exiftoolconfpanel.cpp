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

#include <QGridLayout>
#include <QVBoxLayout>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QLabel>
#include <QStringList>
#include <QGroupBox>
#include <QApplication>
#include <QStyle>
#include <QHeaderView>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "exiftoolbinary.h"
#include "exiftoolparser.h"
#include "dbinarysearch.h"
#include "metaenginesettings.h"

namespace Digikam
{

class Q_DECL_HIDDEN ExifToolConfPanel::Private
{
public:

    explicit Private()
      : exifToolBinWidget(nullptr),
        searchBar        (nullptr),
        exifToolFormats  (nullptr)
    {
    }

public:

    DBinarySearch* exifToolBinWidget;
    ExifToolBinary exifToolBin;
    SearchTextBar* searchBar;

    QTreeWidget*   exifToolFormats;
};

// --------------------------------------------------------

ExifToolConfPanel::ExifToolConfPanel(QWidget* const parent)
    : QWidget(parent),
      d      (new Private)
{
    QGridLayout* const grid        = new QGridLayout;
    QLabel* const exifToolBinLabel = new QLabel(i18nc("@info",
                                                "%1 is an open-source software program for reading, writing, "
                                                "and manipulating multimedia files. It is platform independent "
                                                "available as a command-line application. ExifTool is commonly "
                                                "incorporated into different types of digital workflows and supports "
                                                "many types of metadata including Exif, IPTC, XMP, JFIF, GeoTIFF, ICC Profile, "
                                                "Photoshop IRB, as well as the manufacturer-specific metadata formats of "
                                                "many digital cameras.\n\n"
                                                "Here you can configure location where ExifTool binary is located. "
                                                "Application will try to find this binary automatically if they are "
                                                "already installed on your computer.",
                                                QString::fromUtf8("<a href='https://www.exiftool.org'>ExifTool</a>")),
                                                this);
    exifToolBinLabel->setWordWrap(true);
    exifToolBinLabel->setOpenExternalLinks(true);

    d->exifToolBinWidget         = new DBinarySearch(this);
    d->exifToolBinWidget->addBinary(d->exifToolBin);

    QGroupBox* const exifToolBox = new QGroupBox(i18nc("@title: group", "Supported File Formats"), this);
    d->exifToolFormats           = new QTreeWidget(exifToolBox);
    d->exifToolFormats->setRootIsDecorated(false);
    d->exifToolFormats->setSortingEnabled(true);
    d->exifToolFormats->sortByColumn(0, Qt::AscendingOrder);
    d->exifToolFormats->setSelectionMode(QAbstractItemView::SingleSelection);
    d->exifToolFormats->setAllColumnsShowFocus(true);
    d->exifToolFormats->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    d->exifToolFormats->setColumnCount(4);
    d->exifToolFormats->setHeaderHidden(false);
    d->exifToolFormats->setHeaderLabels(QStringList() << i18nc("@title: column", "Extension")
                                                      << i18nc("@title: column", "Read")
                                                      << i18nc("@title: column", "Write")
                                                      << i18nc("@title: column", "Description"));

    d->searchBar                 = new SearchTextBar(this, QLatin1String("ExifToolFormatsSearchBar"));

    QVBoxLayout* const vlay      = new QVBoxLayout(exifToolBox);
    vlay->addWidget(d->exifToolFormats);
    vlay->addWidget(d->searchBar);

    grid->addWidget(exifToolBinLabel,     0, 0, 1, 2);
    grid->addWidget(d->exifToolBinWidget, 1, 0, 1, 2);
    grid->addWidget(exifToolBox,          2, 0, 1, 2);
    grid->setRowStretch(2, 10);
    setLayout(grid);

    // ---

    connect(d->searchBar, SIGNAL(signalSearchTextSettings(SearchTextSettings)),
            this, SLOT(slotSearchTextChanged(SearchTextSettings)));

    connect(d->exifToolBinWidget, SIGNAL(signalBinariesFound(bool)),
            this, SLOT(slotExifToolBinaryFound(bool)));

    foreach (const QString& path, MetaEngineSettings::instance()->settings().defaultExifToolSearchPaths())
    {
        d->exifToolBinWidget->addDirectory(path);
    }

    d->exifToolBinWidget->allBinariesFound();
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

void ExifToolConfPanel::slotExifToolBinaryFound(bool found)
{
    if (!found)
    {
        return;
    }

    d->exifToolFormats->clear();
    ExifToolParser* const parser = new ExifToolParser(this);
    parser->setExifToolProgram(exifToolDirectory());
    ExifToolParser::ExifToolData parsed;
    QStringList read;
    QStringList write;

    if (parser->readableFormats())
    {
        parsed = parser->currentData();
        read   = parsed.find(QLatin1String("READ_FORMATS")).value()[0].toStringList();
    }

    if (parser->writableFormats())
    {
        parsed = parser->currentData();
        write  = parsed.find(QLatin1String("WRITE_FORMATS")).value()[0].toStringList();
    }

    QString ext;
    QString desc;

    for (int i = 0 ; i < read.size()  ; i += 2)
    {
        ext  = read[i];
        desc = read[i + 1];
        new QTreeWidgetItem(d->exifToolFormats, QStringList() << ext
                                                              << i18nc("@info: status", "yes")
                                                              << (write.contains(ext) ? i18nc("@info: status", "yes") : i18nc("@info: status", "no"))
                                                              << desc);
    }
}

void ExifToolConfPanel::slotSearchTextChanged(const SearchTextSettings& settings)
{
    bool query     = false;
    int  results   = 0;
    QString search = settings.text.toLower();

    QTreeWidgetItemIterator it(d->exifToolFormats);

    while (*it)
    {
        QTreeWidgetItem* const item  = *it;

        if (item->text(0).toLower().contains(search, settings.caseSensitive) ||
            item->text(3).toLower().contains(search, settings.caseSensitive))
        {
            ++results;
            query = true;
            item->setHidden(false);
        }
        else
        {
            item->setHidden(true);
        }

        ++it;
    }

    d->searchBar->slotSearchResult(query);
}

} // namespace Digikam
