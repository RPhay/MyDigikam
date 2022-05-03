/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-10-31
 * Description : QtAV: Multimedia framework based on Qt and FFmpeg
 *
 * Copyright (C) 2012-2022 Wang Bin <wbsecg1 at gmail dot com>
 * Copyright (C)      2022 by Gilles Caulier, <caulier dot gilles at gmail dot com>
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

#ifndef QTAV_MEDIAIO_P_H
#define QTAV_MEDIAIO_P_H

#include "MediaIO.h"

// Qt includes

#include <QString>
#include <QFile>

// Local includes

#include "QtAV_Global.h"
#include "AVCompat.h"

namespace QtAV
{

class MediaIO;

class QTAV_PRIVATE_EXPORT MediaIOPrivate : public DPtrPrivate<MediaIO>
{
public:

    MediaIOPrivate()
        : ctx(nullptr)
        , buffer_size(-1)
        , mode(MediaIO::Read)
    {
    }

    AVIOContext*        ctx;
    int                 buffer_size;
    MediaIO::AccessMode mode;
    QString             url;
};

// -------------------------------------------------------------------

class QIODeviceIOPrivate;

class QTAV_PRIVATE_EXPORT QIODeviceIO : public MediaIO
{
    Q_OBJECT
    Q_PROPERTY(QIODevice* device READ device WRITE setDevice NOTIFY deviceChanged)
    DPTR_DECLARE_PRIVATE(QIODeviceIO)

public:

    QIODeviceIO();
    virtual QString name()                                  const override;

    // MUST open/close outside

    void setDevice(QIODevice *dev); // set private in QFileIO etc
    QIODevice* device()                                     const;

    virtual bool isSeekable()                               const override;
    virtual bool isWritable()                               const override;
    virtual qint64 read(char *data, qint64 maxSize)               override;
    virtual qint64 write(const char *data, qint64 maxSize)        override;
    virtual bool seek(qint64 offset, int from)                    override;
    virtual qint64 position()                               const override;

    /*!
     * \brief size
     * \return <=0 if not support
     */
    virtual qint64 size()                                   const override;

Q_SIGNALS:

    void deviceChanged();

protected:

    explicit QIODeviceIO(QIODeviceIOPrivate &d);
};

// -------------------------------------------------------------------

class QFileIOPrivate;

class QTAV_PRIVATE_EXPORT QFileIO final : public QIODeviceIO
{
    Q_OBJECT
    DPTR_DECLARE_PRIVATE(QFileIO)

public:

    QFileIO();

    QString name()                  const override;

    const QStringList& protocols()  const override;

protected:

    void onUrlChanged()                   override;

private:

    using QIODeviceIO::setDevice;
};

} // namespace QtAV

#endif // QTAV_MEDIAIO_P_H
