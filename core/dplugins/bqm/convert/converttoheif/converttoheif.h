/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2019-09-02
 * Description : HEIF image Converter batch tool.
 *
 * Copyright (C) 2020 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_BQM_CONVERT_TO_HEIF_H
#define DIGIKAM_BQM_CONVERT_TO_HEIF_H

// Local includes

#include "batchtool.h"

using namespace Digikam;

namespace DigikamBqmConvertToHeifPlugin
{

class ConvertToHEIF : public BatchTool
{
    Q_OBJECT

public:

    explicit ConvertToHEIF(QObject* const parent = nullptr);
    ~ConvertToHEIF();

    QString outputSuffix() const;
    BatchToolSettings defaultSettings();

    BatchTool* clone(QObject* const parent=nullptr) const { return new ConvertToHEIF(parent); };

    void registerSettingsWidget();

private Q_SLOTS:

    void slotSettingsChanged();
    void slotAssignSettings2Widget();

private:

    bool toolOperations();

private:

    bool m_changeSettings;
};

} // namespace DigikamBqmConvertToHeifPlugin

#endif // DIGIKAM_BQM_CONVERT_TO_HEIF_H
