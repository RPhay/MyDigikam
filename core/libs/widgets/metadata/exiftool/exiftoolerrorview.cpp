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

#include "exiftoolerrorview.h"

// Qt includes

#include <QLabel>
#include <QApplication>
#include <QStyle>
#include <QGridLayout>
#include <QPushButton>

// KDE includes

#include <klocalizedstring.h>

namespace Digikam
{

class Q_DECL_HIDDEN ExifToolErrorView::Private
{

public:

    explicit Private()
        : errorLbl(nullptr),
          btn     (nullptr)
    {
    }

    QLabel*           errorLbl;

    QPushButton*      btn;
};

ExifToolErrorView::ExifToolErrorView(QWidget* const parent)
    : QWidget(parent),
      d      (new Private)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    const int spacing        = QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing);

    QGridLayout* const grid  = new QGridLayout(this);

    d->errorLbl              = new QLabel(this);
    d->errorLbl->setAlignment(Qt::AlignCenter);
    d->errorLbl->setWordWrap(true);

    d->btn                   = new QPushButton(this);
    d->btn->setText(i18nc("@action: button", "Open Setup Dialog..."));

    connect(d->btn, SIGNAL(clicked()),
            this, SIGNAL(signalSetupExifTool()));

    grid->addWidget(d->errorLbl, 1, 1, 1, 1);
    grid->addWidget(d->btn,      2, 1, 1, 1);
    grid->setColumnStretch(0, 10);
    grid->setColumnStretch(2, 10);
    grid->setContentsMargins(spacing, spacing, spacing, spacing);
    grid->setRowStretch(0, 10);
    grid->setRowStretch(3, 10);
}

ExifToolErrorView::~ExifToolErrorView()
{
    delete d;
}

void ExifToolErrorView::setErrorText(const QString& err)
{
    d->errorLbl->setText(err);
}

} // namespace Digikam
