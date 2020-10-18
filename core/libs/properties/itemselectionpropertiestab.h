/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2020-04-11
 * Description : A tab to display information about the current selection.
 *
 * Copyright (C) 2020 by Kartik Ramesh <kartikx2000 at gmail dot com>
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

#ifndef DIGIKAM_ITEM_SELECTION_PROPERTIES_TAB_H
#define DIGIKAM_ITEM_SELECTION_PROPERTIES_TAB_H

// Qt includes

#include <QUrl>

// Local includes

#include "dexpanderbox.h"
#include "digikam_export.h"

namespace Digikam
{

class ItemInfoList;

class DIGIKAM_EXPORT ItemSelectionPropertiesTab : public DExpanderBox
{
    Q_OBJECT

public:

    explicit ItemSelectionPropertiesTab(QWidget* const parent);
    ~ItemSelectionPropertiesTab() override;

    void setCurrentURL(const QUrl& url = QUrl());

    void setSelectionCount(const QString& str);
    void setSelectionSize(const QString& str);
    void setTotalCount(const QString& str);
    void setTotalSize(const QString& str);

private:

    class Private;
    Private *const d;
};

} // namespace Digikam

#endif // DIGIKAM_ITEM_SELECTION_PROPERTIES_TAB_H
