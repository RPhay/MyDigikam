/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-10-31
 * Description : QtAV: Multimedia framework based on Qt and FFmpeg
 *
 * SPDX-FileCopyrightText: 2012-2022 Wang Bin <wbsecg1 at gmail dot com>
 * SPDX-FileCopyrightText:      2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "MediaIO_p.h"

// Local includes

#include "QtAV_mkid.h"
#include "QtAV_factory.h"
#include "digikam_debug.h"

namespace QtAV
{

typedef QIODeviceIO MediaIOQIODevice;

static const MediaIOId MediaIOId_QIODevice = mkid::id32base36_6<'Q','I','O','D','e','v'>::value;
static const char kQIODevName[]            = "QIODevice";

FACTORY_REGISTER(MediaIO, QIODevice, kQIODevName)

class Q_DECL_HIDDEN QIODeviceIOPrivate : public MediaIOPrivate
{
public:

    QIODeviceIOPrivate()
        : MediaIOPrivate(),
          dev           (nullptr)
    {
    }

    QIODevice* dev;

private:

    Q_DISABLE_COPY(QIODeviceIOPrivate);
};

QIODeviceIO::QIODeviceIO() : MediaIO(*new QIODeviceIOPrivate()) {                                    }
QIODeviceIO::QIODeviceIO(QIODeviceIOPrivate &d) : MediaIO(d)    {                                    }
QString QIODeviceIO::name() const                               { return QLatin1String(kQIODevName); }

void QIODeviceIO::setDevice(QIODevice* dev)
{
    DPTR_D(QIODeviceIO);

    if (d.dev == dev)
        return;

    d.dev = dev;

    Q_EMIT deviceChanged();
}

QIODevice* QIODeviceIO::device() const
{
    return d_func().dev;
}

bool QIODeviceIO::isSeekable() const
{
    DPTR_D(const QIODeviceIO);

    return (d.dev && !d.dev->isSequential());
}

bool QIODeviceIO::isWritable() const
{
    DPTR_D(const QIODeviceIO);

    return (d.dev && d.dev->isWritable());
}

qint64 QIODeviceIO::read(char *data, qint64 maxSize)
{
    DPTR_D(QIODeviceIO);

    if (!d.dev)
        return 0;

    return d.dev->read(data, maxSize);
}

qint64 QIODeviceIO::write(const char *data, qint64 maxSize)
{
    DPTR_D(QIODeviceIO);

    if (!d.dev)
        return 0;

    return d.dev->write(data, maxSize);
}

bool QIODeviceIO::seek(qint64 offset, int from)
{
    DPTR_D(QIODeviceIO);

    if (!d.dev)
        return false;

    if      (from == SEEK_END)
    {
        offset = d.dev->size() - offset;
    }
    else if (from == SEEK_CUR)
    {
        offset = d.dev->pos() + offset;
    }

    return d.dev->seek(offset);
}

qint64 QIODeviceIO::position() const
{
    DPTR_D(const QIODeviceIO);

    if (!d.dev)
        return 0;

    return d.dev->pos();
}

qint64 QIODeviceIO::size() const
{
    DPTR_D(const QIODeviceIO);

    if (!d.dev)
        return 0;

    return d.dev->size(); // sequential device returns bytesAvailable()
}

// -------------------------------------------------------------------

// qrc support

static const char kQFileName[] = "QFile";

QString QFileIO::name() const
{
    return QLatin1String(kQFileName);
}

const QStringList& QFileIO::protocols() const
{
    static QStringList p = QStringList() << QLatin1String("") << QLatin1String("qrc") << QLatin1String("qfile")

#ifdef Q_OS_ANDROID

                                         << QLatin1String("assets")

#endif

#ifdef Q_OS_IOS

                                         << QLatin1String("assets-library")

#endif
                                                ;
    return p;
}

typedef QFileIO MediaIOQFile;

static const MediaIOId MediaIOId_QFile = mkid::id32base36_5<'Q','F','i','l','e'>::value;

FACTORY_REGISTER(MediaIO, QFile, kQFileName)

class Q_DECL_HIDDEN QFileIOPrivate final : public QIODeviceIOPrivate
{
public:

    QFileIOPrivate()
        : QIODeviceIOPrivate()
    {
    }

    ~QFileIOPrivate()
    {
        if (file.isOpen())
            file.close();
    }

    QFile file;
};

QFileIO::QFileIO()
    : QIODeviceIO(*new QFileIOPrivate())
{
    setDevice(&d_func().file);
}

void QFileIO::onUrlChanged()
{
    DPTR_D(QFileIO);

    if (d.file.isOpen())
        d.file.close();

    QString path(url());

    if      (path.startsWith(QLatin1String("qrc:")))
    {
        path = path.mid(3);
    }
    else if (path.startsWith(QLatin1String("qfile:")))
    {
        path = path.mid(6);

#ifdef Q_OS_WIN

        int p = path.indexOf(QLatin1Char(':'));

        if (p < 1)
        {
            qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
                << QString::asprintf("invalid path. ':' wrong position");

            return;
        }

        p       -= 1;
        QChar c  = path.at(p).toUpper();

        if ((c < QLatin1Char('A')) || (c > QLatin1Char('Z')))
        {
            qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
                << QString::asprintf("invalid path. wrong driver");

            return;
        }

        const QString path_maybe = path.mid(p);
        qCDebug(DIGIKAM_QTAV_LOG) << path_maybe;
        --p;

        while (p > 0)
        {
            c = path.at(p);

            if ((c != QLatin1Char('\\')) && (c != QLatin1Char('/')))
            {
                qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
                    << QString::asprintf("invalid path. wrong dir separator ");

                return;
            }

            --p;
        }

        path = path_maybe;

#endif

    }

    d.file.setFileName(path);

    if (path.isEmpty())
        return;

    if (!d.file.open(QIODevice::ReadOnly))
    {
        qCWarning(DIGIKAM_QTAV_LOG_WARN)
            << "Failed to open [" << d.file.fileName() << "]: " << d.file.errorString();
    }
}

} // namespace QtAV
