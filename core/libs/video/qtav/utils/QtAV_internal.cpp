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

#include "QtAV_internal.h"

// Qt includes

#include <QStandardPaths>

#ifdef Q_OS_MAC
#   include <CoreFoundation/CoreFoundation.h>
#endif

// Local include

#include "AVCompat.h"
#include "digikam_debug.h"

namespace QtAV
{

static const char kFileScheme[] = "file:";

#define CHAR_COUNT(s) (sizeof(s) - 1) // tail '\0'

#ifdef Q_OS_MAC

QString absolutePathFromOSX(const QString& s)
{
    QString result;

#   if !(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ < 1060)

    CFStringRef cfStr = CFStringCreateWithCString(kCFAllocatorDefault, s.toUtf8().constData(), kCFStringEncodingUTF8);

    if (cfStr)
    {
        CFURLRef cfUrl = CFURLCreateWithString(kCFAllocatorDefault, cfStr, nullptr);

        if (cfUrl)
        {
            CFErrorRef error  = 0;
            CFURLRef cfUrlAbs = CFURLCreateFilePathURL(kCFAllocatorDefault, cfUrl, &error);

            if (cfUrlAbs)
            {
                CFStringRef cfStrAbsUrl = CFURLGetString(cfUrlAbs);
                result                  = QString().fromCFString(cfStrAbsUrl);
                CFRelease(cfUrlAbs);
            }

            CFRelease(cfUrl);
        }

        CFRelease(cfStr);
    }

#   else

    Q_UNUSED(s);

#   endif

    return result;
}

#endif // Q_OS_MAC

/*!
 * \brief getLocalPath
 * get path that works for both ffmpeg and QFile
 * Windows: ffmpeg does not supports file:///C:/xx.mov, only supports file:C:/xx.mov or C:/xx.mov
 * QFile: does not support file: scheme
 * fullPath can be file:///path from QUrl. QUrl.toLocalFile will remove file://
 */
QString getLocalPath(const QString& fullPath)
{

#ifdef Q_OS_MAC

    if (fullPath.startsWith(QLatin1String("file:///.file/id=")) || fullPath.startsWith(QLatin1String("/.file/id=")))
        return absolutePathFromOSX(fullPath);

#endif

    int pos = fullPath.indexOf(QLatin1String(kFileScheme));

    if (pos >= 0)
    {
        pos           += CHAR_COUNT(kFileScheme);
        bool has_slash = false;

        while (fullPath.at(pos) == QLatin1Char('/'))
        {
            has_slash = true;
            ++pos;
        }

        // win: ffmpeg does not supports file:///C:/xx.mov, only supports file:C:/xx.mov or C:/xx.mov

        // TODO: add mingw support?

#ifndef Q_OS_WIN // for QUrl

        if (has_slash)
            --pos;

#else

    Q_UNUSED(has_slash);

#endif

    }

    // always remove "file:" even thought it works for ffmpeg.but fileName() may be used for QFile which does not file:

    if (pos > 0)
        return fullPath.mid(pos);

    return fullPath;
}

#undef CHAR_COUNT

namespace Internal
{

namespace Path
{

QString toLocal(const QString& fullPath)
{
    return getLocalPath(fullPath);
}

QString appFontsDir()
{

#if 0 // qt may return an read only path, for example MacOS /System/Library/Fonts

    const QString dir(QStandardPaths::writableLocation(QStandardPaths::FontsLocation));

    if (!dir.isEmpty())
        return dir;

#endif

    return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + QLatin1String("/fonts");
}

QString fontsDir()
{
    return QStandardPaths::standardLocations(QStandardPaths::FontsLocation).first();
}

} // namespace Path

QString options2StringHelper(void* obj, const char* unit)
{
    qCDebug(DIGIKAM_QTAV_LOG).noquote()
        << QString::asprintf("obj: %p", obj);

    QString s;
    const AVOption* opt = nullptr;

    while ((opt = av_opt_next(obj, opt)))
    {
        if (opt->type == AV_OPT_TYPE_CONST)
        {
            if (!unit)
                continue;

            if (!qstrcmp(unit, opt->unit))
                s.append(QString::fromUtf8(" %1 = %2").arg(QLatin1String(opt->name)).arg(opt->default_val.i64));

            continue;
        }
        else
        {
            if (unit)
                continue;
        }

        s.append(QString::fromUtf8("\n%1: ").arg(QLatin1String(opt->name)));

        switch (opt->type)
        {
            case AV_OPT_TYPE_FLAGS:
            case AV_OPT_TYPE_INT:
            case AV_OPT_TYPE_INT64:
            {
                s.append(QString::fromUtf8("(%1)").arg(opt->default_val.i64));

                break;
            }

            case AV_OPT_TYPE_DOUBLE:
            case AV_OPT_TYPE_FLOAT:
            {
                s.append(QString::fromUtf8("(%1)").arg(opt->default_val.dbl, 0, 'f'));

                break;
            }

            case AV_OPT_TYPE_STRING:
            {
                if (opt->default_val.str)
                    s.append(QString::fromUtf8("(%1)").arg(QString::fromUtf8(opt->default_val.str)));

                break;
            }

            case AV_OPT_TYPE_RATIONAL:
            {
                s.append(QString::fromUtf8("(%1 / %2)").arg(opt->default_val.q.num).arg(opt->default_val.q.den));

                break;
            }

            default:
            {
                break;
            }
        }

        if (opt->help)
            s.append(QLatin1String(" ")).append(QString::fromUtf8(opt->help));

        if (opt->unit && (opt->type != AV_OPT_TYPE_CONST))
            s.append(QLatin1String("\n ")).append(options2StringHelper(obj, opt->unit));
    }

    return s;
}

QString optionsToString(void* obj)
{
    return options2StringHelper(obj, nullptr);
}

void setOptionsToFFmpegObj(const QVariant& opt, void* obj)
{
    if (!opt.isValid())
        return;

    AVClass* const c = (obj ? *(AVClass**)obj : nullptr);

    if (c)
    {
        qCDebug(DIGIKAM_QTAV_LOG)
            << QString::fromUtf8("%1.%2 options:")
                .arg(QLatin1String(c->class_name))
                .arg(QLatin1String(c->item_name(obj)));
    }
    else
    {
        qCDebug(DIGIKAM_QTAV_LOG) << "options:";
    }

    if (opt.type() == QVariant::Map)
    {
        QVariantMap options(opt.toMap());

        if (options.isEmpty())
            return;

        QMapIterator<QString, QVariant> i(options);

        while (i.hasNext())
        {
            i.next();
            const QVariant::Type vt = i.value().type();

            if (vt == QVariant::Map)
                continue;

            const QByteArray key(i.key().toUtf8());

            qCDebug(DIGIKAM_QTAV_LOG).noquote()
                << QString::asprintf("%s => %s",
                    i.key().toUtf8().constData(),
                    i.value().toByteArray().constData());

            if      ((vt == QVariant::Int) || (vt == QVariant::UInt) || (vt == QVariant::Bool))
            {
                // QVariant.toByteArray(): "true" or "false", can not recognized by avcodec

                av_opt_set_int(obj, key.constData(), i.value().toInt(), AV_OPT_SEARCH_CHILDREN);
            }
            else if ((vt == QVariant::LongLong) || (vt == QVariant::ULongLong))
            {
                av_opt_set_int(obj, key.constData(), i.value().toLongLong(), AV_OPT_SEARCH_CHILDREN);
            }
            else if (vt == QVariant::Double)
            {
                av_opt_set_double(obj, key.constData(), i.value().toDouble(), AV_OPT_SEARCH_CHILDREN);
            }
        }

        return;
    }

    QVariantHash options(opt.toHash());

    if (options.isEmpty())
        return;

    QHashIterator<QString, QVariant> i(options);

    while (i.hasNext())
    {
        i.next();
        const QVariant::Type vt = i.value().type();

        if (vt == QVariant::Hash)
            continue;

        const QByteArray key(i.key().toUtf8());

        qCDebug(DIGIKAM_QTAV_LOG).noquote()
            << QString::asprintf("%s => %s",
                i.key().toUtf8().constData(),
                i.value().toByteArray().constData());

        if      ((vt == QVariant::Int) || (vt == QVariant::UInt) || (vt == QVariant::Bool))
        {
            av_opt_set_int(obj, key.constData(), i.value().toInt(), AV_OPT_SEARCH_CHILDREN);
        }
        else if ((vt == QVariant::LongLong) || (vt == QVariant::ULongLong))
        {
            av_opt_set_int(obj, key.constData(), i.value().toLongLong(), AV_OPT_SEARCH_CHILDREN);
        }
    }
}

// FIXME: why to lower case?

void setOptionsToDict(const QVariant& opt, AVDictionary** dict)
{
    if (!opt.isValid())
        return;

    if (opt.type() == QVariant::Map)
    {
        QVariantMap options(opt.toMap());

        if (options.isEmpty())
            return;

        QMapIterator<QString, QVariant> i(options);

        while (i.hasNext())
        {
            i.next();
            const QVariant::Type vt = i.value().type();

            if (vt == QVariant::Map)
                continue;

            const QByteArray key(i.key().toUtf8());

            switch (vt)
            {
                case QVariant::Bool:
                {
                    // QVariant.toByteArray(): "true" or "false", can not recognized by avcodec

                    av_dict_set(dict, key.constData(), QByteArray::number(i.value().toInt()).constData(), 0);

                    break;
                }

                default:
                {
                    // key and value are in lower case

                    av_dict_set(dict, i.key().toUtf8().constData(), i.value().toByteArray().constData(), 0);

                    break;
                }
            }

            qCDebug(DIGIKAM_QTAV_LOG).noquote()
                << QString::asprintf("dict: %s => %s",
                    i.key().toUtf8().constData(),
                    i.value().toByteArray().constData());
        }

        return;
    }

    QVariantHash options(opt.toHash());

    if (options.isEmpty())
        return;

    QHashIterator<QString, QVariant> i(options);

    while (i.hasNext())
    {
        i.next();
        const QVariant::Type vt = i.value().type();

        if (vt == QVariant::Hash)
            continue;

        const QByteArray key(i.key().toUtf8());

        switch (vt)
        {
            case QVariant::Bool:
            {
                // QVariant.toByteArray(): "true" or "false", can not recognized by avcodec

                av_dict_set(dict, key.constData(), QByteArray::number(i.value().toInt()).constData(), 0);

                break;
            }

            default:
            {
                // key and value are in lower case

                av_dict_set(dict, i.key().toUtf8().constData(), i.value().toByteArray().constData(), 0);

                break;
            }
        }

        qCDebug(DIGIKAM_QTAV_LOG).noquote()
            << QString::asprintf("dict: %s => %s",
                i.key().toUtf8().constData(),
                i.value().toByteArray().constData());
    }
}

void setOptionsForQObject(const QVariant& opt, QObject* const obj)
{
    if (!opt.isValid())
        return;

    qCDebug(DIGIKAM_QTAV_LOG)
        << QString::fromUtf8("set %1 (%2) meta properties:")
            .arg(QLatin1String(obj->metaObject()->className()))
            .arg(obj->objectName());

    if (opt.type() == QVariant::Hash)
    {
        QVariantHash options(opt.toHash());

        if (options.isEmpty())
            return;

        QHashIterator<QString, QVariant> i(options);

        while (i.hasNext())
        {
            i.next();

            if (i.value().type() == QVariant::Hash) // for example "vaapi": {...}
                continue;

            obj->setProperty(i.key().toUtf8().constData(), i.value());

            qCDebug(DIGIKAM_QTAV_LOG).noquote()
                << QString::asprintf("%s => %s",
                    i.key().toUtf8().constData(),
                    i.value().toByteArray().constData());
        }
    }

    if (opt.type() != QVariant::Map)
        return;

    QVariantMap options(opt.toMap());

    if (options.isEmpty())
        return;

    QMapIterator<QString, QVariant> i(options);

    while (i.hasNext())
    {
        i.next();

        if (i.value().type() == QVariant::Map) // for example "vaapi": {...}
            continue;

        obj->setProperty(i.key().toUtf8().constData(), i.value());

        qCDebug(DIGIKAM_QTAV_LOG).noquote()
            << QString::asprintf("%s => %s",
                i.key().toUtf8().constData(),
                i.value().toByteArray().constData());
    }
}

} // namespace Internal

} // namespace QtAV
