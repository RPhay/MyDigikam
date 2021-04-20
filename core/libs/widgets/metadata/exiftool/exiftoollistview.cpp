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

// Local includes

#include "exiftoollistviewgroup.h"
#include "exiftoollistviewitem.h"

namespace Digikam
{

ExifToolListView::ExifToolListView(QWidget* const parent)
    : QTreeWidget(parent)
{
    setSortingEnabled(true);
    sortByColumn(0, Qt::AscendingOrder);
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setAllColumnsShowFocus(true);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setColumnCount(2);
    setHeaderHidden(true);
    header()->setSectionResizeMode(QHeaderView::Stretch);

    m_parser = new ExifToolParser(this);
    m_parser->setTranslations(false);
}

ExifToolListView::~ExifToolListView()
{
}

bool ExifToolListView::loadFromUrl(const QUrl& url)
{
    clear();

    if (!url.isValid())
    {
        return true;
    }

    if (!m_parser->load(url.toLocalFile()))
    {
        return false;
    }

    setMetadata(m_parser->currentParsedTags());

    return true;
}

void ExifToolListView::setMetadata(const ExifToolParser::TagsMap& map)
{
    for (ExifToolParser::TagsMap::const_iterator it = map.constBegin() ;
         it != map.constEnd() ; ++it)
    {
        QString grp                   = it.key().section(QLatin1Char('.'), 0, 0)
                                                .replace(QLatin1Char('_'), QLatin1Char(' '));

        if (grp == QLatin1String("ExifTool"))
        {
            continue;
        }

        QString name                  = it.key().section(QLatin1Char('.'), -1);
        QString value                 = it.value()[1].toString();
        QString desc                  = it.value()[3].toString();
        ExifToolListViewGroup* igroup = findGroup(grp);

        if (!igroup)
        {
            igroup = new ExifToolListViewGroup(this, grp);
        }

        new ExifToolListViewItem(igroup, name, value, desc);
    }
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

} // namespace Digikam
