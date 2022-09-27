/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2020-27-07
 * Description : system settings widget
 *
 * SPDX-FileCopyrightText: 2020 by Maik Qualmann <metzpinguin at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_SYSTEM_SETTINGS_WIDGET_H
#define DIGIKAM_SYSTEM_SETTINGS_WIDGET_H

// Qt includes

#include <QWidget>

// Local includes

#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_EXPORT SystemSettingsWidget : public QWidget
{
    Q_OBJECT

public:

    explicit SystemSettingsWidget(QWidget* const parent);
    ~SystemSettingsWidget() override;

public:

    void readSettings();
    void saveSettings();

private Q_SLOTS:

    void slotFaceDownload();

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_SYSTEM_SETTINGS_WIDGET_H
