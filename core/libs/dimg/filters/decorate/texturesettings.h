/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-03-10
 * Description : Texture settings view.
 *
 * Copyright (C) 2005-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2006-2010 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
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

#ifndef DIGIKAM_TEXTURE_SETTINGS_H
#define DIGIKAM_TEXTURE_SETTINGS_H

// Local includes

#include <QWidget>

// Local includes

#include "digikam_export.h"
#include "texturecontainer.h"

class KConfigGroup;

namespace Digikam
{

class DIGIKAM_EXPORT TextureSettings : public QWidget
{
    Q_OBJECT

public:

    explicit TextureSettings(QWidget* const parent);
    ~TextureSettings() override;

    TextureContainer defaultSettings() const;
    void resetToDefault();

    TextureContainer settings()        const;
    void setSettings(const TextureContainer& settings);

    void readSettings(KConfigGroup& group);
    void writeSettings(KConfigGroup& group);

Q_SIGNALS:

    void signalSettingsChanged();

private Q_SLOTS:

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_TEXTURE_SETTINGS_H
