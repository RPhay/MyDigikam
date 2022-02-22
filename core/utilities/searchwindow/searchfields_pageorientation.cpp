/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-01-20
 * Description : User interface for searches
 *
 * Copyright (C) 2008-2012 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2011-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "searchfields_p.h"

namespace Digikam
{

SearchFieldPageOrientation::SearchFieldPageOrientation(QObject* const parent)
    : SearchFieldComboBox(parent)
{
}

void SearchFieldPageOrientation::setupValueWidgets(QGridLayout* layout, int row, int column)
{
    SearchFieldComboBox::setupValueWidgets(layout, row, column);
    m_comboBox->addItem(i18n("Any Orientation"));
    m_comboBox->addItem(i18n("Landscape Orientation"), 1);
    m_comboBox->addItem(i18n("Portrait orientation"), 2);

    m_comboBox->setCurrentIndex(0);
}

void SearchFieldPageOrientation::read(SearchXmlCachingReader& reader)
{
    SearchXml::Relation relation = reader.fieldRelation();

    if (relation == SearchXml::Equal)
    {
        int value = reader.valueToInt();

        if      (value == 1)
        {
            m_comboBox->setCurrentIndex(1);
        }
        else if (value == 2)
        {
            m_comboBox->setCurrentIndex(2);
        }
    }
}

} // namespace Digikam
