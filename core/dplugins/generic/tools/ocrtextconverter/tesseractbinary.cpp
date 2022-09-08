/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2017-07-04
 * Description : Autodetect Tesseract binary program
 *
 * Copyright (C) 2017-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "tesseractbinary.h"

// KDE includes

#include <klocalizedstring.h>

namespace DigikamGenericTextConverterPlugin
{

TesseractBinary::TesseractBinary(QObject* const)
    : DBinaryIface(
                   QLatin1String("tesseract"),
                   QLatin1String("Tesseract OCR"),
                   QLatin1String("https://github.com/tesseract-ocr/tesseract#installing-tesseract"),
                   QLatin1String("TextConverter"),
                   QStringList(QLatin1String("--version")),
                   i18n("Tesseract is an optical character recognition engine for various operating systems.")
                  )
{
    setup();
}

TesseractBinary::~TesseractBinary()
{
}

} // namespace DigikamGenericTextConverterPlugin
