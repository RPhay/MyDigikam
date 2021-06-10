/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-04-18
 * Description : ExifTool error view.
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

#include "exiftoolloadingview.h"

// Qt includes

#include <QLabel>
#include <QApplication>
#include <QStyle>
#include <QTimer>
#include <QGridLayout>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "dworkingpixmap.h"

namespace Digikam
{

class Q_DECL_HIDDEN ExifToolLoadingView::Private
{

public:

    explicit Private()
        : msgLbl          (nullptr),
          busy            (false),
          progressCount   (0),
          progressPix     (nullptr),
          progressTimer   (nullptr),
          progressLabel   (nullptr)
    {
    }

    QLabel*         msgLbl;

    bool            busy;
    int             progressCount;
    DWorkingPixmap* progressPix;
    QTimer*         progressTimer;
    QLabel*         progressLabel;
};

ExifToolLoadingView::ExifToolLoadingView(QWidget* const parent)
    : QWidget(parent),
      d      (new Private)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    const int spacing        = QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing);

    QGridLayout* const grid  = new QGridLayout(this);

    d->progressPix           = new DWorkingPixmap(this);
    d->progressLabel         = new QLabel(this);
    d->progressLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    d->msgLbl                = new QLabel(i18nc("info", "Loading in progress"), this);
    d->msgLbl->setAlignment(Qt::AlignCenter);
    d->msgLbl->setWordWrap(true);

    grid->addWidget(d->progressLabel, 1, 1, 1, 1);
    grid->addWidget(d->msgLbl,        2, 1, 1, 1);
    grid->setColumnStretch(0, 10);
    grid->setColumnStretch(2, 10);
    grid->setContentsMargins(spacing, spacing, spacing, spacing);
    grid->setRowStretch(0, 10);
    grid->setRowStretch(3, 10);

    d->progressTimer         = new QTimer(this);

    connect(d->progressTimer, &QTimer::timeout,
            this, &ExifToolLoadingView::slotProgressTimerDone);
}

ExifToolLoadingView::~ExifToolLoadingView()
{
    delete d;
}

void ExifToolLoadingView::setBusy(bool b)
{
    d->busy = b;

    if (d->busy)
    {
        setCursor(Qt::WaitCursor);
        d->progressTimer->start(300);
    }
    else
    {
        unsetCursor();
        d->progressTimer->stop();
        d->progressLabel->setPixmap(QPixmap());
    }
}

void ExifToolLoadingView::slotProgressTimerDone()
{
    d->progressLabel->setPixmap(d->progressPix->frameAt(d->progressCount));
    d->progressCount++;

    if (d->progressCount == 8)
    {
        d->progressCount = 0;
    }

    d->progressTimer->start(300);
}

} // namespace Digikam
