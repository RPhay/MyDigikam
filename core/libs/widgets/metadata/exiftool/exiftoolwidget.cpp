/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-04-18
 * Description : ExifTool metadata widget.
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

#include "exiftoolwidget.h"

// Qt includes

#include <QLabel>
#include <QGridLayout>
#include <QPushButton>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "exiftoollistviewgroup.h"
#include "exiftoollistviewitem.h"
#include "exiftoollistview.h"
#include "searchtextbar.h"

namespace Digikam
{

class Q_DECL_HIDDEN ExifToolWidget::Private
{
public:

    enum ViewMode
    {
        MetadataView = 0,
        ErrorView
    };

public:

    explicit Private()
        : metadataView(nullptr),
          errorView   (nullptr),
          view        (nullptr),
          searchBar   (nullptr)
    {
    }

    QWidget*          metadataView;
    QWidget*          errorView;
    ExifToolListView* view;
    SearchTextBar*    searchBar;
};

ExifToolWidget::ExifToolWidget(QWidget* const parent)
    : QStackedWidget(parent),
      d             (new Private)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // ---

    d->metadataView          = new QWidget(this);
    QGridLayout* const grid2 = new QGridLayout(d->metadataView);

    d->view                  = new ExifToolListView(d->metadataView);
    d->searchBar             = new SearchTextBar(d->metadataView, QLatin1String("ExifToolSearchBar"));

    grid2->addWidget(d->searchBar, 0, 1, 1, 1);
    grid2->addWidget(d->view,      1, 1, 1, 1);

    // ---

    d->errorView             = new QWidget(this);
    QGridLayout* const grid  = new QGridLayout(d->errorView);

    QLabel* const errorLbl   = new QLabel(d->errorView);
    errorLbl->setAlignment(Qt::AlignCenter);
    errorLbl->setText(i18n("Cannot load data\nwith ExifTool.\nCheck your configuration."));

    QPushButton* const btn   = new QPushButton(d->errorView);
    btn->setText(i18n("Open Setup Dialog..."));

    connect(btn, SIGNAL(clicked()),
            this, SIGNAL(signalSetupExifTool()));

    grid->addWidget(errorLbl, 1, 1, 1, 1);
    grid->addWidget(btn,      2, 1, 1, 1);
    grid->setColumnStretch(0, 10);
    grid->setColumnStretch(2, 10);
    grid->setRowStretch(0, 10);
    grid->setRowStretch(3, 10);

    insertWidget(Private::MetadataView, d->metadataView);
    insertWidget(Private::ErrorView,    d->errorView);

    setCurrentIndex(Private::MetadataView);

    connect(d->searchBar, SIGNAL(signalSearchTextSettings(SearchTextSettings)),
            d->view, SLOT(slotSearchTextChanged(SearchTextSettings)));

    connect(d->view, SIGNAL(signalTextFilterMatch(bool)),
            d->searchBar, SLOT(slotSearchResult(bool)));
}

ExifToolWidget::~ExifToolWidget()
{
    delete d;
}

void ExifToolWidget::loadFromUrl(const QUrl& url)
{
    bool ret = d->view->loadFromUrl(url);

    if (ret)
    {
        setCurrentIndex(Private::MetadataView);
    }
    else
    {
        setCurrentIndex(Private::ErrorView);
    }
}

} // namespace Digikam
