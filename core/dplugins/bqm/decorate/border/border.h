/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-03-17
 * Description : batch tool to add border.
 *
 * Copyright (C) 2010-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_BQM_BORDER_H
#define DIGIKAM_BQM_BORDER_H

// Local includes

#include "batchtool.h"
#include "bordersettings.h"

using namespace Digikam;

namespace DigikamBqmBorderPlugin
{

class Border : public BatchTool
{
    Q_OBJECT

public:

    explicit Border(QObject* const parent = nullptr);
    ~Border()                                               override;

    BatchToolSettings defaultSettings()                     override;

    BatchTool* clone(QObject* const parent = nullptr) const override;

    void registerSettingsWidget()                           override;

private Q_SLOTS:

    void slotSettingsChanged()                              override;
    void slotAssignSettings2Widget()                        override;

private:

    bool toolOperations()                                   override;

private:

    BorderSettings* m_settingsView;
};

} // namespace DigikamBqmBorderPlugin

#endif // DIGIKAM_BQM_BORDER_H
