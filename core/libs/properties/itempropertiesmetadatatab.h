/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2004-11-17
 * Description : a tab to display item metadata information
 *
 * SPDX-FileCopyrightText: 2004-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_ITEM_PROPERTIES_METADATA_TAB_H
#define DIGIKAM_ITEM_PROPERTIES_METADATA_TAB_H

// Qt includes

#include <QWidget>
#include <QTabWidget>
#include <QUrl>

// Local includes

#include "digikam_export.h"
#include "dmetadata.h"

namespace Digikam
{

class DIGIKAM_EXPORT ItemPropertiesMetadataTab : public QTabWidget
{
    Q_OBJECT

public:

    explicit ItemPropertiesMetadataTab(QWidget* const parent);
    ~ItemPropertiesMetadataTab() override;

    void setCurrentURL(const QUrl& url = QUrl());
    void setCurrentData(DMetadata* const metadata = nullptr,
                        const QUrl& url = QUrl());

    void loadFilters();

    void readSettings(const KConfigGroup& group);
    void writeSettings(KConfigGroup& group);

Q_SIGNALS:

    void signalSetupMetadataFilters(int);
    void signalSetupExifTool();

private Q_SLOTS:

    void slotSetupMetadataFilters();

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_ITEM_PROPERTIES_METADATA_TAB_H
