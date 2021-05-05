/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2019-10-02
 * Description : save HEIF image options.
 *
 * Copyright (C) 2020 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "heifsettings.h"

// Qt includes

#include <QString>
#include <QLabel>
#include <QCheckBox>
#include <QLayout>
#include <QGridLayout>
#include <QApplication>
#include <QStyle>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "dnuminput.h"

namespace Digikam
{

class Q_DECL_HIDDEN HEIFSettings::Private
{

public:

    explicit Private()
      : HEIFGrid            (nullptr),
        labelHEIFcompression(nullptr),
        HEIFLossLess        (nullptr),
        HEIFcompression     (nullptr)
    {
    }

    QGridLayout*  HEIFGrid;

    QLabel*       labelHEIFcompression;

    QCheckBox*    HEIFLossLess;

    DIntNumInput* HEIFcompression;
};

HEIFSettings::HEIFSettings(QWidget* const parent)
    : QWidget(parent),
      d      (new Private)
{
    setAttribute(Qt::WA_DeleteOnClose);

    const int spacing = QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing);

    d->HEIFGrid     = new QGridLayout(this);
    d->HEIFLossLess = new QCheckBox(i18n("Lossless HEIF files"), this);

    d->HEIFLossLess->setWhatsThis(i18n("<p>Toggle lossless compression for HEIF images.</p>"
                                      "<p>If this option is enabled, a lossless method will be used "
                                      "to compress HEIF pictures.</p>"));

    d->HEIFcompression = new DIntNumInput(this);
    d->HEIFcompression->setDefaultValue(75);
    d->HEIFcompression->setRange(1, 100, 1);
    d->labelHEIFcompression = new QLabel(i18n("HEIF quality:"), this);

    d->HEIFcompression->setWhatsThis(i18n("<p>The quality value for HEIF images:</p>"
                                          "<p><b>1</b>: low quality (high compression and small "
                                          "file size)<br/>"
                                          "<b>50</b>: medium quality<br/>"
                                          "<b>75</b>: good quality (default)<br/>"
                                          "<b>100</b>: high quality (no compression and "
                                          "large file size)</p>"
                                          "<p><b>Note: HEIF is not a lossless image "
                                          "compression format when you use this setting.</b></p>"));

    d->HEIFGrid->addWidget(d->HEIFLossLess,         0, 0, 1, 2);
    d->HEIFGrid->addWidget(d->labelHEIFcompression, 1, 0, 1, 2);
    d->HEIFGrid->addWidget(d->HEIFcompression,      2, 0, 1, 2);
    d->HEIFGrid->setColumnStretch(1, 10);
    d->HEIFGrid->setRowStretch(3, 10);
    d->HEIFGrid->setContentsMargins(spacing, spacing, spacing, spacing);
    d->HEIFGrid->setSpacing(spacing);

    connect(d->HEIFLossLess, SIGNAL(toggled(bool)),
            this, SLOT(slotToggleHEIFLossLess(bool)));

    connect(d->HEIFLossLess, SIGNAL(toggled(bool)),
            this, SIGNAL(signalSettingsChanged()));

    connect(d->HEIFcompression, SIGNAL(valueChanged(int)),
            this, SIGNAL(signalSettingsChanged()));
}

HEIFSettings::~HEIFSettings()
{
    delete d;
}

void HEIFSettings::setCompressionValue(int val)
{
    d->HEIFcompression->setValue(val);
}

int HEIFSettings::getCompressionValue() const
{
    return d->HEIFcompression->value();
}

void HEIFSettings::setLossLessCompression(bool b)
{
    d->HEIFLossLess->setChecked(b);
    slotToggleHEIFLossLess(d->HEIFLossLess->isChecked());
}

bool HEIFSettings::getLossLessCompression() const
{
    return d->HEIFLossLess->isChecked();
}

void HEIFSettings::slotToggleHEIFLossLess(bool b)
{
    d->HEIFcompression->setEnabled(!b);
    d->labelHEIFcompression->setEnabled(!b);
}

} // namespace Digikam
