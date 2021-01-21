/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2020-11-14
 * Description : Files downloader
 *
 * Copyright (C) 2020 by Maik Qualmann <metzpinguin at gmail dot com>
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
    bool exists(int index) const;
    void createRequest(const QUrl& url);

private Q_SLOTS:

    void reject() override;
    void slotDownload();
    void slotDownloaded(QNetworkReply* reply);
    void slotDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);

private:

    // Hidden copy constructor and assignment operator.
    FilesDownloader(const FilesDownloader&);
    FilesDownloader& operator=(const FilesDownloader&);

private:

    class Private;
    Private* const d;
};

// ----------------------------------------------------------------------------

class DIGIKAM_EXPORT DownloadInfo
{
public:

    explicit DownloadInfo()
      : size(0)
    {
    };

    ~DownloadInfo()
    {
    };

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

} // namespace Digikam

#endif // DIGIKAM_FILES_DOWNLOADER_H
