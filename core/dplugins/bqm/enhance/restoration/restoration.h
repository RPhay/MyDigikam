/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-02-19
 * Description : Restoration batch tool.
 *
 * Copyright (C) 2009-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef DIGIKAM_BQM_RESTORATION_H
#define DIGIKAM_BQM_RESTORATION_H

// Local includes

#include "batchtool.h"
#include "greycstorationfilter.h"

class QComboBox;

using namespace Digikam;

namespace Digikam
{

class Restoration : public BatchTool
{
    Q_OBJECT

public:

    explicit Restoration(QObject* const parent = nullptr);
    ~Restoration() override;

    BatchToolSettings defaultSettings() override;

    BatchTool* clone(QObject* const parent=nullptr) const override { return new Restoration(parent); };

    void registerSettingsWidget() override;

    void cancel() override;

private:

    bool toolOperations() override;

private Q_SLOTS:

    void slotAssignSettings2Widget() override;
    void slotSettingsChanged() override;

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

} // namespace Digikam

#endif // DIGIKAM_BQM_RESTORATION_H
