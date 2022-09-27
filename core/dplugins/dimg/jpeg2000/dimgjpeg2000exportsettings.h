/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2007-08-02
 * Description : JPEG-2000 image export settings widget.
 *
 * SPDX-FileCopyrightText: 2007-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_DIMG_JPEG2000_EXPORT_SETTINGS_H
#define DIGIKAM_DIMG_JPEG2000_EXPORT_SETTINGS_H

// Local includes

#include "dimgloadersettings.h"

namespace Digikam
{

class DImgJPEG2000ExportSettings : public DImgLoaderSettings
{
    Q_OBJECT

public:

    explicit DImgJPEG2000ExportSettings(QWidget* const parent = nullptr);
    ~DImgJPEG2000ExportSettings()               override;

    /**
     * This widget manage 2 parameters for the image encoding:
     * "quality"  as integer [1 - 100].
     * "lossless" as boolean.
     */
    void setSettings(const DImgLoaderPrms& set) override;
    DImgLoaderPrms settings() const             override;

private Q_SLOTS:

    void slotToggleJPEG2000LossLess(bool);

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_DIMG_JPEG2000_EXPORT_SETTINGS_H
