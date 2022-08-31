/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * https://www.digikam.org
 *
 * Date        : 2022-08-26
 * Description : Text converter plugin action descriptions
 *
 * Copyright (C) 2008-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2022      by Quoc Hung Tran <quochungtran1999 at gmail dot com>
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

#ifndef DIGIKAM_TEXT_CONVERTER_ACTIONS_H
#define DIGIKAM_TEXT_CONVERTER_ACTIONS_H

// Qt includes

#include <QImage>

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
        result  (OcrTesseracrEngine::PROCESS_COMPLETE),
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
