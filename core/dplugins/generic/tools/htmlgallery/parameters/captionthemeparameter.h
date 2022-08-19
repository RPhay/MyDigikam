/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2022-08-11
 * Description : a tool to generate HTML image galleries
 *
 * Copyright (C) 2012-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_CAPTION_THEME_PARAMETER_H
#define DIGIKAM_CAPTION_THEME_PARAMETER_H

// Local includes

#include "abstractthemeparameter.h"

class QWidget;
class QString;

namespace DigikamGenericHtmlGalleryPlugin
{

/**
 * A theme parameter which let the user enter a caption with spell-checking support.
 */
class CaptionThemeParameter : public AbstractThemeParameter
{
public:

    QWidget* createWidget(QWidget* parent, const QString& value) const override;
    QString  valueFromWidget(QWidget*)                           const override;
};

} // namespace DigikamGenericHtmlGalleryPlugin

#endif // DIGIKAM_CAPTION_THEME_PARAMETER_H
