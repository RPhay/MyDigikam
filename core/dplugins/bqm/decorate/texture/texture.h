/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2020-08-13
 * Description : batch tool to add texture.
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

#ifndef DIGIKAM_BQM_TEXTURE_H
#define DIGIKAM_BQM_TEXTURE_H

// Local includes

#include "batchtool.h"
#include "texturesettings.h"

using namespace Digikam;

namespace DigikamBqmTexturePlugin
{

class Texture : public BatchTool
{
    Q_OBJECT

public:

    explicit Texture(QObject* const parent = nullptr);
    ~Texture()                                              override;

    BatchToolSettings defaultSettings()                     override;

    BatchTool* clone(QObject* const parent = nullptr) const override;

    void registerSettingsWidget()                           override;

private Q_SLOTS:

    void slotSettingsChanged()                              override;
    void slotAssignSettings2Widget()                        override;

private:

    bool toolOperations()                                   override;

private:

    TextureSettings* m_settingsView;
};

} // namespace DigikamBqmTexturePlugin

#endif // DIGIKAM_BQM_TEXTURE_H
