/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2013-08-19
 * Description : Image Quality settings widget
 *
 * SPDX-FileCopyrightText: 2013-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2013-2014 by Gowtham Ashok <gwty93 at gmail dot com>
 * SPDX-FileCopyrightText: 2021-2022 by Phuoc Khanh Le <phuockhanhnk94 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Qt includes

#include <QWidget>

// Local includes

#include "digikam_export.h"
#include "imagequalitycontainer.h"

class KConfigGroup;

namespace Digikam
{

class DIGIKAM_EXPORT ImageQualitySettings : public QWidget
{
    Q_OBJECT

public:

    explicit ImageQualitySettings(QWidget* const parent = nullptr);
    ~ImageQualitySettings()                                 override;

    void applySettings();
    void applySettings(KConfigGroup&);

    void readSettings();
    void readSettings(const KConfigGroup&);

    ImageQualityContainer getImageQualityContainer() const;
    void setImageQualityContainer(const ImageQualityContainer& imq);

    ImageQualityContainer defaultSettings()          const;
    void resetToDefault();

Q_SIGNALS:

    void signalSettingsChanged();

private Q_SLOTS:

    void slotDisableOptionViews();

private:

    class Private;
    Private* const d = nullptr;
};

} // namespace Digikam
