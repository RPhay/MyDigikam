/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-04-13
 * Description : ExifTool tags translator for Exiv2
 *
 * Copyright (C) 2020-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "exiftooltranslator.h"

// Qt includes

#include <QHash>
#include <QStringList>

// Local includes

#include "exiftooltranslator_p.h"

namespace Digikam
{

class Q_DECL_HIDDEN ExifToolTranslatorCreator
{
public:

    ExifToolTranslator object;
};

Q_GLOBAL_STATIC(ExifToolTranslatorCreator, creator)

ExifToolTranslator* ExifToolTranslator::instance()
{
    return &creator->object;
}

// --------------------------------------------------------------------------

ExifToolTranslator::ExifToolTranslator()
    : d(new Private)
{
}

ExifToolTranslator::~ExifToolTranslator()
{
    delete d;
}

bool ExifToolTranslator::isBlackListedGroup(const QString& group) const
{
    foreach (const QString& bgrp, d->ignoreGroupsET)
    {
        if (group.startsWith(bgrp))
        {
            return true;
        }
    }

    return false;
}

QString ExifToolTranslator::translateToExiv2(const QString& tagName) const
{
    QHash<QString, QString>::iterator it = d->mapETtoExiv2.find(tagName);

    if (it != d->mapETtoExiv2.end())
    {
        return it.value();
    }

    return QString();
}

QString ExifToolTranslator::translateToExifTool(const QString& tagName) const
{
    QHash<QString, QString>::iterator it = d->mapExiv2toET.find(tagName);

    if (it != d->mapExiv2toET.end())
    {
        return it.value();
    }

    return QString();
}

} // namespace Digikam
