/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2019-10-02
 * Description : save HEIF image options.
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

#ifndef DIGIKAM_HEIF_SETTINGS_H
#define DIGIKAM_HEIF_SETTINGS_H

// Qt includes

#include <QWidget>

// Local includes

#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_EXPORT HEIFSettings : public QWidget
{
    Q_OBJECT

public:

    explicit HEIFSettings(QWidget* const parent = nullptr);
    ~HEIFSettings() override;

    void setCompressionValue(int val);
    int  getCompressionValue()    const;

    void setLossLessCompression(bool b);
    bool getLossLessCompression() const;

Q_SIGNALS:

    void signalSettingsChanged();

private Q_SLOTS:

    void slotToggleHEIFLossLess(bool);

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_HEIF_SETTINGS_H
