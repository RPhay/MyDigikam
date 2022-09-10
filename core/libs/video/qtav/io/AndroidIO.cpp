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

// Qt includes

#include <QFile>
#include <QGuiApplication>
#include <QtAndroidExtras>

// Local includes

#include "QtAV_mkid.h"
#include "QtAV_factory.h"
#include "AndroidIO_jmi.h"
#include "digikam_debug.h"

// TODO: how to get filename and find subtitles?

// http://stackoverflow.com/questions/5657411/android-getting-a-file-uri-from-a-content-uri                                                     // krazy:exclude=insecurenet
// http://stackoverflow.com/questions/19834842/android-gallery-on-kitkat-returns-different-uri-for-intent-action-get-content/20559418#20559418  // krazy:exclude=insecurenet
// http://stackoverflow.com/questions/22029815/how-to-use-the-qt-jni-class-qandroidjniobject                                                    // krazy:exclude=insecurenet

namespace QtAV
{

static const MediaIOId MediaIOId_Android = mkid::id32base36_6<'A','D','r','o','i', 'd'>::value;
static const char kName[]                = "Android";

class AndroidIOPrivate;

class Q_DECL_HIDDEN AndroidIO : public MediaIO
{
    DPTR_DECLARE_PRIVATE(AndroidIO)

public:

    AndroidIO();

    QString name()                                      const override
    {
        return QLatin1String(kName);
    }

    const QStringList& protocols()                      const override
    {
        static QStringList p = QStringList() << QLatin1String("content")
                                             << QLatin1String("android.resource");

        // "file:" is supported too but we use QFile

        return p;
    }

    virtual bool isSeekable()                           const override;
    virtual qint64 read(char *data, qint64 maxSize)           override;
    virtual bool seek(qint64 offset, int from)                override;
    virtual qint64 position()                           const override;

    /*!
     * \brief size
     * \return <=0 if not support
     */
    virtual qint64 size()                               const override
    {
        return qt_file.size();
    }

protected:

    explicit AndroidIO(AndroidIOPrivate &d);
    void onUrlChanged()                                       override;

private:

    QFile qt_file;

    // if use Java.io.InputStream, record pos
};

typedef AndroidIO MediaIOAndroid;

FACTORY_REGISTER(MediaIO, Android, kName)

AndroidIO::AndroidIO()
    : MediaIO()
{
    jmi::javaVM(QAndroidJniEnvironment::javaVM()); // nativeResourceForIntegration("javaVM")
}

bool AndroidIO::isSeekable() const
{
    return !qt_file.isSequential();
}

qint64 AndroidIO::read(char *data, qint64 maxSize)
{
    return qt_file.read(data, maxSize);
}

bool AndroidIO::seek(qint64 offset, int from)
{
    if      (from == SEEK_END)
        offset = qt_file.size() - offset;
    else if (from == SEEK_CUR)
        offset = qt_file.pos()  + offset;

    return qt_file.seek(offset);
}

qint64 AndroidIO::position() const
{
    return qt_file.pos();
}

void AndroidIO::onUrlChanged()
{
    qt_file.close();

    if (url().isEmpty())
        return;

    struct Application final: jmi::ClassTag              { static std::string name() { return "android/app/Application";                 } };

    jmi::JObject<Application> app_ctx(jmi::android::application());

    struct ContentResolver final: jmi::ClassTag          { static std::string name() { return "android/content/ContentResolver";         } };
    struct GetContentResolver final: jmi::MethodTag      { static const char* name() { return "getContentResolver";                      } };

    jmi::JObject<ContentResolver> cr = app_ctx.call<jmi::JObject<ContentResolver>, GetContentResolver>();

    if (!cr.error().empty())
    {
        qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote() << QString::asprintf("getContentResolver error: %s", cr.error().data());

        return;
    }

    struct Uri final: jmi::ClassTag                      { static std::string name() { return "android/net/Uri";                         } };
    struct Parse final: jmi::MethodTag                   { static const char* name() { return "parse";                                   } };

    jmi::JObject<Uri> uri = jmi::JObject<Uri>::callStatic<jmi::JObject<Uri>, Parse>(url().toUtf8().constData()); // move?

    // openInputStream?

    struct ParcelFileDescriptor final: jmi::ClassTag     { static std::string name() { return "android/os/ParcelFileDescriptor";         } };

    // AssetFileDescriptor supported schemes: content, android.resource, file
    // ParcelFileDescriptor supported schemes: content, file

#if 1

    struct AssetFileDescriptor final: jmi::ClassTag      { static std::string name() { return "android/content/res/AssetFileDescriptor"; } };
    struct OpenAssetFileDescriptor final: jmi::MethodTag { static const char* name() { return "openAssetFileDescriptor";                 } };

    jmi::JObject<AssetFileDescriptor> afd = cr.call<jmi::JObject<AssetFileDescriptor>, OpenAssetFileDescriptor>(std::move(uri), "r"); // TODO: rw

    if (!afd.error().empty())
    {
        qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote() << QString::asprintf("openAssetFileDescriptor error: %s", afd.error().data());
        return;
    }

    struct GetParcelFileDescriptor final: jmi::MethodTag { static const char* name() { return "getParcelFileDescriptor";                 } };

    jmi::JObject<ParcelFileDescriptor> pfd = afd.call<jmi::JObject<ParcelFileDescriptor>, GetParcelFileDescriptor>();

#else

    struct OpenFileDescriptor final: jmi::MethodTag      { static const char* name() { return "openFileDescriptor";                      } };

    jmi::JObject<ParcelFileDescriptor> pfd = cr.call<jmi::JObject<ParcelFileDescriptor>, OpenFileDescriptor>(std::move(uri), "r");

#endif

    if (!pfd.error().empty())
    {
        qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote() << QString::asprintf("get ParcelFileDescriptor error: %s", pfd.error().data());

        return;
    }

    struct DetachFd final: jmi::MethodTag                { static const char* name() { return "detachFd";                                } };
    int fd = pfd.call<int,DetachFd>();
    qt_file.open(fd, QIODevice::ReadOnly);
}

} // namespace QtAV
