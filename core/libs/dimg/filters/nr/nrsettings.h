/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-11-22
 * Description : noise reduction settings view.
 *
 * Copyright (C) 2009-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_NR_SETTINGS_H
#define DIGIKAM_NR_SETTINGS_H

// Local includes

#include <QWidget>

// Local includes

#include "digikam_export.h"
#include "nrfilter.h"

class KConfigGroup;

namespace Digikam
{

class DIGIKAM_EXPORT NRSettings : public QWidget
{
    Q_OBJECT

public:

    explicit NRSettings(QWidget* const parent);
    ~NRSettings() override;

    NRContainer defaultSettings()   const;
    void resetToDefault();

    NRContainer settings()          const;
    void setSettings(const NRContainer& settings);

    void setEstimateNoise(bool b);
    bool estimateNoise()            const;

    void readSettings(KConfigGroup& group);
    void writeSettings(KConfigGroup& group);

    void loadSettings();
    void saveAsSettings();

Q_SIGNALS:

    void signalSettingsChanged();
    void signalEstimateNoise();

private Q_SLOTS:

    void slotDisableParameters(bool);

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_NR_SETTINGS_H
