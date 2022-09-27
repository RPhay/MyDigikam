/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-12-28
 * Description : crop image batch tool.
 *
 * SPDX-FileCopyrightText: 2012-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_BQM_CROP_H
#define DIGIKAM_BQM_CROP_H

// Local includes

#include "batchtool.h"

using namespace Digikam;

namespace DigikamBqmCropPlugin
{

class Crop : public BatchTool
{
    Q_OBJECT

public:

    explicit Crop(QObject* const parent = nullptr);
    ~Crop()                                                 override;

    BatchToolSettings defaultSettings()                     override;

    BatchTool* clone(QObject* const parent = nullptr) const override;

    void registerSettingsWidget()                           override;

Q_SIGNALS:

    void signalAutoCrop();

private Q_SLOTS:

    void slotAssignSettings2Widget()                        override;
    void slotSettingsChanged()                              override;
    void slotDisableParameters(bool);

private:

    bool toolOperations()                                   override;

private:

    class Private;
    Private* const d;
};

} // namespace DigikamBqmCropPlugin

#endif // DIGIKAM_BQM_CROP_H
