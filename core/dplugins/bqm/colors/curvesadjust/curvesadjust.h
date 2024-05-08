/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-03-02
 * Description : Curves Adjust batch tool.
 *
 * SPDX-FileCopyrightText: 2010-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_BQM_CURVES_ADJUST_H
#define DIGIKAM_BQM_CURVES_ADJUST_H

// Local includes

#include "batchtool.h"
#include "curvessettings.h"

class QComboBox;

using namespace Digikam;

namespace DigikamBqmCurvesAdjustPlugin
{

class CurvesAdjust : public BatchTool
{
    Q_OBJECT

public:

    explicit CurvesAdjust(QObject* const parent = nullptr);
    ~CurvesAdjust()                                         override = default;

    BatchToolSettings defaultSettings()                     override;

    BatchTool* clone(QObject* const parent = nullptr) const override;

    void registerSettingsWidget()                           override;

public Q_SLOTS:

    void slotResetSettingsToDefault();

private:

    bool toolOperations()                                   override;

private Q_SLOTS:

    void slotChannelChanged();
    void slotAssignSettings2Widget()                        override;
    void slotSettingsChanged()                              override;
    void slotSettingsLoad();

private:

    DImg            m_preview;
    QComboBox*      m_channelCB     = nullptr;
    CurvesSettings* m_settingsView  = nullptr;
};

} // namespace DigikamBqmCurvesAdjustPlugin

#endif // DIGIKAM_BQM_CURVES_ADJUST_H
