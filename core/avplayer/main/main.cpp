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

// Qt includes

#include <QApplication>
#include <QDir>
#include <QIcon>
#include <QMessageBox>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "AVPlayerCore.h"
#include "VideoOutput.h"
#include "PropertyEditor.h"
#include "MainWindow.h"
#include "common.h"
#include "digikam_debug.h"
#include "digikam_globals.h"

#ifdef Q_OS_WIN
#   include <windows.h>
#   include <shellapi.h>
#   include <objbase.h>
#endif

#if defined Q_OS_WIN
#   define MAIN_EXPORT __declspec(dllexport)
#   define MAIN_FN avplayer_main
#else
#   define MAIN_EXPORT
#   define MAIN_FN main
#endif

using namespace QtAV;
using namespace AVPlayer;
using namespace Digikam;

static const struct
{
    const char*     name = nullptr;
    VideoRendererId id   = 0;
}
    vid_map[] =
{
    { "opengl", VideoRendererId_OpenGLWidget },
    { "gl",     VideoRendererId_GLWidget2    },
    { "d2d",    VideoRendererId_Direct2D     },
    { "gdi",    VideoRendererId_GDI          },
    { "xv",     VideoRendererId_XV           },
    { "x11",    VideoRendererId_X11          },
    { "qt",     VideoRendererId_Widget       },
    { nullptr,  0                            }
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

extern "C" MAIN_EXPORT int MAIN_FN(int argc, char** argv)
{
    KLocalizedString::setApplicationDomain("digikam");

    QOptions options = get_common_options();
    options.add(QString::fromLatin1("player options"))
            ("ffmpeg-log",  QString(), QString::fromLatin1("ffmpeg log level. can be: quiet, panic, fatal,"
                                                           "error, warn, info, verbose, debug. This can "
                                                           "override env 'QTAV_FFMPEG_LOG'"))
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

            qCDebug(DIGIKAM_AVPLAYER_LOG).noquote()
                << QString::asprintf("- %s:", vd->name().toUtf8().constData());

            qCDebug(DIGIKAM_AVPLAYER_LOG)
                << pe.buildOptions().toUtf8().constData();
        }

        exit(0);
    }

    QApplication app(argc, argv);

    digikamSetDebugFilterRules(true);

    app.setAttribute(Qt::AA_DontCreateNativeWidgetSiblings);

    qCDebug(DIGIKAM_AVPLAYER_LOG) << app.arguments();

    // See bug #438701

    installQtTranslationFiles(app);

    tryInitDrMingw();

    // Force to use application icon for non plasma desktop as Unity for ex.

    QApplication::setWindowIcon(QIcon::fromTheme(QLatin1String("avplayer"), app.windowIcon()));
    app.setApplicationDisplayName(QString::fromLatin1("AVPlayer"));

#ifdef Q_OS_WIN

    if (QSysInfo::currentCpuArchitecture().contains(QLatin1String("64")) &&
        !QSysInfo::buildCpuArchitecture().contains(QLatin1String("64")))
    {
        QMessageBox::critical(qApp->activeWindow(),
                              qApp->applicationName(),
                              i18nc("#info", "<p>You are running AVPlayer as a 32-bit version on a 64-bit Windows.</p>"
                                    "<p>Please install the 64-bit version of AVPlayer to get "
                                    "a better experience with AVPlayer.</p>"));
    }

#endif

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
            vo = exe.mid(i + 1, exe.indexOf(QLatin1Char('.')) - i - 1);
        }
    }

    qCDebug(DIGIKAM_AVPLAYER_LOG).noquote() << QString::asprintf("vo: %s", vo.toUtf8().constData());

#ifdef Q_OS_WIN

    // Necessary to open native open with dialog on windows

    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

#endif

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
        qCDebug(DIGIKAM_AVPLAYER_LOG) << "-f set: " << op.value().toString();
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

#ifdef Q_OS_WIN

    // Necessary to open native open with dialog on windows

    CoUninitialize();

#endif

    return ret;
}
