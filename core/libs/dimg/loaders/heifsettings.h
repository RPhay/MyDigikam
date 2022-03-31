/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2019-10-02
 * Description : save HEIF image options.
 *
 * Copyright (C) 2020-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_HEIF_SETTINGS_H
#define DIGIKAM_HEIF_SETTINGS_H

// Local includes

#include "dimgloadersettings.h"
#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_EXPORT HEIFSettings : public DImgLoaderSettings
{
    Q_OBJECT

public:

    explicit HEIFSettings(QWidget* const parent = nullptr);
    ~HEIFSettings() override;

    /**
     * This widget manage 2 parameters for the image encoding:
     * "quality"  as integer [1 - 100].
     * "lossless" as boolean.
     */
    void setSettings(const DImgLoaderPrms& set);
    DImgLoaderPrms settings() const;
    QStringList parameters()  const;

private Q_SLOTS:

    void slotToggleHEIFLossLess(bool);

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_HEIF_SETTINGS_H
