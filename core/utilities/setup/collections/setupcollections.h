/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-02-01
 * Description : collections setup tab
 *
 * SPDX-FileCopyrightText: 2005-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_SETUP_COLLECTIONS_H
#define DIGIKAM_SETUP_COLLECTIONS_H

// Qt includes

#include <QScrollArea>

// Local includes

#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_GUI_EXPORT SetupCollections : public QScrollArea
{
    Q_OBJECT

public:

    enum CollectionsTab
    {
        Collections = 0,
        IgnoreDirs
    };

public:

    explicit SetupCollections(QWidget* const parent = nullptr);
    ~SetupCollections() override;

    void setActiveTab(CollectionsTab tab);
    CollectionsTab activeTab() const;

    void applySettings();

private:

    void readSettings();

private Q_SLOTS:

    void slotShowCurrentIgnoredDirectoriesSettings() const;

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_SETUP_COLLECTIONS_H
