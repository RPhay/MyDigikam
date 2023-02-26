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

#include "common.h"

// C++ includes

#include <cstdio>
#include <cstdlib>

// Qt includes

#include <QFileOpenEvent>
#include <QLocale>
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QMutex>

// KDE includes

#include <kconfiggroup.h>
#include <ksharedconfig.h>

// Local includes

#include "digikam_debug.h"
#include "AVPlayerConfigMngr.h"

// Windows includes

#ifdef Q_OS_WINRT
#   include <wrl.h>
#   include <windows.foundation.h>
#   include <windows.storage.pickers.h>
#   include <Windows.ApplicationModel.activation.h>
#   include <qfunctions_winrt.h>

using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;
using namespace ABI::Windows::ApplicationModel::Activation;
using namespace ABI::Windows::Foundation;
using namespace ABI::Windows::Foundation::Collections;
using namespace ABI::Windows::Storage;
using namespace ABI::Windows::Storage::Pickers;

#   define COM_LOG_COMPONENT  "WinRT"
#   define COM_ENSURE(f, ...) COM_CHECK(f, return __VA_ARGS__;)
#   define COM_WARN(f)        COM_CHECK(f)

#   define COM_CHECK(f, ...)                                                                \
    do {                                                                                    \
        HRESULT hr = f;                                                                     \
        if (FAILED(hr)) {                                                                   \
            qCWarning(DIGIKAM_AVPLAYER_LOG)                                                 \
                << QString::fromLatin1(COM_LOG_COMPONENT " error@%1. " #f ": (0x%2) %3")    \
                    .arg(__LINE__)                                                          \
                    .arg(hr, 0, 16)                                                         \
                    .arg(qt_error_string(hr));                                              \
            __VA_ARGS__                                                                     \
        }                                                                                   \
    } while (0)

QString UrlFromFileArgs(IInspectable* args)
{
    ComPtr<IFileActivatedEventArgs> fileArgs;
    COM_ENSURE(args->QueryInterface(fileArgs.GetAddressOf()), QString());
    ComPtr<IVectorView<IStorageItem*>> files;
    COM_ENSURE(fileArgs->get_Files(&files), QString());
    ComPtr<IStorageItem> item;
    COM_ENSURE(files->GetAt(0, &item), QString());
    HString path;
    COM_ENSURE(item->get_Path(path.GetAddressOf()), QString());

    quint32 pathLen;
    const wchar_t* pathStr = path.GetRawBuffer(&pathLen);
    const QString filePath = QString::fromWCharArray(pathStr, pathLen);

    qCDebug(DIGIKAM_AVPLAYER_LOG) << "file path: " << filePath;

    item->AddRef(); // ensure we can access it later. TODO: how to release?

    return (QString::fromUtf8("winrt:@%1:%2").arg((qint64)(qptrdiff)item.Get()).arg(filePath));
}

#endif // Q_OS_WINRT

using namespace QtAV;

namespace AVPlayer
{

QOptions get_common_options()
{
    static QOptions ops = QOptions().addDescription(QString::fromLatin1("Options for QtAV players"))
            .add(QString::fromLatin1("common options"))
            ("help,h",                  QLatin1String("print this"))
            ("ao",           QString(), QLatin1String("audio output. Can be ordered combination of "
                                                      "available backends (-ao help). Leave empty "
                                                      "to use the default setting. Set 'null' to disable audio."))
            ("-egl",                    QLatin1String("Use EGL. Only works for Qt>=5.5+XCB"))
            ("-gl",                     QLatin1String("OpenGL backend for Qt>=5.4(windows). can be 'desktop', "
                                                      "'opengles' and 'software'"))
            ("x",            0,         QString())
            ("y",            0,         QLatin1String("y"))
            ("-width",       800,       QLatin1String("width of player"))
            ("height",       450,       QLatin1String("height of player"))
            ("fullscreen",              QLatin1String("fullscreen"))
            ("decoder",                 QLatin1String("FFmpeg"), QLatin1String("use a given decoder"))
            ("decoders,-vd",            QLatin1String("cuda;vaapi;vda;dxva;cedarv;ffmpeg"),
                                        QLatin1String("decoder name list in priority order separated by ';'"))
            ("file,f",       QString(), QLatin1String("file or url to play"))
    ;

    return ops;
}

void do_common_options_before_qapp(const QOptions& options)
{

#ifdef Q_OS_LINUX

    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group        = config->group(QLatin1String("MediaPlayer OpenGL"));
    const bool set_egl        = group.readEntry(QLatin1String("egl"), false);

    // https://bugreports.qt.io/browse/QTBUG-49529
    // it's too late if qApp is created. but why ANGLE is not?

    if (options.value(QString::fromLatin1("egl")).toBool() || set_egl)
    {
        // FIXME: Config is constructed too early because it requires qApp
        // only apply to current run. no config change

        qputenv("QT_XCB_GL_INTEGRATION", "xcb_egl");
    }
    else
    {
        qputenv("QT_XCB_GL_INTEGRATION", "xcb_glx");
    }

    qCDebug(DIGIKAM_AVPLAYER_LOG) << "QT_XCB_GL_INTEGRATION: "
                                  << qgetenv("QT_XCB_GL_INTEGRATION");

#else

    Q_UNUSED(options);

#endif // Q_OS_LINUX

}

void do_common_options(const QOptions& options, const QString& /*appName*/)
{
    if (options.value(QString::fromLatin1("help")).toBool())
    {
        options.print();
        exit(0);
    }
}

void set_opengl_backend(const QString& glopt, const QString& appname)
{
    QString gl = appname.toLower().replace(QLatin1String("\\"), QLatin1String("/"));
    int idx    = gl.lastIndexOf(QLatin1String("/"));

    if (idx >= 0)
    {
        gl = gl.mid(idx + 1);
    }

    idx = gl.lastIndexOf(QLatin1String("."));

    if (idx > 0)
    {
        gl = gl.left(idx);
    }

    if      (gl.indexOf(QLatin1String("-desktop")) > 0)
    {
        gl = QLatin1String("desktop");
    }
    else if ((gl.indexOf(QLatin1String("-es")) > 0) || (gl.indexOf(QLatin1String("-angle")) > 0))
    {
        gl = gl.mid(gl.indexOf(QLatin1String("-es")) + 1);
    }
    else if ((gl.indexOf(QLatin1String("-sw")) > 0) || (gl.indexOf(QLatin1String("-software")) > 0))
    {
        gl = QLatin1String("software");
    }
    else
    {
        gl = glopt.toLower();
    }

    if (gl.isEmpty())
    {
        switch (AVPlayerConfigMngr::instance().openGLType())
        {
            case AVPlayerConfigMngr::Desktop:
            {
                gl = QLatin1String("desktop");

                break;
            }

            case AVPlayerConfigMngr::OpenGLES:
            {
                gl = QLatin1String("es");

                break;
            }

            case AVPlayerConfigMngr::Software:
            {
                gl = QLatin1String("software");

                break;
            }

            default:
            {
                break;
            }
        }
    }

    if ((gl == QLatin1String("es"))     ||
        (gl == QLatin1String("angle"))  ||
        (gl == QLatin1String("opengles")))
    {
        gl = QLatin1String("es_");
        gl.append(AVPlayerConfigMngr::instance().getANGLEPlatform().toLower());
    }

    if      (gl.startsWith(QLatin1String("es")))
    {
        qApp->setAttribute(Qt::AA_UseOpenGLES);

#ifdef QT_OPENGL_DYNAMIC

        qputenv("QT_OPENGL", "angle");

#endif

#ifdef Q_OS_WIN

        if      (gl.endsWith(QLatin1String("d3d11")))
        {
            qputenv("QT_ANGLE_PLATFORM", "d3d11");
        }
        else if (gl.endsWith(QLatin1String("d3d9")))
        {
            qputenv("QT_ANGLE_PLATFORM", "d3d9");
        }
        else if (gl.endsWith(QLatin1String("warp")))
        {
            qputenv("QT_ANGLE_PLATFORM", "warp");
        }

#endif

    }
    else if (gl == QLatin1String("desktop"))
    {
        qApp->setAttribute(Qt::AA_UseDesktopOpenGL);
    }
    else if (gl == QLatin1String("software"))
    {
        qApp->setAttribute(Qt::AA_UseSoftwareOpenGL);
    }
}

AppEventFilter::AppEventFilter(QObject* const player, QObject* const parent)
    : QObject (parent),
      m_player(player)
{
}

bool AppEventFilter::eventFilter(QObject* obj, QEvent* ev)
{
/*
    qCDebug(DIGIKAM_AVPLAYER_LOG) << __FUNCTION__
                                  << " watcher: "
                                  << obj << ev;
*/
    if (obj != qApp)
    {
        return false;
    }

    if (ev->type() == QEvent::WinEventAct)
    {
        // winrt file open/pick. since qt5.6.1

        qCDebug(DIGIKAM_AVPLAYER_LOG).noquote()
            << QString::asprintf("QEvent::WinEventAct");

#ifdef Q_OS_WINRT

        class Q_DECL_HIDDEN QActivationEvent : public QEvent
        {
        public:

            void* args() const
            {
                // IInspectable*

                return d;
            }
        };

        QActivationEvent* const ae = static_cast<QActivationEvent*>(ev);
        const QString url(UrlFromFileArgs((IInspectable*)ae->args()));

        if (!url.isEmpty())
        {
            qCDebug(DIGIKAM_AVPLAYER_LOG) << "winrt url: " << url;

            if (m_player)
            {
                QMetaObject::invokeMethod(m_player, "play", Q_ARG(QUrl, QUrl(url)));
            }
        }

        return true;
#endif

    }

    if (ev->type() != QEvent::FileOpen)
    {
        return false;
    }

    QFileOpenEvent* const foe = static_cast<QFileOpenEvent*>(ev);

    if (m_player)
    {
        QMetaObject::invokeMethod(m_player, "play", Q_ARG(QUrl, QUrl(foe->url())));
    }

    return true;
}

} // namespace AVPlayer
