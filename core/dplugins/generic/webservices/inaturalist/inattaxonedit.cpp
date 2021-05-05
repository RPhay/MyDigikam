/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-03-20
 * Description : a tool to export images to iNaturalist web service
 *
 * Copyright (C) 2021      by Joerg Lohse <joergmlpts at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "inattaxonedit.h"

// Qt includes

#include <QDebug>

namespace DigikamGenericINatPlugin
{

TaxonEdit::TaxonEdit(QWidget* const)
{
}

TaxonEdit::~TaxonEdit()
{
}

void TaxonEdit::focusInEvent(QFocusEvent* e)
{
    QLineEdit::focusInEvent(e);

    if ((e->reason() == Qt::MouseFocusReason) && text().isEmpty())
    {
        emit inFocus();
    }
}

} // namespace DigikamGenericINatPlugin
