/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2022-08-26
 * Description : Text converter plugin action descriptions
 *
 * SPDX-FileCopyrightText: 2008-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2022      by Quoc Hung Tran <quochungtran1999 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_TEXT_CONVERTER_ACTIONS_H
#define DIGIKAM_TEXT_CONVERTER_ACTIONS_H

// Qt includes

#include <QImage>
#include <QUrl>

// Local includes

#include "ocrtesseractengine.h"

using namespace Digikam;

namespace DigikamGenericTextConverterPlugin
{

enum TextConverterAction
{
    NONE = 0,
    PROCESS
};

class TextConverterActionData
{

public:

    TextConverterActionData()
      : starting(false),
        result  (OcrTesseractEngine::PROCESS_COMPLETE),
        action  (NONE)
    {
    }

    bool                 starting;
    int                  result;

    QString              destPath;
    QString              message;

    QString              outputText;

    QImage               image;

    QUrl                 fileUrl;

    TextConverterAction  action;
};

}  // namespace DigikamGenericTextConverterPlugin

Q_DECLARE_METATYPE(DigikamGenericTextConverterPlugin::TextConverterActionData)

#endif /// DIGIKAM_TEXT_CONVERTER_ACTIONS_H
