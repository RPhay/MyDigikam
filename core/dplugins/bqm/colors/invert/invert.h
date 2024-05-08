/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-03-16
 * Description : Invert colors batch tool.
 *
 * SPDX-FileCopyrightText: 2010-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_BQM_INVERT_H
#define DIGIKAM_BQM_INVERT_H

// Local includes

#include "batchtool.h"

using namespace Digikam;

namespace DigikamBqmInvertPlugin
{

class Invert : public BatchTool
{
    Q_OBJECT

public:

    explicit Invert(QObject* const parent = nullptr);
    ~Invert()                                               override = default;

    BatchToolSettings defaultSettings()                     override;

    BatchTool* clone(QObject* const parent = nullptr) const override;

private:

    bool toolOperations()                                   override;

private Q_SLOTS:

    void slotAssignSettings2Widget()                        override {};
    void slotSettingsChanged()                              override {};
};

} // namespace DigikamBqmInvertPlugin

#endif // DIGIKAM_BQM_INVERT_H
