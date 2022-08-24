/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2022-05-12
 * Description : video setup page.
 *
 * Copyright (C) 2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "setupvideo.h"

// Qt includes

#include <QApplication>
#include <QLayout>
#include <QPushButton>
#include <QTabWidget>
#include <QList>
#include <QMessageBox>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "DecoderConfigPage.h"
#include "AVFormatConfigPage.h"
#include "MiscPage.h"
#include "AVPlayerConfigMngr.h"
#include "digikam_debug.h"

using namespace QtAV;

namespace Digikam
{

class Q_DECL_HIDDEN SetupVideo::Private
{
public:

    explicit Private()
      : tab     (nullptr),
        resetBtn(nullptr)
    {
    }

    QTabWidget*                  tab;
    QPushButton*                 resetBtn;
    QList<QtAV::ConfigPageBase*> pages;
};

SetupVideo::SetupVideo(QWidget* const parent)
    : QScrollArea(parent),
      d          (new Private)
{
    QWidget* const panel   = new QWidget(viewport());
    setWidget(panel);
    setWidgetResizable(true);

    QVBoxLayout* const vbl = new QVBoxLayout();
    panel->setLayout(vbl);

    d->tab                 = new QTabWidget(this);
    d->tab->setTabPosition(QTabWidget::North);

    d->resetBtn            = new QPushButton(this);
    d->resetBtn->setText(i18nc("@action", "Reset"));
    d->resetBtn->setToolTip(i18nc("@info", "Press this button to reset all video settings to the default values."));

    connect(d->resetBtn, SIGNAL(clicked()),
            this, SLOT(slotReset()));

    vbl->addWidget(d->tab);
    vbl->addWidget(d->resetBtn);

    d->pages << new DecoderConfigPage(nullptr, false)
             << new AVFormatConfigPage()
             << new MiscPage(nullptr, false)
    ;

    readSettings();
}

SetupVideo::~SetupVideo()
{
    delete d;
}

void SetupVideo::setActiveTab(VideoTab tab)
{
    d->tab->setCurrentIndex(tab);
}

SetupVideo::VideoTab SetupVideo::activeTab() const
{
    return (VideoTab)d->tab->currentIndex();
}

void SetupVideo::applySettings()
{
    Q_FOREACH (ConfigPageBase* const page, d->pages)
    {
        page->apply();
    }

    AVPlayerConfigMngr::instance().save();
}

void SetupVideo::readSettings()
{
    Q_FOREACH (ConfigPageBase* const page, d->pages)
    {
        page->applyToUi();
        page->applyOnUiChange(false);
        d->tab->addTab(page, page->name());
    }
}

void SetupVideo::slotReset()
{
    int result = QMessageBox::warning(this, qApp->applicationName(),
                                      i18nc("@info",
                                            "Do you want to reset all video settings to the default value?"),
                                            QMessageBox::Yes | QMessageBox::No);

    if (result == QMessageBox::Yes)
    {
        AVPlayerConfigMngr::instance().reset();

        Q_FOREACH (ConfigPageBase* const page, d->pages)
        {
            page->reset();
        }
    }
}

void SetupVideo::cancel()
{
    Q_FOREACH (ConfigPageBase* const page, d->pages)
    {
        page->cancel();
    }
}

} // namespace Digikam
