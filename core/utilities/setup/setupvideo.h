/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2022-05-12
 * Description : video setup page.
 *
 * Copyright (C) 2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_SETUP_VIDEO_H
#define DIGIKAM_SETUP_VIDEO_H

// Qt includes

#include <QScrollArea>

namespace Digikam
{

class SetupVideo : public QScrollArea
{
    Q_OBJECT

public:

    enum VideoTab
    {
        Decoder = 0,
        AVFormat,
        Misc
    };

public:

    explicit SetupVideo(QWidget* const parent = nullptr);
    ~SetupVideo() override;

    void applySettings();
    void cancel();

    void setActiveTab(VideoTab tab);
    VideoTab activeTab() const;

private Q_SLOTS:

    void slotReset();

private:

    void readSettings();

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_SETUP_VIDEO_H
