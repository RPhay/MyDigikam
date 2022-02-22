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

#ifndef DIGIKAM_SEARCH_GROUP_P_H
#define DIGIKAM_SEARCH_GROUP_P_H

#include "searchgroup.h"

// Qt includes

#include <QGridLayout>
#include <QLabel>
#include <QPainter>
#include <QRadioButton>
#include <QStackedLayout>
#include <QVBoxLayout>
#include <QStandardPaths>
#include <QIcon>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "searchfieldgroup.h"
#include "searchfields.h"
#include "searchutilities.h"
#include "searchview.h"

namespace Digikam
{

class Q_DECL_HIDDEN RadioButtonHBox : public QHBoxLayout
{
    Q_OBJECT

public:

    RadioButtonHBox(QWidget* const left, QWidget* const right, Qt::LayoutDirection dir);
};

} // namepace Digikam

#endif // DIGIKAM_SEARCH_GROUP_P_H
