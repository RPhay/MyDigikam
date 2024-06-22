/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2013-08-19
 * Description : Image Quality configuration selector widget
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

namespace Digikam
{

class DIGIKAM_EXPORT ImageQualityConfSelector : public QWidget
{
    Q_OBJECT

public:

    enum SettingsType
    {
        GlobalSettings = 0,     ///< Global settings avaialble in setup dialog.
        CustomSettings          ///< Sttings cusomized by end-user.
    };

public:

    explicit ImageQualityConfSelector(QWidget* const parent = nullptr);
    ~ImageQualityConfSelector()                                         override;

    SettingsType          settingsSelected()                      const;
    void                  setSettingsSelected(SettingsType type);

    ImageQualityContainer customSettings()                        const;
    void setCustomSettings(const ImageQualityContainer& settings);

Q_SIGNALS:

    void signalQualitySetup();
    void signalSettingsChanged();

private Q_SLOTS:

    void slotSelectionChanged();

private:

    class Private;
    Private* const d = nullptr;
};

} // namespace Digikam
