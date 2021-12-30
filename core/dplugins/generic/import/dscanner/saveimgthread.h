/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-10-11
 * Description : save image thread for scanned data
 *
 * Copyright (C) 2009-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef DIGIKAM_SAVE_IMG_THREAD_H
#define DIGIKAM_SAVE_IMG_THREAD_H

#include <ksane_version.h>

// Qt includes

#include <QObject>
#include <QThread>
#include <QString>
#include <QUrl>
#if KSANE_VERSION < QT_VERSION_CHECK(21,8,0)
#include <QByteArray>
#else
#include <QImage>
#endif

namespace DigikamGenericDScannerPlugin
{

class SaveImgThread : public QThread
{
    Q_OBJECT

public:

    explicit SaveImgThread(QObject* const parent);
    ~SaveImgThread()    override;

    void setTargetFile(const QUrl& url, const QString& format);
    void setScannerModel(const QString& make, const QString& model);
#if KSANE_VERSION < QT_VERSION_CHECK(21,8,0)
    void setImageData(const QByteArray& ksaneData, int width, int height,
                      int bytesPerLine, int ksaneFormat);
#else
    void setImageData(const QImage& imageData);
#endif

Q_SIGNALS:

    void signalProgress(const QUrl&, int);
    void signalComplete(const QUrl&, bool);

private:

    void run()          override;

private:

    class Private;
    Private* const d;
};

} // namespace DigikamGenericDScannerPlugin

#endif // DIGIKAM_SAVE_IMG_THREAD_H
