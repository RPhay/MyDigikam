/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-12-16
 * Description : Import filter combobox
 *
 * Copyright (C) 2010-2011 by Petri Damstén <petri dot damsten at iki dot fi>
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

#ifndef DIGIKAM_IMPORT_FILTER_COMBOBOX_H
#define DIGIKAM_IMPORT_FILTER_COMBOBOX_H

// Qt includes

#include <QComboBox>

// Local includes

#include "filter.h"

namespace Digikam
{

class CamItemInfo;

class ImportFilterComboBox : public QComboBox
{
    Q_OBJECT

public:

    explicit ImportFilterComboBox(QWidget* const parent);
    ~ImportFilterComboBox() override;

    Filter* currentFilter();
    void saveSettings();

    static void  defaultFilters(FilterList* const filters);
    static const QString defaultIgnoreNames;
    static const QString defaultIgnoreExtensions;

Q_SIGNALS:

    void signalFilterChanged(Filter*);

protected:

    void  fillCombo();

protected Q_SLOTS:

    void indexChanged(int index);

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_IMPORT_FILTER_COMBOBOX_H
