/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-01-20
 * Description : User interface for searches
 *
 * Copyright (C) 2008-2012 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2011-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "searchfields_p.h"

namespace Digikam
{

SearchFieldKeyword::SearchFieldKeyword(QObject* const parent)
    : SearchFieldText(parent)
{
}

void SearchFieldKeyword::read(SearchXmlCachingReader& reader)
{
    QString keyword = reader.value();
    m_edit->setText(KeywordSearch::merge(m_edit->text(), keyword));
}

void SearchFieldKeyword::write(SearchXmlWriter& writer)
{
    QStringList keywordList = KeywordSearch::split(m_edit->text());

    Q_FOREACH (const QString& keyword, keywordList)
    {
        if (!keyword.isEmpty())
        {
            writer.writeField(m_name, SearchXml::Like);
            writer.writeValue(keyword);
            writer.finishField();
        }
    }
}

} // namespace Digikam
