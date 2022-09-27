/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2022-09-22
 * Description : assign pick label metadata by image quality batch tool.
 *
 * SPDX-FileCopyrightText: 2021-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_BQM_QUALITY_SORT_H
#define DIGIKAM_BQM_QUALITY_SORT_H

// Local includes

#include "batchtool.h"

using namespace Digikam;

namespace DigikamBqmQualitySortPlugin
{

class QualitySort : public BatchTool
{
    Q_OBJECT

public:

    explicit QualitySort(QObject* const parent = nullptr);
    ~QualitySort()                                          override;

    BatchToolSettings defaultSettings()                     override;

    BatchTool* clone(QObject* const parent = nullptr) const override;

    void registerSettingsWidget()                           override;

private Q_SLOTS:

    void slotAssignSettings2Widget()                        override;
    void slotSettingsChanged()                              override;
    void slotQualitySetup();

private:

    bool toolOperations()                                   override;
    void cancel()                                           override;

private:

    class Private;
    Private* const d;
};

} // namespace DigikamBqmQualitySortPlugin

#endif // DIGIKAM_BQM_QUALITY_SORT_H
