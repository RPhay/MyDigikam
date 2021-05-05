/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2013-05-18
 * Description : Wrapper class for face recognition
 *
 * Copyright (C)      2013 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2014-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "dataproviders.h"

namespace Digikam
{

QListImageListProvider::QListImageListProvider()
    : it(list.constBegin())
{
}

QListImageListProvider::~QListImageListProvider()
{
    QList<QImage*>::iterator img = list.begin();

    while (img != list.end())
    {
        delete *img;
        img = list.erase(img);
    }
}

int  QListImageListProvider::size()  const
{
    return list.size();
}

bool QListImageListProvider::atEnd() const
{
    return (it == list.constEnd());
}

void QListImageListProvider::proceed(int steps)
{
    it += steps;
}

void QListImageListProvider::reset()
{
    it = list.constBegin();
}

QImage* QListImageListProvider::image()
{
    return *it;
}

QList<QImage*> QListImageListProvider::images()
{
    return list;
}

void QListImageListProvider::setImages(const QList<QImage*>& lst)
{
    list = lst;
    it   = list.constBegin();
}

// ----------------------------------------------------------------------------------------

int  EmptyImageListProvider::size()  const
{
    return 0;
}

bool EmptyImageListProvider::atEnd() const
{
    return true;
}

void EmptyImageListProvider::proceed(int steps)
{
    Q_UNUSED(steps)
}

QImage* EmptyImageListProvider::image()
{
    return nullptr;
}

QList<QImage*> EmptyImageListProvider::images()
{
    return QList<QImage*>();
}

void EmptyImageListProvider::setImages(const QList<QImage*>&)
{
}

} // namespace Digikam
