/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2020-11-14
 * Description : Files downloader
 *
 * Copyright (C) 2020-2021 by Maik Qualmann <metzpinguin at gmail dot com>
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

#ifndef DIGIKAM_FILES_DOWNLOADER_H
#define DIGIKAM_FILES_DOWNLOADER_H

// Qt includes

#include <QDialog>
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
    qint64  size;
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


private Q_SLOTS:

    void reject() override;
    void slotDownload();
    void slotDownloaded(QNetworkReply* reply);
    void slotDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);

private:

    // Disable
    FilesDownloader(const FilesDownloader&)            = delete;
    FilesDownloader& operator=(const FilesDownloader&) = delete;

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_FILES_DOWNLOADER_H
