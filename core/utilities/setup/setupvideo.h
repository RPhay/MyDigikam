/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2022-05-12
 * Description : video setup page.
 *
 * SPDX-FileCopyrightText: 2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
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
