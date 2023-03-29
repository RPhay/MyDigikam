/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-02-19
 * Description : Restoration batch tool.
 *
 * SPDX-FileCopyrightText: 2009-2023 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_BQM_RESTORATION_H
#define DIGIKAM_BQM_RESTORATION_H

// Local includes

#include "batchtool.h"
#include "greycstorationfilter.h"

class QComboBox;

using namespace Digikam;

namespace DigikamBqmRestorationPlugin
{

class Restoration : public BatchTool
{
    Q_OBJECT

public:

    explicit Restoration(QObject* const parent = nullptr);
    ~Restoration()                                          override;

    BatchToolSettings defaultSettings()                     override;

    BatchTool* clone(QObject* const parent = nullptr) const override;

    void registerSettingsWidget()                           override;

    void cancel()                                           override;

private:

    bool toolOperations()                                   override;

private Q_SLOTS:

    void slotAssignSettings2Widget()                        override;
    void slotSettingsChanged()                              override;

private:

    enum RestorationPreset
    {
        ReduceUniformNoise = 0,
        ReduceJPEGArtefacts,
        ReduceTexturing
    };

    QComboBox*            m_comboBox;

    GreycstorationFilter* m_cimgIface;
};

} // namespace DigikamBqmRestorationPlugin

#endif // DIGIKAM_BQM_RESTORATION_H
