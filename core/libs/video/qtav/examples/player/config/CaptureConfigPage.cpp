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

#include "CaptureConfigPage.h"

// Qt includes

#include <QLabel>
#include <QFormLayout>
#include <QImageWriter>
#include <QToolButton>
#include <QDesktopServices>
#include <QFileDialog>
#include <QUrl>

// Local includes

#include "Config.h"
#include "Slider.h"

namespace QtAVPlayer
{

CaptureConfigPage::CaptureConfigPage(QWidget *parent) :
    ConfigPageBase(parent)
{
    QFormLayout *formLayout = new QFormLayout();
    setLayout(formLayout);
    QHBoxLayout *hb = new QHBoxLayout();
    mpDir = new QLineEdit();
    hb->addWidget(mpDir);
    QToolButton *bt = new QToolButton();
    bt->setText(QString::fromLatin1("..."));
    hb->addWidget(bt);
    connect(bt, SIGNAL(clicked()), SLOT(selectSaveDir()));
    bt = new QToolButton();
    bt->setText(tr("Browse"));
    hb->addWidget(bt);
    connect(bt, SIGNAL(clicked()), SLOT(browseCaptureDir()));
    formLayout->addRow(tr("Save dir"), hb);
    mpDir->setEnabled(false);
    mpFormat = new QComboBox();
    formLayout->addRow(tr("Save format"), mpFormat);
    QList<QByteArray> formats;
    formats << "Original" << QImageWriter::supportedImageFormats();

    foreach (const QByteArray& fmt, formats)
    {
        mpFormat->addItem(QString::fromLatin1(fmt));
    }

    mpQuality = new Slider();
    formLayout->addRow(tr("Quality"), mpQuality);
    mpQuality->setRange(0, 100);
    mpQuality->setOrientation(Qt::Horizontal);
    mpQuality->setSingleStep(1);
    mpQuality->setTickInterval(10);
    mpQuality->setTickPosition(QSlider::TicksBelow);

    connect(&Config::instance(), SIGNAL(captureDirChanged(QString)),
            mpDir, SLOT(setText(QString)));

    connect(&Config::instance(), SIGNAL(captureQualityChanged(int)),
            mpQuality, SLOT(setValue(int)));
/*
    connect(mpDir, SIGNAL(textChanged(QString)),
            this, SLOT(changeDirByUi(QString)));

    connect(mpFormat, SIGNAL(currentIndexChanged(QString)),
            this, SLOT(changeFormatByUi(QString)));

    connect(mpQuality, SIGNAL(valueChanged(int)),
            this, SLOT(changeQualityByUi(int)));
*/
}

QString CaptureConfigPage::name() const
{
    return tr("Capture");
}

void CaptureConfigPage::applyFromUi()
{
    Config::instance().setCaptureDir(mpDir->text())
            .setCaptureFormat(mpFormat->currentText())
            .setCaptureQuality(mpQuality->value());
}

void CaptureConfigPage::applyToUi()
{
    mpDir->setText(Config::instance().captureDir());
    int idx = mpFormat->findText(Config::instance().captureFormat());

    if (idx >= 0)
        mpFormat->setCurrentIndex(idx);

    mpQuality->setValue(Config::instance().captureQuality());
}

void CaptureConfigPage::selectSaveDir()
{
    QString dir = QFileDialog::getExistingDirectory(0, tr("Save dir"), mpDir->text());

    if (dir.isEmpty())
        return;

    mpDir->setText(dir);
}

void CaptureConfigPage::browseCaptureDir()
{
    qDebug("browse capture dir");
    QDesktopServices::openUrl(QUrl(QString::fromLatin1("file:///") + mpDir->text()));
}

} // namespace QtAVPlayer
