/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2020-11-14
 * Description : Files downloader
 *
 * SPDX-FileCopyrightText: 2020-2024 by Maik Qualmann <metzpinguin at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Qt includes

#include <QDialog>
#include <QSettings>
#include <QNetworkReply>

// Local includes

#include "digikam_export.h"

namespace Digikam
{

// ----------------------------------------------------------------------------

class DIGIKAM_EXPORT DownloadInfo
{
public:

    DownloadInfo();
    DownloadInfo(const QString& _path,
                 const QString& _name,
                 const QString& _hash,
                 const qint64&  _size);
    DownloadInfo(const DownloadInfo& other);
    ~DownloadInfo();

    DownloadInfo& operator=(const DownloadInfo& other);

    /**
     * The file path on the server
     */
    QString path;

    /**
     * The file name on the server
     */
    QString name;

    /**
     * The file hash as SHA256
     */
    QString hash;

    /**
     * The file size
     */
    qint64  size = 0;
};

// ----------------------------------------------------------------------------

class DIGIKAM_EXPORT FilesDownloader : public QDialog
{
    Q_OBJECT

public:

    explicit FilesDownloader(QWidget* const parent = nullptr);
    ~FilesDownloader() override;

    bool checkDownloadFiles() const;
    void startDownload();

private:

    void download();
    void nextDownload();
    void createRequest(const QUrl& url);
    void printDownloadInfo(const QUrl& url);
    bool downloadExists(const DownloadInfo& info) const;

    QString getFacesEnginePath()                  const;
    void createDownloadInfo();

private Q_SLOTS:

    void reject() override;
    void slotDownload();
    void slotHelp();
    void slotUpdateDownloadInfo();
    void slotDownloaded(QNetworkReply* reply);
    void slotDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);

private:

    // Disable
    FilesDownloader(const FilesDownloader&)            = delete;
    FilesDownloader& operator=(const FilesDownloader&) = delete;

private:

    class Private;
    Private* const d = nullptr;
};

} // namespace Digikam
