/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2020-12-21
 * Description : Online version dialog.
 *
 * Copyright (C) 2010-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_ONLINE_VERSION_DLG_H
#define DIGIKAM_ONLINE_VERSION_DLG_H

// Qt includes

#include <QDialog>

// Local includes

#include "digikam_export.h"
#include "digikam_version.h"

namespace Digikam
{

class DIGIKAM_EXPORT OnlineVersionDlg : public QDialog
{
    Q_OBJECT

public:

    explicit OnlineVersionDlg(QWidget* const parent = nullptr,
                              const QString& version = QLatin1String(digikam_version_short),
                              const QDateTime& buildDt = digiKamBuildDate(),
                              bool checkPreRelease = false,
                              bool updateWithDebug = false);
    ~OnlineVersionDlg() override;

Q_SIGNALS:

    void signalSetupUpdate();

private Q_SLOTS:

    void slotNewVersionAvailable(const QString& version);
    void slotNewVersionCheckError(const QString& error);

    void slotReleaseNotesData(const QString& notes);

    void slotDownloadInstaller();
    void slotDownloadError(const QString& error);
    void slotDownloadProgress(qint64, qint64);

    void slotComputeChecksum();

    void slotRunInstaller();
    void slotOpenInFileManager();

    void slotHelp();
    void slotSetupUpdate();

    void slotUpdateStats();

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_ONLINE_VERSION_DLG_H
