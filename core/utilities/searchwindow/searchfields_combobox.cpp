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

SearchFieldComboBox::SearchFieldComboBox(QObject* const parent)
    : SearchField(parent),
      m_comboBox (nullptr)
{
}

void SearchFieldComboBox::setupValueWidgets(QGridLayout* layout, int row, int column)
{
    m_comboBox = new QComboBox;
    m_comboBox->setEditable(false);
    layout->addWidget(m_comboBox, row, column, 1, 3);

    connect(m_comboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(indexChanged(int)));
}

void SearchFieldComboBox::write(SearchXmlWriter& writer)
{
    int index = m_comboBox->currentIndex();

    if (index != -1)
    {
        QVariant bits = m_comboBox->itemData(index);

        if (!bits.isNull())
        {
            writer.writeField(m_name, SearchXml::Equal);
            writer.writeValue(bits.toInt());
            writer.finishField();
        }
    }
}

void SearchFieldComboBox::setValueWidgetsVisible(bool visible)
{
    m_comboBox->setVisible(visible);
}

void SearchFieldComboBox::reset()
{
    m_comboBox->setCurrentIndex(0);
}

QList<QRect> SearchFieldComboBox::valueWidgetRects() const
{
    QList<QRect> rects;
    rects << m_comboBox->geometry();

    return rects;
}

void SearchFieldComboBox::indexChanged(int index)
{
    setValidValueState(index != 0);
}

} // namespace Digikam
