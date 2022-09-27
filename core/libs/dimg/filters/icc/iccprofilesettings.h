/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-02-17
 * Description : Icc profile settings view.
 *
 * SPDX-FileCopyrightText: 2010-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_ICC_PROFILE_SETTINGS_H
#define DIGIKAM_ICC_PROFILE_SETTINGS_H

// Local includes

#include <QWidget>

// Local includes

#include "dlayoutbox.h"
#include "digikam_export.h"
#include "iccprofile.h"

class KConfigGroup;

namespace Digikam
{

class DIGIKAM_EXPORT IccProfilesSettings : public DVBox
{
    Q_OBJECT

public:

    explicit IccProfilesSettings(QWidget* const parent = nullptr);
    ~IccProfilesSettings() override;

    IccProfile defaultProfile() const;
    void resetToDefault();

    IccProfile currentProfile() const;
    void setCurrentProfile(const IccProfile& prof);

    void readSettings(KConfigGroup& group);
    void writeSettings(KConfigGroup& group);

    static QStringList favoriteProfiles(KConfigGroup& group);

Q_SIGNALS:

    void signalSettingsChanged();

private Q_SLOTS:

    void slotNewProfInfo();
    void slotProfileChanged();

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_ICC_PROFILE_SETTINGS_H
