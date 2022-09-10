/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2020-04-11
 * Description : A tab to display information about the current selection.
 *
 * SPDX-FileCopyrightText: 2020 by Kartik Ramesh <kartikx2000 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
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
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_ITEM_SELECTION_PROPERTIES_TAB_H
