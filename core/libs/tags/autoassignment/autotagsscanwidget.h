/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2025-01-06
 * Description : Widget to choose options for autotags scanning
 *
 * SPDX-FileCopyrightText: 2024-2025 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2024-2025 by Michael Miller <michael underscore miller at msn dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Qt includes

#include <QTabWidget>

// Local includes

#include "digikam_export.h"
#include "statesavingobject.h"
#include "autotagsscansettings.h"

namespace Digikam
{

class DIGIKAM_GUI_EXPORT AutoTagsScanWidget : public QTabWidget,
                                              public StateSavingObject
{
    Q_OBJECT

public:

    explicit AutoTagsScanWidget(QWidget* const parent = nullptr);
    ~AutoTagsScanWidget()                 override;

    AutoTagsScanSettings settings() const;

protected:

    void doLoadState()                    override;
    void doSaveState()                    override;

private:

    void setupUi();

private:

    class Private;
    Private* const d = nullptr;
};

} // namespace Digikam
