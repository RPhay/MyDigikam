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

// Qt includes

#include <QApplication>
#include <QDir>
#include <QIcon>
#include <QMessageBox>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "AVPlayer.h"
#include "VideoOutput.h"
#include "QtAVWidgets.h"
#include "PropertyEditor.h"
#include "MainWindow.h"
#include "common.h"
#include "digikam_debug.h"
#include "digikam_globals.h"

using namespace QtAV;
using namespace AVPlayer;
using namespace Digikam;

static const struct
{
    const char*     name;
    VideoRendererId id;
} vid_map[] =
{
    { "opengl", VideoRendererId_OpenGLWidget },
    { "gl",     VideoRendererId_GLWidget2    },
    { "d2d",    VideoRendererId_Direct2D     },
    { "gdi",    VideoRendererId_GDI          },
    { "xv",     VideoRendererId_XV           },
    { "x11",    VideoRendererId_X11          },
    { "qt",     VideoRendererId_Widget       },
    { 0,        0                            }
};

VideoRendererId rendererId_from_opt_name(const QString& name)
{
    for (int i = 0 ; vid_map[i].name ; ++i)
    {
        if (name == QLatin1String(vid_map[i].name))
            return vid_map[i].id;
    }

#ifndef QT_NO_OPENGL
#   if defined(Q_OS_ANDROID) || defined(Q_OS_IOS)

    return VideoRendererId_OpenGLWidget; // qglwidget is not suitable for android

#   endif

    return VideoRendererId_GLWidget2;

#endif

    return VideoRendererId_Widget;
}

int main(int argc, char* argv[])
{
    qCDebug(DIGIKAM_QTAVPLAYER_LOG) << aboutQtAV_PlainText();

    KLocalizedString::setApplicationDomain("digikam");
    ConfigManager::setName(QString::fromLatin1("Player"));
    QOptions options = get_common_options();
    options.add(QString::fromLatin1("player options"))
            ("ffmpeg-log",  QString(), QString::fromLatin1("ffmpeg log level. can be: quiet, panic, fatal, error, warn, info, verbose, debug. this can override env 'QTAV_FFMPEG_LOG'"))
            ("vd-list",                QString::fromLatin1("List video decoders and their properties"))
            ("-vo",

#ifndef QT_NO_OPENGL
#   if defined(Q_OS_ANDROID) || defined(Q_OS_IOS)

             QString::fromLatin1("opengl")

#   else

             QString::fromLatin1("gl")

#   endif
#else

             QString::fromLatin1("qt")

#endif

             , QString::fromLatin1("video renderer engine. can be gl, qt, d2d, gdi, xv, x11."))
    ;

    options.parse(argc, argv);
    do_common_options_before_qapp(options);

    if (options.value(QString::fromLatin1("vd-list")).toBool())
    {
        PropertyEditor pe;
        VideoDecoderId* vid = nullptr;

        while ((vid = VideoDecoder::next(vid)) != nullptr)
        {
            VideoDecoder* const vd = VideoDecoder::create(*vid);
            pe.getProperties(vd);

            qCDebug(DIGIKAM_QTAVPLAYER_LOG).noquote() << QString::asprintf("- %s:", vd->name().toUtf8().constData());
            qCDebug(DIGIKAM_QTAVPLAYER_LOG) << pe.buildOptions().toUtf8().constData();
        }

        exit(0);
    }

    QApplication app(argc, argv);
    app.setAttribute(Qt::AA_DontCreateNativeWidgetSiblings);
    qCDebug(DIGIKAM_QTAVPLAYER_LOG) << app.arguments();

    // See bug #438701

    installQtTranslationFiles(app);

    tryInitDrMingw();

    // Force to use application icon for non plasma desktop as Unity for ex.

    QApplication::setWindowIcon(QIcon::fromTheme(QLatin1String("avplayer"), app.windowIcon()));

    app.setApplicationDisplayName(QString::fromLatin1("AVPlayer"));
    QDir::setCurrent(qApp->applicationDirPath());

    do_common_options(options);
    set_opengl_backend(options.option(QString::fromLatin1("gl")).value().toString(), app.arguments().first());
    QtAV::setFFmpegLogLevel(options.value(QString::fromLatin1("ffmpeg-log")).toByteArray());

    QOption op = options.option(QString::fromLatin1("vo"));
    QString vo = op.value().toString();

    if (!op.isSet())
    {
        QString exe(app.arguments().at(0));
        int i = exe.lastIndexOf(QLatin1Char('-'));

        if (i > 0)
        {
            vo = exe.mid(i+1, exe.indexOf(QLatin1Char('.')) - i - 1);
        }
    }

    qCDebug(DIGIKAM_QTAVPLAYER_LOG).noquote() << QString::asprintf("vo: %s", vo.toUtf8().constData());

    MainWindow window;
    window.setProperty("rendererId", rendererId_from_opt_name(vo.toLower()));
    window.show();
    window.setWindowTitle(QString::fromLatin1("AVPlayer %1").arg(QtAV_Version_String_Long()));
    AppEventFilter ae(&window);
    qApp->installEventFilter(&ae);

    int x = window.x();
    int y = window.y();
    op    = options.option(QString::fromLatin1("width"));
    int w = op.value().toInt();
    op    = options.option(QString::fromLatin1("height"));
    int h = op.value().toInt();
    op    = options.option(QString::fromLatin1("x"));

    if (op.isSet())
        x = op.value().toInt();

    op = options.option(QString::fromLatin1("y"));

    if (op.isSet())
        y = op.value().toInt();

    window.resize(w, h);
    window.move(x, y);

    if (options.value(QString::fromLatin1("fullscreen")).toBool())
        window.showFullScreen();        // krazy:exclude=qmethods

    op = options.option(QString::fromLatin1("ao"));

    if (op.isSet())
    {
        QString aos(op.value().toString());
        QStringList ao;

        if (aos.contains(QString::fromLatin1(";")))
            ao = aos.split(QString::fromLatin1(";"),

#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)

            Qt::SkipEmptyParts

#else

            QString::SkipEmptyParts

#endif

            );
        else
            ao = aos.split(QString::fromLatin1(","),

#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)

            Qt::SkipEmptyParts

#else

            QString::SkipEmptyParts

#endif

            );

        window.setAudioBackends(ao);
    }

    op = options.option(QString::fromLatin1("vd"));

    if (op.isSet())
    {
        QStringList vd = op.value().toString().split(QString::fromLatin1(";"),

#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)

            Qt::SkipEmptyParts

#else

            QString::SkipEmptyParts

#endif

        );

        if (!vd.isEmpty())
            window.setVideoDecoderNames(vd);
    }

    op = options.option(QString::fromLatin1("file"));

    if (op.isSet())
    {
        qCDebug(DIGIKAM_QTAVPLAYER_LOG) << "-f set: " << op.value().toString();
        window.play(op.value().toString());
    }
    else
    {
        if ((argc > 1)                                            &&
            !app.arguments().last().startsWith(QLatin1Char('-'))  &&
            !app.arguments().at(argc-2).startsWith(QLatin1Char('-')))
        {
            window.play(app.arguments().last());
        }
    }

    int ret = app.exec();         // krazy:exclude=crashy

    return ret;
}
