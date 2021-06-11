/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-04-18
 * Description : ExifTool metadata list view.
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

#include "exiftoollistview.h"

// Qt includes

#include <QHeaderView>
#include <QApplication>
#include <QStringList>
#include <QStyle>
#include <QtConcurrent>   // krazy:exclude=includes

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
    {
    }

    QString                       lastError;
    QString                       selectedItemKey;
    QStringList                   simplifiedTagsList;
};

ExifToolListView::ExifToolListView(QWidget* const parent)
    : QTreeWidget(parent),
      d          (new Private)
{
    setSortingEnabled(true);
    sortByColumn(0, Qt::AscendingOrder);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setAllColumnsShowFocus(true);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setColumnCount(2);
    setIndentation(QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing));
    setHeaderHidden(true);
    header()->setSectionResizeMode(QHeaderView::Stretch);

    connect(this, SIGNAL(itemClicked(QTreeWidgetItem*,int)),
            this, SLOT(slotSelectionChanged(QTreeWidgetItem*,int)));
}

ExifToolListView::~ExifToolListView()
{
    delete d;
}

void ExifToolListView::exifToolParseThreaded(const QString& file,
                                             ExifToolParser::ExifToolData* const parsed,
                                             QString* const errorString,
                                             bool* const error)
{
    ExifToolParser* const parser = new ExifToolParser(nullptr);

    if (!parser->load(file))
    {
        *errorString = parser->currentErrorString();
        *error       = true;
        delete parser;

        return;
    }

    errorString->clear();
    *parsed = parser->currentData();
    *error  = false;
    delete parser;
}

bool ExifToolListView::loadFromUrl(const QUrl& url)
{
    clear();

    if (!url.isValid())
    {
        return true;
    }

    bool error          = false;
    ExifToolParser::ExifToolData parsed;

    // Note; pass writable argument by pointer to QtConcurent::run()
    // For details: https://stackoverflow.com/questions/25091518/qt-concurrent-run-pass-value-by-reference-but-the-memory-address-is-different

    QFuture<void> task  = QtConcurrent::run(this,
                                            &ExifToolListView::exifToolParseThreaded,
                                            url.toLocalFile(),
                                            &parsed,
                                            &d->lastError,
                                            &error);

    task.waitForFinished();

    if (error)
    {
        return false;
    }

    setMetadata(parsed);

    return true;
}

QString ExifToolListView::errorString() const
{
    return d->lastError;
}

void ExifToolListView::setMetadata(const ExifToolParser::ExifToolData& map)
{
    d->simplifiedTagsList.clear();
    QString simplifiedTag;

    for (ExifToolParser::ExifToolData::const_iterator it = map.constBegin() ;
         it != map.constEnd() ; ++it)
    {
        QString grp                   = it.key().section(QLatin1Char('.'), 0, 0)
                                                .replace(QLatin1Char('_'), QLatin1Char(' '));

        if (grp == QLatin1String("ExifTool"))
        {
            continue;
        }

        QString key                   = it.key();
        QString value                 = it.value()[0].toString();
        QString desc                  = it.value()[2].toString();
        ExifToolListViewGroup* igroup = findGroup(grp);
        simplifiedTag                 = grp + QLatin1Char('.') + it.key().section(QLatin1Char('.'), -1);

        if (!d->simplifiedTagsList.contains(simplifiedTag))
        {
            d->simplifiedTagsList.append(simplifiedTag);

            if (!igroup)
            {
                igroup = new ExifToolListViewGroup(this, grp);
            }

            new ExifToolListViewItem(igroup, key, value, desc);
        }
    }

    setCurrentItemByKey(d->selectedItemKey);
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

    // Restore all MdKey items.

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

} // namespace Digikam
