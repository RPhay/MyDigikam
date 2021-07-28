/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-07-21
 * Description : MJPEG Stream Server configuration dialog to share a single list of files
 *
 * Copyright (C) 2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef DIGIKAM_MJPEG_STREAM_DLG_H
#define DIGIKAM_MJPEG_STREAM_DLG_H

// Qt includes

#include <QUrl>

// Local includes

#include "dinfointerface.h"
#include "dplugindialog.h"

using namespace Digikam;

namespace DigikamGenericMjpegStreamPlugin
{

class MjpegStreamDlg : public DPluginDialog
{
    Q_OBJECT

public:

    explicit MjpegStreamDlg(QObject* const parent,
                            DInfoInterface* const iface = nullptr);
    ~MjpegStreamDlg()               override;

private:

    void readSettings();
    void saveSettings();
    void updateServerStatus();
    bool setMjpegServerContents();
    bool startMjpegServer();

private Q_SLOTS:

    void accept()                   override;
    void slotToggleMjpegServer();
    void slotSelectionChanged();
    void slotSettingsChanged();

private:

    // Disable
    explicit MjpegStreamDlg(QWidget*) = delete;

private:

    class Private;
    Private* const d;
};

} // namespace DigikamGenericMjpegStreamPlugin

#endif // DIGIKAM_MJPEG_STREAM_DLG_H
