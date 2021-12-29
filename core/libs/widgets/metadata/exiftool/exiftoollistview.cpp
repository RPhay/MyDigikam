/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-04-18
 * Description : ExifTool metadata list view.
 *
 * Copyright (C) 2021-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "exiftoollistview.h"

// Qt includes

#include <QApplication>
#include <QHeaderView>
#include <QStringList>
#include <QStyle>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "exiftoollistviewgroup.h"
#include "exiftoollistviewitem.h"
#include "digikam_debug.h"

namespace Digikam
{

class Q_DECL_HIDDEN ExifToolListView::Private
{

public:

    explicit Private()
      : parser(nullptr)
    {
    }

    QString                      lastError;
    QString                      selectedItemKey;
    QStringList                  simplifiedTagsList;

    ExifToolParser*              parser;
    ExifToolParser::ExifToolData map;
};

ExifToolListView::ExifToolListView(QWidget* const parent)
    : QTreeWidget(parent),
      d          (new Private)
{
    setColumnCount(2);
    setHeaderHidden(true);
    setSortingEnabled(true);
    setAllColumnsShowFocus(true);
    sortByColumn(0, Qt::AscendingOrder);
    setSelectionMode(QAbstractItemView::SingleSelection);
    header()->setSectionResizeMode(QHeaderView::Stretch);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    d->parser = new ExifToolParser(this);

    connect(d->parser, SIGNAL(signalExifToolDataAvailable()),
            this, SLOT(slotExifToolDataAvailable()));

    connect(this, SIGNAL(itemClicked(QTreeWidgetItem*,int)),
            this, SLOT(slotSelectionChanged(QTreeWidgetItem*,int)));
}

ExifToolListView::~ExifToolListView()
{
    delete d;
}

void ExifToolListView::loadFromUrl(const QUrl& url)
{
    clear();
    d->map.clear();

    if (!url.isValid())
    {
        emit signalLoadingResult(true);

        return;
    }

    if (!d->parser->load(url.toLocalFile(), true))
    {
        d->lastError = d->parser->currentErrorString();

        emit signalLoadingResult(false);

        return;
    }

    d->lastError.clear();
}

QString ExifToolListView::errorString() const
{
    return d->lastError;
}

void ExifToolListView::slotExifToolDataAvailable()
{
    d->lastError = d->parser->currentErrorString();

    setMetadata(d->parser->currentData());

    emit signalLoadingResult(d->lastError.isEmpty());
}

ExifToolListViewGroup* ExifToolListView::findGroup(const QString& group)
{
    QTreeWidgetItemIterator it(this);

    while (*it)
    {
        ExifToolListViewGroup* const item = dynamic_cast<ExifToolListViewGroup*>(*it);

        if (item && (item->text(0) == group))
        {
            return item;
        }

        ++it;
    }

    return nullptr;
}

void ExifToolListView::slotSearchTextChanged(const SearchTextSettings& settings)
{
    bool query     = false;
    QString search = settings.text;

    // Restore all Group items.

    QTreeWidgetItemIterator it2(this);

    while (*it2)
    {
        ExifToolListViewGroup* const item = dynamic_cast<ExifToolListViewGroup*>(*it2);

        if (item)
        {
            item->setHidden(false);
        }

        ++it2;
    }

    QTreeWidgetItemIterator it(this);

    while (*it)
    {
        ExifToolListViewItem* const item = dynamic_cast<ExifToolListViewItem*>(*it);

        if (item)
        {
            if (item->text(0).contains(search, settings.caseSensitive) ||
                item->text(1).contains(search, settings.caseSensitive))
            {
                query = true;
                item->setHidden(false);
            }
            else
            {
                item->setHidden(true);
            }
        }

        ++it;
    }

    emit signalTextFilterMatch(query);
}

QString ExifToolListView::getCurrentItemKey() const
{
    if (currentItem() && (currentItem()->flags() & Qt::ItemIsSelectable))
    {
        ExifToolListViewItem* const item = static_cast<ExifToolListViewItem*>(currentItem());

        return item->getKey();
    }

    return QString();
}

void ExifToolListView::setCurrentItemByKey(const QString& itemKey)
{
    if (itemKey.isNull())
    {
        return;
    }

    QTreeWidgetItemIterator it(this);

    while (*it)
    {
        ExifToolListViewItem* const item = dynamic_cast<ExifToolListViewItem*>(*it);

        if (item)
        {
            if (item->getKey() == itemKey)
            {
                setCurrentItem(item);
                scrollToItem(item);
                d->selectedItemKey = itemKey;

                return;
            }
        }

        ++it;
    }
}

void ExifToolListView::slotSelectionChanged(QTreeWidgetItem* item, int)
{
    ExifToolListViewItem* const viewItem = dynamic_cast<ExifToolListViewItem*>(item);

    if (!viewItem)
    {
        return;
    }

    d->selectedItemKey                   = viewItem->getKey();
    QString tagValue                     = viewItem->getValue();
    QString tagTitle                     = viewItem->getTitle();
    QString tagDesc                      = viewItem->getDescription();

    if (tagValue.length() > 128)
    {
        tagValue.truncate(128);
        tagValue.append(QLatin1String("..."));
    }

    this->setWhatsThis(i18n("<b>Title: </b><p>%1</p>"
                            "<b>Value: </b><p>%2</p>"
                            "<b>Description: </b><p>%3</p>",
                            tagTitle, tagValue, tagDesc));
}

void ExifToolListView::setMetadata(const ExifToolParser::ExifToolData& map)
{
    d->map = map;
}

void ExifToolListView::setGroupList(const QStringList& tagsFilter, const QStringList& keysFilter)
{
    clear();
    d->simplifiedTagsList.clear();
    QString simplifiedTag;

    QStringList filters = tagsFilter;
    QString groupItemName;

    /** Key is formatted like this:
     *
     * EXIF.ExifIFD.Image.ExposureCompensation
     * File.File.Other.FileType
     * Composite.Composite.Time.SubSecModifyDate
     * File.System.Time.FileInodeChangeDate
     * File.System.Other.FileSize
     * EXIF.GPS.Location.GPSLongitude
     * ICC_Profile.ICC-header.Image.ProfileCreator
     * EXIF.IFD1.Image.ThumbnailOffset
     * JFIF.JFIF.Image.YResolution
     * ICC_Profile.ICC_Profile.Image.GreenMatrixColumn
     */
    for (ExifToolParser::ExifToolData::const_iterator it = d->map.constBegin() ;
         it != d->map.constEnd() ; ++it)
    {
        // We checking if we have changed of GroupName

        QString currentGroupName = it.key().section(QLatin1Char('.'), 0, 0)
                                           .replace(QLatin1Char('_'), QLatin1Char(' '));

        if (currentGroupName == QLatin1String("ExifTool"))
        {
            // Always ignore ExifTool errors or warnings.

            continue;
        }

        if (!keysFilter.isEmpty() && !keysFilter.contains(currentGroupName))
        {
            // If group filters, always ignore not found entries.

            continue;
        }

        ExifToolListViewGroup* parentGroupItem = findGroup(currentGroupName);
        simplifiedTag                          = currentGroupName + QLatin1Char('.') + it.key().section(QLatin1Char('.'), -1);

        if (!d->simplifiedTagsList.contains(simplifiedTag))
        {
            d->simplifiedTagsList.append(simplifiedTag);

            if (!parentGroupItem)
            {
                parentGroupItem = new ExifToolListViewGroup(this, currentGroupName);
            }

            if      (tagsFilter.isEmpty())
            {
                new ExifToolListViewItem(parentGroupItem, it.key(), it.value()[0].toString(), it.value()[2].toString());
            }
            else
            {
                // We ignore all unknown tags if necessary.

                if      (filters.contains(QLatin1String("FULL")))
                {
                    // We don't filter the output (Photo Mode)

                    new ExifToolListViewItem(parentGroupItem, it.key(), it.value()[0].toString(), it.value()[2].toString());
                }
                else if (!filters.isEmpty())
                {
                    // We using the filter to make a more user friendly output (Custom Mode)

                    // Filter is not a list of complete tag keys

                    if      (!filters.at(0).contains(QLatin1Char('.')) && filters.contains(it.key().section(QLatin1Char('.'), -1)))
                    {
                        new ExifToolListViewItem(parentGroupItem, it.key(), it.value()[0].toString(), it.value()[2].toString());
                        filters.removeAll(it.key());
                    }
                    else if (filters.contains(it.key()))
                    {
                        new ExifToolListViewItem(parentGroupItem, it.key(), it.value()[0].toString(), it.value()[2].toString());
                        filters.removeAll(it.key());
                    }
                }
            }
        }
    }

    // Add not found tags from filter as grey items.

    d->simplifiedTagsList.clear();

    if (!filters.isEmpty()                       &&
        (filters.at(0) != QLatin1String("FULL")) &&
        filters.at(0).contains(QLatin1Char('.')))
    {
        foreach (const QString& key, filters)
        {
            QString grp                  = key.section(QLatin1Char('.'), 0, 0)
                                              .replace(QLatin1Char('_'), QLatin1Char(' '));
            simplifiedTag                = grp + QLatin1Char('.') + key.section(QLatin1Char('.'), -1);
            ExifToolListViewGroup* pitem = findGroup(grp);

            if (!d->simplifiedTagsList.contains(simplifiedTag))
            {
                d->simplifiedTagsList.append(simplifiedTag);

                if (!pitem)
                {
                    pitem = new ExifToolListViewGroup(this, grp);
                }

                new ExifToolListViewItem(pitem, key);
            }
        }

        // Remove groups with no children.

        QTreeWidgetItemIterator it(this);

        while (*it)
        {
            ExifToolListViewGroup* const item = dynamic_cast<ExifToolListViewGroup*>(*it);

            if (item && !item->childCount())
            {
                delete item;
            }

            ++it;
        }
    }

    setCurrentItemByKey(d->selectedItemKey);
    update();
}

} // namespace Digikam
