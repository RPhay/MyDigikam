/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-09-27
 * Description : Showfoto stack view favorites item
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

#include "showfotostackviewfavoriteitem.h"

// KDE includes

#include <klocalizedstring.h>

// Local include

#include "digikam_debug.h"

namespace ShowFoto
{

ShowfotoStackViewFavoriteItem::ShowfotoStackViewFavoriteItem(QTreeWidgetItem* const parent)
    : QTreeWidgetItem(parent)
{
    setDisabled(false);
    setSelected(false);
}

ShowfotoStackViewFavoriteItem::~ShowfotoStackViewFavoriteItem()
{
}

void ShowfotoStackViewFavoriteItem::setName(const QString& name)
{
    setText(0, name);

    updateToolTip();
}

QString ShowfotoStackViewFavoriteItem::name() const
{
    return text(0);
}

void ShowfotoStackViewFavoriteItem::setDescription(const QString& desc)
{
    m_desc = desc;

    updateToolTip();
}

QString ShowfotoStackViewFavoriteItem::description() const
{
    return m_desc;
}

void ShowfotoStackViewFavoriteItem::setUrls(const QList<QUrl>& urls)
{
    m_urls = urls;

    updateToolTip();
}

QList<QUrl> ShowfotoStackViewFavoriteItem::urls() const
{
    return m_urls;
}

QStringList ShowfotoStackViewFavoriteItem::urlsToPaths() const
{
    QStringList files;

    foreach (const QUrl& url, urls())
    {
        files << url.toLocalFile();
    }

    return files;
}

void ShowfotoStackViewFavoriteItem::updateToolTip()
{
    QString desc = description().isEmpty() ? QLatin1String("-") : description();
    QString tip  = i18n("Name: %1\n", name());
    tip         += i18n("Items: %1\n", urls().count());
    tip         += i18n("Description: %1", desc);

    setToolTip(0, tip);
}

} // namespace ShowFoto
