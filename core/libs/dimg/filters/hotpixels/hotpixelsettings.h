/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2020-08-05)
 * Description : HotPixel settings view.
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

#ifndef DIGIKAM_HOTPIXEL_SETTINGS_H
#define DIGIKAM_HOTPIXEL_SETTINGS_H

// Local includes

#include <QWidget>
#include <QPolygon>
#include <QList>
#include <QUrl>

// Local includes

#include "digikam_export.h"
#include "hotpixelcontainer.h"
#include "hotpixelprops.h"

class KConfigGroup;

namespace Digikam
{

class DIGIKAM_EXPORT HotPixelSettings : public QWidget
{
    Q_OBJECT

public:

    explicit HotPixelSettings(QWidget* const parent);
    ~HotPixelSettings() override;

    HotPixelContainer defaultSettings()  const;
    void resetToDefault();

    HotPixelContainer settings()         const;
    void setSettings(const HotPixelContainer& settings);

    void readSettings(KConfigGroup& group);
    void writeSettings(KConfigGroup& group);

    QString configGroupName()            const;

Q_SIGNALS:

    void signalSettingsChanged();
    void signalHotPixels(const QPolygon& pointList);

private Q_SLOTS:

    void slotAddBlackFrame();
    void slotBlackFrameSelected(const QList<HotPixelProps>& hpList, const QUrl& url);
    void slotBlackFrameRemoved(const QUrl& url);
    void slotClearBlackFrameList();

private:

    void loadBlackFrame(const QUrl& url, bool selected);

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_HOTPIXEL_SETTINGS_H
