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

#include <QLayout>
#include <QPushButton>
#include <QApplication>
#include <QStyle>

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

SetupVideo::SetupVideo(QWidget* const parent)
    : QScrollArea(parent)
{
    QWidget* const panel   = new QWidget(viewport());
    setWidget(panel);
    setWidgetResizable(true);

    QVBoxLayout* const vbl = new QVBoxLayout();
    panel->setLayout(vbl);

    mpContent              = new QTabWidget(this);
    mpContent->setTabPosition(QTabWidget::North);
/*
    mpButtonBox            = new QDialogButtonBox(Qt::Horizontal);
    mpButtonBox->addButton(i18n("Reset"),  QDialogButtonBox::ResetRole);      // QDialogButtonBox::Reset;
    mpButtonBox->addButton(i18n("Ok"),     QDialogButtonBox::AcceptRole);     // QDialogButtonBox::Ok
    mpButtonBox->addButton(i18n("Cancel"), QDialogButtonBox::RejectRole);
    mpButtonBox->addButton(i18n("Apply"),  QDialogButtonBox::ApplyRole);

    connect(mpButtonBox, SIGNAL(clicked(QAbstractButton*)),
            this, SLOT(onButtonClicked(QAbstractButton*)));
*/
    vbl->addWidget(mpContent);
    //vbl->addWidget(mpButtonBox);

    mPages << new DecoderConfigPage(nullptr, false)
           << new AVFormatConfigPage()
           << new MiscPage()
    ;

    readSettings();
}

SetupVideo::~SetupVideo()
{
//    delete d;
}

void SetupVideo::applySettings()
{
    foreach (ConfigPageBase* const page, mPages)
    {
        page->apply();
    }

    AVPlayerConfigMngr::instance().save();
}

void SetupVideo::readSettings()
{
    foreach (ConfigPageBase* const page, mPages)
    {
        page->applyToUi();
        page->applyOnUiChange(false);
        mpContent->addTab(page, page->name());
    }
}

void SetupVideo::slotReset()
{
    AVPlayerConfigMngr::instance().reset();

    foreach (ConfigPageBase* const page, mPages)
    {
        page->reset();
    }
}

void SetupVideo::cancel()
{
    foreach (ConfigPageBase* const page, mPages)
    {
        page->cancel();
    }
}

} // namespace Digikam
