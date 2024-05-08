/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-02-17
 * Description : Color profile conversion tool.
 *
 * SPDX-FileCopyrightText: 2010-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_BQM_ICC_CONVERT_H
#define DIGIKAM_BQM_ICC_CONVERT_H

// Local includes

#include "batchtool.h"
#include "iccprofilesettings.h"

using namespace Digikam;

namespace DigikamBqmIccConvertPlugin
{

class IccConvert : public BatchTool
{
    Q_OBJECT

public:

    explicit IccConvert(QObject* const parent = nullptr);
    ~IccConvert()                                           override = default;

    BatchToolSettings defaultSettings()                     override;

    BatchTool* clone(QObject* const parent = nullptr) const override;

    void registerSettingsWidget()                           override;

private:

    bool toolOperations()                                   override;

private Q_SLOTS:

    void slotAssignSettings2Widget()                        override;
    void slotSettingsChanged()                              override;

private:

    IccProfilesSettings* m_settingsView = nullptr;
};

} // namespace DigikamBqmIccConvertPlugin

#endif // DIGIKAM_BQM_ICC_CONVERT_H
