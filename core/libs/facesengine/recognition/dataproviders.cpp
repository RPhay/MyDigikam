/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2013-05-18
 * Description : Wrapper class for face recognition
 *
 * SPDX-FileCopyrightText: 2013      by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * SPDX-FileCopyrightText: 2014-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
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
    QList<QPair<QImage*, QString> >::iterator img = list.begin();

    while (img != list.end())
    {
        delete (*img).first;
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

QPair<QImage*, QString> QListImageListProvider::image()
{
    return *it;
}

QList<QPair<QImage*, QString> > QListImageListProvider::images()
{
    return list;
}

void QListImageListProvider::setImages(const QList<QPair<QImage*, QString> >& lst)
{
    list = lst;
    it   = list.constBegin();
}

void QListImageListProvider::setUnpairedImages(const QList<QImage*>& lst)
{
    QList<QImage*>::const_iterator i;

    for (i = lst.begin() ; i != lst.end() ; ++i)
    {
        // Add to QPair list here.

        list << QPair<QImage*, QString>(*i, QString());
    }
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

QPair<QImage*, QString> EmptyImageListProvider::image()
{
    return QPair<QImage*, QString>(nullptr, QString());
}

QList<QPair<QImage*, QString> > EmptyImageListProvider::images()
{
    return QList<QPair<QImage*, QString> >();
}

void EmptyImageListProvider::setImages(const QList<QPair<QImage*, QString> >&)
{
}

void EmptyImageListProvider::setUnpairedImages(const QList<QImage*>&)
{
}

} // namespace Digikam
