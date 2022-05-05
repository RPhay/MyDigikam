/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-10-31
 * Description : QtAV: Multimedia framework based on Qt and FFmpeg
 *
 * Copyright (C) 2012-2022 Wang Bin <wbsecg1 at gmail dot com>
 * Copyright (C)      2022 by Gilles Caulier, <caulier dot gilles at gmail dot com>
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

#include "ConfigDialog.h"
#include <QFile>
#include <QLayout>
#include <QPushButton>
#include "CaptureConfigPage.h"
#include "DecoderConfigPage.h"
#include "AVFormatConfigPage.h"
#include "AVFilterConfigPage.h"
#include "MiscPage.h"
#include "ShaderPage.h"
#include "common/Config.h"
void ConfigDialog::display()
{
    static ConfigDialog *dialog = new ConfigDialog();
    dialog->show();
}

ConfigDialog::ConfigDialog(QWidget *parent) :
    QDialog(parent)
{
    QVBoxLayout *vbl = new QVBoxLayout();
    setLayout(vbl);
    vbl->setSizeConstraint(QLayout::SetFixedSize);

    mpContent = new QTabWidget();
    mpContent->setTabPosition(QTabWidget::West);

    mpButtonBox = new QDialogButtonBox(Qt::Horizontal);
    mpButtonBox->addButton(tr("Reset"), QDialogButtonBox::ResetRole);// (QDialogButtonBox::Reset);
    mpButtonBox->addButton(tr("Ok"), QDialogButtonBox::AcceptRole); //QDialogButtonBox::Ok
    mpButtonBox->addButton(tr("Cancel"), QDialogButtonBox::RejectRole);
    mpButtonBox->addButton(tr("Apply"), QDialogButtonBox::ApplyRole);

    connect(mpButtonBox, SIGNAL(accepted()), SLOT(accept()));
    connect(mpButtonBox, SIGNAL(rejected()), SLOT(reject()));
    connect(mpButtonBox, SIGNAL(clicked(QAbstractButton*)), SLOT(onButtonClicked(QAbstractButton*)));

    vbl->addWidget(mpContent);
    vbl->addWidget(mpButtonBox);

    mPages << new MiscPage()
           << new CaptureConfigPage()
           << new DecoderConfigPage()
           << new AVFormatConfigPage()
           << new AVFilterConfigPage()
           << new ShaderPage()
              ;

    foreach (ConfigPageBase* page, mPages) {
        page->applyToUi();
        page->applyOnUiChange(false);
        mpContent->addTab(page, page->name());
    }
}

void ConfigDialog::onButtonClicked(QAbstractButton *btn)
{
    qDebug("QDialogButtonBox clicked role=%d", mpButtonBox->buttonRole(btn));
    switch (mpButtonBox->buttonRole(btn)) {
    case QDialogButtonBox::ResetRole:
        qDebug("QDialogButtonBox ResetRole clicked");
        onReset();
        break;
    case QDialogButtonBox::AcceptRole:
    case QDialogButtonBox::ApplyRole:
        qDebug("QDialogButtonBox ApplyRole clicked");
        onApply();
        break;
    case QDialogButtonBox::DestructiveRole:
    case QDialogButtonBox::RejectRole:
        onCancel();
        break;
    default:
        break;
    }
}

void ConfigDialog::onReset()
{
    Config::instance().reset();
    // TODO: check change
    foreach (ConfigPageBase* page, mPages) {
        page->reset();
    }
}

void ConfigDialog::onApply()
{
    // TODO: check change
    foreach (ConfigPageBase* page, mPages) {
        page->apply();
    }
    Config::instance().save();
}

void ConfigDialog::onCancel()
{
    // TODO: check change
    foreach (ConfigPageBase* page, mPages) {
        page->cancel();
    }
}

