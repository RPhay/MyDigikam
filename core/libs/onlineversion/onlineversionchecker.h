/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2020-12-31
 * Description : Online version checker
 *
 * Copyright (C) 2020-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_ONLINE_VERSION_CHECKER_H
#define DIGIKAM_ONLINE_VERSION_CHECKER_H

// Qt includes

#include <QObject>
#include <QUrl>
#include <QNetworkReply>
#include <QDateTime>

// Local includes

#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_EXPORT OnlineVersionChecker : public QObject
{
    Q_OBJECT

public:

    explicit OnlineVersionChecker(QObject* const parent, bool checkPreRelease = false);
    ~OnlineVersionChecker();

    void setCurrentVersion(const QString& version);
    void setCurrentBuildDate(const QDateTime& dt);

    void checkForNewVersion();
    void cancelCheck();

    void downloadReleaseNotes(const QString& version);

    QString preReleaseFileName() const;

    /**
     * Return true if the system and architecture are supported by the bundle workflow.
     * 'arch' is the relevant prefix for the bundle architecture.
     * 'ext'  is the relevant bundle file extension.
     */
    static bool bundleProperties(QString& arch, QString& ext);

    /**
     * Return the last date as string when have been performed a check for new version.
     */
    static QString lastCheckDate();

Q_SIGNALS:

    void signalNewVersionAvailable(const QString& version);
    void signalNewVersionCheckError(const QString& error);
    void signalReleaseNotesData(const QString& notes);

private Q_SLOTS:

    void slotDownloadFinished(QNetworkReply* reply);

private:

    void download(const QUrl& url);

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_ONLINE_VERSION_CHECKER_H
