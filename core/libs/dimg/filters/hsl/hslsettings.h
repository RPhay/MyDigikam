/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-02-11
 * Description : HSL settings view.
 *
 * SPDX-FileCopyrightText: 2010-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_HSL_SETTINGS_H
#define DIGIKAM_HSL_SETTINGS_H

// Local includes

#include <QWidget>

// Local includes

#include "digikam_export.h"
#include "hslfilter.h"

class KConfigGroup;

namespace Digikam
{

class DIGIKAM_EXPORT HSLSettings : public QWidget
{
    Q_OBJECT

public:

    explicit HSLSettings(QWidget* const parent);
    ~HSLSettings() override;

    HSLContainer defaultSettings()  const;
    void resetToDefault();

    HSLContainer settings()         const;
    void setSettings(const HSLContainer& settings);

    void readSettings(KConfigGroup& group);
    void writeSettings(KConfigGroup& group);

Q_SIGNALS:

    void signalSettingsChanged();

private Q_SLOTS:

    void slotHSChanged(int h, int s);
    void slotHChanged(double h);
    void slotSChanged(double s);

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_HSL_SETTINGS_H
