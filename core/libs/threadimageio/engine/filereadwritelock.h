/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-01-29
 * Description : Intra-process file i/o lock
 *
 * SPDX-FileCopyrightText: 2012 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_FILE_READ_WRITE_LOCK_H
#define DIGIKAM_FILE_READ_WRITE_LOCK_H

// Qt includes

#include <QString>
#include <QTemporaryFile>

// Local includes

#include "digikam_export.h"

namespace Digikam
{

class FileReadWriteLockPriv;

class DIGIKAM_EXPORT FileReadWriteLockKey
{
public:

    explicit FileReadWriteLockKey(const QString& filePath);
    ~FileReadWriteLockKey();

    void lockForRead();
    void lockForWrite();
    bool tryLockForRead();
    bool tryLockForRead(int timeout);
    bool tryLockForWrite();
    bool tryLockForWrite(int timeout);
    void unlock();

private:

    FileReadWriteLockPriv* d;
};

// ----------------------------------------------------------------------

class DIGIKAM_EXPORT FileReadLocker
{
public:

    explicit FileReadLocker(const QString& filePath);
    ~FileReadLocker();

private:

    FileReadWriteLockPriv* d;
};

// ----------------------------------------------------------------------

class DIGIKAM_EXPORT FileWriteLocker
{
public:

    explicit FileWriteLocker(const QString& filePath);
    ~FileWriteLocker();

private:

    FileReadWriteLockPriv* d;
};

// ----------------------------------------------------------------------

class DIGIKAM_EXPORT SafeTemporaryFile : public QTemporaryFile
{
    Q_OBJECT

public:

    explicit SafeTemporaryFile(const QString& templ);
    SafeTemporaryFile();

    bool open();

    QString safeFilePath() const;

protected:

    bool open(QIODevice::OpenMode) override;

private:

    // Disable
    SafeTemporaryFile(QObject*) = delete;

    QString m_templ;
};

} // namespace Digikam

#endif // DIGIKAM_FILE_READ_WRITE_LOCK_H
