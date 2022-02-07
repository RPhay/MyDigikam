/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-05-21
 * Description : widget to display a list of items
 *
 * Copyright (C) 2006-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2008-2010 by Andi Clemens <andi dot clemens at googlemail dot com>
 * Copyright (C) 2009-2010 by Luka Renko <lure at kubuntu dot org>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef DIGIKAM_DITEMS_LIST_P_H
#define DIGIKAM_DITEMS_LIST_P_H

#include "ditemslist.h"

// Qt includes

#include <QDragEnterEvent>
#include <QFileInfo>
#include <QGridLayout>
#include <QGroupBox>
#include <QMimeData>
#include <QHeaderView>
#include <QLabel>
#include <QPainter>
#include <QPushButton>
#include <QUrl>
#include <QTimer>
#include <QFile>
#include <QPointer>
#include <QXmlStreamAttributes>
#include <QStringRef>
#include <QString>
#include <QStandardPaths>
#include <QIcon>
#include <QApplication>
#include <QStyle>

// KDE includes

#include <klocalizedstring.h>
#include <ksharedconfig.h>
#include <kconfiggroup.h>

// Local includes

#include "drawdecoder.h"
#include "imagedialog.h"
#include "digikam_debug.h"
#include "dlayoutbox.h"
#include "dfiledialog.h"
#include "thumbnailloadthread.h"
#include "dworkingpixmap.h"

namespace Digikam
{

class Q_DECL_HIDDEN CtrlButton : public QPushButton
{
    Q_OBJECT

public:

    explicit CtrlButton(const QIcon& icon, QWidget* parent = nullptr)
       : QPushButton(parent)
    {
        const int btnSize = 32;

        setMinimumSize(btnSize, btnSize);
        setMaximumSize(btnSize, btnSize);
        setIcon(icon);
    }

    ~CtrlButton() override
    {
    }
};

} // namespace Digikam

#endif // DIGIKAM_DITEMS_LIST_P_H
