/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-03-04
 * Description : a tool to adjust date time stamp of images
 *
 * SPDX-FileCopyrightText: 2009-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_BQM_TIME_ADJUST_H
#define DIGIKAM_BQM_TIME_ADJUST_H

// Local includes

#include "batchtool.h"
#include "timeadjustsettings.h"

using namespace Digikam;

namespace DigikamBqmTimeAdjustPlugin
{

class TimeAdjust : public BatchTool
{
    Q_OBJECT

public:

    explicit TimeAdjust(QObject* const parent = nullptr);
    ~TimeAdjust()                                           override = default;

    BatchToolSettings defaultSettings()                     override;

    BatchTool* clone(QObject* const parent = nullptr) const override;

    void registerSettingsWidget()                           override;

private:

    bool toolOperations()                                   override;

private Q_SLOTS:

    void slotAssignSettings2Widget()                        override;
    void slotSettingsChanged()                              override;

private:

    TimeAdjustSettings* m_taWidget       = nullptr;
    bool                m_changeSettings = true;
};

} // namespace DigikamBqmTimeAdjustPlugin

#endif // DIGIKAM_BQM_TIME_ADJUST_H
