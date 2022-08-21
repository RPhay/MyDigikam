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

#include "captionthemeparameter.h"

// Local includes

#include "dtextedit.h"

using namespace Digikam;

namespace DigikamGenericHtmlGalleryPlugin
{

QWidget* CaptionThemeParameter::createWidget(QWidget* parent, const QString& value) const
{
    DTextEdit* const edit = new DTextEdit(parent);
    edit->setLinesVisible(4);
    edit->setText(value);

    return edit;
}

QString CaptionThemeParameter::valueFromWidget(QWidget* widget) const
{
    Q_ASSERT(widget);
    DTextEdit* const edit = static_cast<DTextEdit*>(widget);

    return edit->text();
}

} // namespace DigikamGenericHtmlGalleryPlugin
