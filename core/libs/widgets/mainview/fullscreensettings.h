/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2013-04-29
 * Description : a full screen settings widget
 *
 * SPDX-FileCopyrightText: 2013-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_FULL_SCREEN_SETTINGS_H
#define DIGIKAM_FULL_SCREEN_SETTINGS_H

// Qt includes

#include <QGroupBox>

// Local includes

#include "digikam_export.h"

class KConfigGroup;

namespace Digikam
{

class DIGIKAM_EXPORT FullScreenSettings : public QGroupBox
{
    Q_OBJECT

public:

    explicit FullScreenSettings(int options, QWidget* const parent);
    ~FullScreenSettings() override;

    void readSettings(const KConfigGroup& group);
    void saveSettings(KConfigGroup& group);

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_FULL_SCREEN_SETTINGS_H
