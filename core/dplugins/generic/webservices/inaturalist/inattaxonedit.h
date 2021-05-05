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

#ifndef DIGIKAM_INAT_TAXON_EDIT_H
#define DIGIKAM_INAT_TAXON_EDIT_H

// Local includes

#include <QWidget>
#include <QLineEdit>
#include <QFocusEvent>

namespace DigikamGenericINatPlugin
{

class TaxonEdit : public QLineEdit
{
    Q_OBJECT

public:

    explicit TaxonEdit(QWidget* const parent = nullptr);
    ~TaxonEdit();

Q_SIGNALS:

    void inFocus();

protected:

    virtual void focusInEvent(QFocusEvent* e);
};

} // namespace DigikamGenericINatPlugin

#endif // DIGIKAM_INAT_TAXON_EDIT_H
