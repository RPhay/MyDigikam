/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-06-15
 * Description : digiKam 8/16 bits image management API.
 *               Private data container.
 *
 * Copyright (C) 2005      by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2005-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_DIMG_PRIVATE_H
#define DIGIKAM_DIMG_PRIVATE_H

#include "digikam_config.h"

// C ANSI includes

#ifndef Q_OS_WIN
extern "C"
{
#endif

#include <stdint.h>

#ifndef Q_OS_WIN
#include <math.h>
}
#endif

// C++ includes

#include <cstdio>

// Qt includes

#include <QCryptographicHash>
#include <QSharedData>
#include <QString>
#include <QByteArray>
#include <QVariant>
#include <QMap>
#include <QFile>
#include <QImage>
#include <QImageReader>
#include <QPaintEngine>
#include <QPainter>
#include <QPixmap>
#include <QSysInfo>
#include <QUuid>
#include <QScopedPointer>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "dimg.h"
#include "dplugindimg.h"
#include "digikam_export.h"
#include "digikam_debug.h"
#include "dmetadata.h"
#include "dimagehistory.h"
#include "iccprofile.h"
#include "metaengine_rotation.h"
#include "drawdecoder.h"
#include "loadsavethread.h"
#include "filereadwritelock.h"
#include "iccmanager.h"
#include "icctransform.h"
#include "exposurecontainer.h"
#include "dimgloaderobserver.h"
#include "randomnumbergenerator.h"

/**
 * Lanczos kernel is precomputed in a table with this resolution
 * The value below seems to be enough for HQ upscaling up to eight times
 */
#define LANCZOS_TABLE_RES  256

/**
 * A support of 3 gives an overall sharper looking image, but
 * it is a) slower b) gives more sharpening artifacts
 */
#define LANCZOS_SUPPORT    2

/**
 * Define this to use a floating-point implementation of Lanczos interpolation.
 * The integer implementation is a little bit less accurate, but MUCH faster
 * (even on machines with FPU - ~2.5 times faster on Core2); besides, it will
 * run a hell lot faster on computers without a FPU (e.g. PDAs).
 */
//#define LANCZOS_DATA_FLOAT

#ifdef LANCZOS_DATA_FLOAT
#   define LANCZOS_DATA_TYPE float
#   define LANCZOS_DATA_ONE 1.0
#else
#   define LANCZOS_DATA_TYPE int
#   define LANCZOS_DATA_ONE 4096
#endif

typedef uint64_t ullong;    // krazy:exclude=typedefs
typedef int64_t  llong;     // krazy:exclude=typedefs

namespace Digikam
{

class DIGIKAM_EXPORT DImg::Private : public QSharedData
{
public:

    explicit Private()
      : null(true),
        alpha(false),
        sixteenBit(false),
        width(0),
        height(0),
        data(nullptr),
        lanczos_func(nullptr)
    {
    }

    ~Private()
    {
        delete [] data;
        delete [] lanczos_func;
    }

public:

    bool                    null;
    bool                    alpha;
    bool                    sixteenBit;

    unsigned int            width;
    unsigned int            height;

    unsigned char*          data;
    LANCZOS_DATA_TYPE*      lanczos_func;

    MetaEngineData          metaData;
    QMap<QString, QVariant> attributes;
    QMap<QString, QString>  embeddedText;
    IccProfile              iccProfile;
    DImageHistory           imageHistory;
};

// ----------------------------------------------------------------------------

class DIGIKAM_EXPORT DImgStaticPriv
{
public:

    static DPluginDImg* pluginForFile(const QFileInfo& fileInfo, bool magic)
    {
        QMultiMap<int, DPluginDImg*> pluginMap;

        foreach (DPlugin* const p, DPluginLoader::instance()->allPlugins())
        {
            int prio;
            DPluginDImg* const plug = dynamic_cast<DPluginDImg*>(p);

            if (plug && ((prio = plug->canRead(fileInfo, magic)) > 0))
            {
 /*
                qCDebug(DIGIKAM_DIMG_LOG) << "File path:" << filePath
                                          << "Priority:"  << prio
                                          << "Loader:"    << plug->loaderName();
 */
                pluginMap.insert(prio, plug);
            }
        }

        if (pluginMap.isEmpty())
        {
            return nullptr;
        }

        return pluginMap.first();
    }

    static DPluginDImg* pluginForFormat(const QString& format)
    {
        QMultiMap<int, DPluginDImg*> pluginMap;

        if (!format.isNull())
        {
            foreach (DPlugin* const p, DPluginLoader::instance()->allPlugins())
            {
                int prio;
                DPluginDImg* const plug = dynamic_cast<DPluginDImg*>(p);

                if (plug && ((prio = plug->canWrite(format)) > 0))
                {
                    pluginMap.insert(prio, plug);
                }
            }
        }

        if (pluginMap.isEmpty())
        {
            return nullptr;
        }

        return pluginMap.first();
    }

    static DImg::FORMAT loaderNameToFormat(const QString& name)
    {
        if      (name.isNull())
        {
            return DImg::NONE;
        }
        else if (name == QLatin1String("JPEG"))
        {
            return DImg::JPEG;
        }
        else if (name == QLatin1String("PNG"))
        {
            return DImg::PNG;
        }
        else if (name == QLatin1String("TIFF"))
        {
            return DImg::TIFF;
        }
        else if (name == QLatin1String("RAW"))
        {
            return DImg::RAW;
        }
        else if (name == QLatin1String("JPEG2000"))
        {
            return DImg::JP2K;
        }
        else if (name == QLatin1String("PGF"))
        {
            return DImg::PGF;
        }
        else if (name == QLatin1String("HEIF"))
        {
            return DImg::HEIF;
        }

        // In others cases, ImageMagick or QImage will be used to try to open file.
        return DImg::QIMAGE;
    }

    static QStringList fileOriginAttributes()
    {
        QStringList list;
        list << QLatin1String("format")
             << QLatin1String("isReadOnly")
             << QLatin1String("originalFilePath")
             << QLatin1String("originalSize")
             << QLatin1String("originalImageHistory")
             << QLatin1String("rawDecodingSettings")
             << QLatin1String("rawDecodingFilterAction")
             << QLatin1String("uniqueHash")
             << QLatin1String("uniqueHashV2");

        return list;
    }
};

} // namespace Digikam

#endif // DIGIKAM_DIMG_PRIVATE_H
