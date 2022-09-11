/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2006-09-24
 * Description : DNG converter plugin action descriptions
 *
 * SPDX-FileCopyrightText: 2008-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_DNG_CONVERTER_ACTIONS_H
#define DIGIKAM_DNG_CONVERTER_ACTIONS_H

// Qt includes

#include <QString>
#include <QImage>
#include <QMetaType>
#include <QUrl>

// Local includes

#include "dngwriter.h"

using namespace Digikam;

namespace DigikamGenericDNGConverterPlugin
{

enum DNGConverterAction
{
    NONE = 0,
    IDENTIFY,
    PROCESS
};

class DNGConverterActionData
{

public:

    DNGConverterActionData()
      : starting(false),
        result  (DNGWriter::PROCESS_COMPLETE),
        action  (NONE)
    {
    }

    bool                starting;
    int                 result;

    QString             destPath;
    QString             message;

    QImage              image;

    QUrl                fileUrl;

    DNGConverterAction  action;
};

}  // namespace DigikamGenericDNGConverterPlugin

Q_DECLARE_METATYPE(DigikamGenericDNGConverterPlugin::DNGConverterActionData)

#endif /// DIGIKAM_DNG_CONVERTER_ACTIONS_H
