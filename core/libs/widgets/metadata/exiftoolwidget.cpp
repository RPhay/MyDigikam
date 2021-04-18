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

#include "exiftoolwidget.h"

// Qt includes

#include <QHeaderView>
#include <QFont>

namespace Digikam
{

ExifToolListViewGroup::ExifToolListViewGroup(ExifToolListView* const parent, const QString&  group)
    : QTreeWidgetItem(parent)
{
    setFlags(Qt::ItemIsEnabled);
    setExpanded(true);
    setDisabled(false);
    QFont fn0(font(0));
    fn0.setBold(true);
    fn0.setItalic(false);
    setFont(0, fn0);
    setText(0, group);
}

ExifToolListViewGroup::~ExifToolListViewGroup()
{
}

// ---------------------------------------------------------------------------

ExifToolListViewItem::ExifToolListViewItem(ExifToolListViewGroup* const parent, const QStringList& data)
    : QTreeWidgetItem(parent)
{
    setDisabled(false);
    setSelected(false);
    setText(0, data[0]);
    setText(1, data[1]);
}

ExifToolListViewItem::~ExifToolListViewItem()
{
}

// ---------------------------------------------------------------------------

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
        QString grp                   = it.key().section(QLatin1Char('.'), 0, 0);

        if (grp == QLatin1String("ExifTool"))
        {
            continue;
        }

        QString name                  = it.key().section(QLatin1Char('.'), -1);
        QString data                  = it.value()[1].toString();
        ExifToolListViewGroup* igroup = findGroup(grp);

        if (!igroup)
        {
            igroup = new ExifToolListViewGroup(this, grp);
        }

        new ExifToolListViewItem(igroup, QStringList() << name << data);
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

} // namespace Digikam
