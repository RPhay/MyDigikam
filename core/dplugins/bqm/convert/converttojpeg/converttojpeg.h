/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-11-24
 * Description : JPEG image Converter batch tool.
 *
 * Copyright (C) 2008-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_BQM_CONVERT_TO_JPEG_H
#define DIGIKAM_BQM_CONVERT_TO_JPEG_H

// Local includes

#include "batchtool.h"

using namespace Digikam;

namespace DigikamBqmConvertToJpegPlugin
{

class ConvertToJPEG : public BatchTool
{
    Q_OBJECT

public:

    explicit ConvertToJPEG(QObject* const parent = nullptr);
    ~ConvertToJPEG() override;

    QString outputSuffix() const override;
    BatchToolSettings defaultSettings() override;

    BatchTool* clone(QObject* const parent=nullptr) const override { return new ConvertToJPEG(parent); };

    void registerSettingsWidget() override;

private Q_SLOTS:

    void slotSettingsChanged() override;
    void slotAssignSettings2Widget() override;

private:

    bool toolOperations() override;

private:

    bool m_changeSettings;
};

} // namespace DigikamBqmConvertToJpegPlugin

#endif // DIGIKAM_BQM_CONVERT_TO_JPEG_H
