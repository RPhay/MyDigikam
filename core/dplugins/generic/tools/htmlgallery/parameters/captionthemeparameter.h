/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2022-08-11
 * Description : a tool to generate HTML image galleries
 *
 * SPDX-FileCopyrightText: 2012-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
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
