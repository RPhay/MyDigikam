/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2007-08-02
 * Description : save JPEG 2000 image options.
 *
 * Copyright (C) 2007-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_JP2K_SETTINGS_H
#define DIGIKAM_JP2K_SETTINGS_H

// Local includes

#include "dimgloadersettings.h"
#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_EXPORT JP2KSettings : public DImgLoaderSettings
{
    Q_OBJECT

public:

    explicit JP2KSettings(QWidget* const parent = nullptr);
    ~JP2KSettings() override;

    /**
     * This widget manage 2 parameters for the image encoding:
     * "quality"  as integer [1 - 100].
     * "lossless" as boolean.
     */
    void setSettings(const DImgLoaderPrms& set);
    DImgLoaderPrms settings() const;

private Q_SLOTS:

    void slotToggleJPEG2000LossLess(bool);

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_JP2K_SETTINGS_H
