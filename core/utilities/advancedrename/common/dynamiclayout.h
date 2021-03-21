/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-10-22
 * Description : a dynamic layout manager
 *
 * Copyright (C) 2009-2012 by Andi Clemens <andi dot clemens at gmail dot com>
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

#ifndef DIGIKAM_DYNAMIC_LAYOUT_H
#define DIGIKAM_DYNAMIC_LAYOUT_H

// Qt includes

#include <QLayout>

class QLayoutItem;
class QRect;
class QWidget;

namespace Digikam
{

class DynamicLayout : public QLayout
{
    Q_OBJECT

public:

    explicit DynamicLayout(QWidget* const parent, int margin = -1,
                           int hSpacing = 0, int vSpacing = 0);
    explicit DynamicLayout(int margin = -1, int hSpacing = 0,
                           int vSpacing = 0);
    ~DynamicLayout()                              override;

    void addItem(QLayoutItem* item)               override;

    int  horizontalSpacing()                const;
    int  verticalSpacing()                  const;

    Qt::Orientations expandingDirections()  const override;

    bool hasHeightForWidth()                const override;
    int  heightForWidth(int)                const override;

    int count()                             const override;

    QLayoutItem* itemAt(int index)          const override;
    QLayoutItem* takeAt(int index)                override;

    QSize minimumSize()                     const override;

    void  setGeometry(const QRect& rect)          override;
    QSize sizeHint()                        const override;


private:

    int reLayout(const QRect& rect,
                 bool testOnly)             const;

private:

    // Disable
    DynamicLayout(const DynamicLayout&)            = delete;
    DynamicLayout& operator=(const DynamicLayout&) = delete;

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_DYNAMIC_LAYOUT_H
