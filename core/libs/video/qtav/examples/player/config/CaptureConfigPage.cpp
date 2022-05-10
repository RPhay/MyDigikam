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

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "ConfigManager.h"
#include "Slider.h"
#include "digikam_debug.h"

namespace QtAVPlayer
{

CaptureConfigPage::CaptureConfigPage(QWidget* const parent)
    : ConfigPageBase(parent)
{
    QFormLayout* const formLayout = new QFormLayout();
    setLayout(formLayout);

    QHBoxLayout* const hb         = new QHBoxLayout();
    mpDir                         = new QLineEdit();
    hb->addWidget(mpDir);
    QToolButton* bt               = new QToolButton();
    bt->setText(QString::fromLatin1("..."));
    hb->addWidget(bt);

    connect(bt, SIGNAL(clicked()),
            this,  SLOT(selectSaveDir()));

    bt              = new QToolButton();
    bt->setText(i18n("Browse"));
    hb->addWidget(bt);

    connect(bt, SIGNAL(clicked()),
            this, SLOT(browseCaptureDir()));

    formLayout->addRow(i18n("Save dir"), hb);
    mpDir->setEnabled(false);
    mpFormat        = new QComboBox();
    formLayout->addRow(i18n("Save format"), mpFormat);
    QList<QByteArray> formats;
    formats << "Original" << QImageWriter::supportedImageFormats();

    foreach (const QByteArray& fmt, formats)
    {
        mpFormat->addItem(QString::fromLatin1(fmt));
    }

    mpQuality       = new Slider();
    formLayout->addRow(i18n("Quality"), mpQuality);
    mpQuality->setRange(0, 100);
    mpQuality->setOrientation(Qt::Horizontal);
    mpQuality->setSingleStep(1);
    mpQuality->setTickInterval(10);
    mpQuality->setTickPosition(QSlider::TicksBelow);

    connect(&ConfigManager::instance(), SIGNAL(captureDirChanged(QString)),
            mpDir, SLOT(setText(QString)));

    connect(&ConfigManager::instance(), SIGNAL(captureQualityChanged(int)),
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
    return i18n("Capture");
}

void CaptureConfigPage::applyFromUi()
{
    ConfigManager::instance().setCaptureDir(mpDir->text())
            .setCaptureFormat(mpFormat->currentText())
            .setCaptureQuality(mpQuality->value());
}

void CaptureConfigPage::applyToUi()
{
    mpDir->setText(ConfigManager::instance().captureDir());
    int idx = mpFormat->findText(ConfigManager::instance().captureFormat());

    if (idx >= 0)
        mpFormat->setCurrentIndex(idx);

    mpQuality->setValue(ConfigManager::instance().captureQuality());
}

void CaptureConfigPage::selectSaveDir()
{
    QString dir = QFileDialog::getExistingDirectory(0, i18n("Save dir"), mpDir->text());

    if (dir.isEmpty())
        return;

    mpDir->setText(dir);
}

void CaptureConfigPage::browseCaptureDir()
{
    qCDebug(DIGIKAM_QTAVPLAYER_LOG).noquote() << QString::asprintf("browse capture dir");
    QDesktopServices::openUrl(QUrl(QString::fromLatin1("file:///") + mpDir->text()));
}

} // namespace QtAVPlayer
