/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2013-08-19
 * Description : Image Quality configuration selector widget
 *
 * SPDX-FileCopyrightText: 2013-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2013-2014 by Gowtham Ashok <gwty93 at gmail dot com>
 * SPDX-FileCopyrightText: 2021-2022 by Phuoc Khanh Le <phuockhanhnk94 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_IMAGE_QUALITY_CONF_SELECTOR_H
#define DIGIKAM_IMAGE_QUALITY_CONF_SELECTOR_H

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
        DefaultSettings = 0,
        CustomSettings
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

private Q_SLOTS:

    void slotSelectionChanged();

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_IMAGE_QUALITY_CONF_SELECTOR_H
