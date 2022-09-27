/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-03-16
 * Description : Free rotation settings view.
 *
 * SPDX-FileCopyrightText: 2010-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_FREE_ROTATION_SETTINGS_H
#define DIGIKAM_FREE_ROTATION_SETTINGS_H

// Local includes

#include <QWidget>

// Local includes

#include "digikam_export.h"
#include "freerotationfilter.h"

class KConfigGroup;

namespace Digikam
{

class DIGIKAM_EXPORT FreeRotationSettings : public QWidget
{
    Q_OBJECT

public:

    explicit FreeRotationSettings(QWidget* const parent);
    ~FreeRotationSettings() override;

    FreeRotationContainer defaultSettings() const;
    void resetToDefault();

    FreeRotationContainer settings()        const;
    void setSettings(const FreeRotationContainer& settings);

    void readSettings(KConfigGroup& group);
    void writeSettings(KConfigGroup& group);

Q_SIGNALS:

    void signalSettingsChanged();

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_FREE_ROTATION_SETTINGS_H
