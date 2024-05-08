/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-05-03
 * Description : blur image batch tool.
 *
 * SPDX-FileCopyrightText: 2010-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_BQM_BLUR_H
#define DIGIKAM_BQM_BLUR_H

#include "batchtool.h"
#include "dnuminput.h"

using namespace Digikam;

namespace DigikamBqmBlurPlugin
{

class Blur : public BatchTool
{
    Q_OBJECT

public:

    explicit Blur(QObject* const parent = nullptr);
    ~Blur()                                                 override = default;

    BatchToolSettings defaultSettings()                     override;

    BatchTool* clone(QObject* const parent = nullptr) const override;

    void registerSettingsWidget()                           override;

private:

    bool toolOperations()                                   override;

private Q_SLOTS:

    void slotAssignSettings2Widget()                        override;
    void slotSettingsChanged()                              override;

private:

    DIntNumInput* m_radiusInput     = nullptr;
    bool          m_changeSettings  = true;
};

} // namespace DigikamBqmBlurPlugin

#endif // DIGIKAM_BQM_BLUR_H
