/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-04-18
 * Description : ExifTool metadata list view item.
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

#include "exiftoollistviewitem.h"

// Qt includes

#include <QHeaderView>
#include <QFont>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "ditemtooltip.h"
#include "exiftoollistviewgroup.h"

namespace Digikam
{

class Q_DECL_HIDDEN ExifToolListViewItem::Private
{
public:

    explicit Private()
    {
    }

    QString key;
    QString desc;
};

ExifToolListViewItem::ExifToolListViewItem(ExifToolListViewGroup* const parent,
                                           const QString& key,
                                           const QString& value,
                                           const QString& desc)
    : QTreeWidgetItem(parent),
      d              (new Private)
{
    d->key  = key;
    d->desc = desc;

    setChildIndicatorPolicy(QTreeWidgetItem::DontShowIndicator);
    setDisabled(false);
    setSelected(false);

    QString name = d->key.section(QLatin1Char('.'), -1);
    setText(0, name);
    setToolTip(0, name);

    QString tagVal = value.simplified();

    if (tagVal.length() > 512)
    {
        tagVal.truncate(512);
        tagVal.append(QLatin1String("..."));
    }

    setText(1, tagVal);

    DToolTipStyleSheet cnt;
    setToolTip(1, QLatin1String("<qt><p>") + cnt.breakString(tagVal) + QLatin1String("</p></qt>"));
}

ExifToolListViewItem::ExifToolListViewItem(ExifToolListViewGroup* const parent,
                                           const QString& key)
    : QTreeWidgetItem(parent),
      d              (new Private)
{
    d->key = key;

    setChildIndicatorPolicy(QTreeWidgetItem::DontShowIndicator);

    QString name = d->key.section(QLatin1Char('.'), -1);
    setText(0, name);
    setToolTip(0, name);

    setDisabled(true);
    setText(1, i18n("Unavailable"));
    QFont fnt = font(1);
    fnt.setItalic(true);
    setFont(1, fnt);
}

ExifToolListViewItem::~ExifToolListViewItem()
{
    delete d;
}

QString ExifToolListViewItem::getKey() const
{
    return d->key;
}

QString ExifToolListViewItem::getTitle() const
{
    return text(0);
}

QString ExifToolListViewItem::getValue() const
{
    return text(1);
}

QString ExifToolListViewItem::getDescription() const
{
    return d->desc;
}

} // namespace Digikam
