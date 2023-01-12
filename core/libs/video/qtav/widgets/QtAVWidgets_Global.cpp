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

#include "QtAVWidgets_Global.h"

// Qt includes

#include <QApplication>
#include <QBoxLayout>
#include <QMessageBox>
#include <QPushButton>
#include <QTabWidget>
#include <QTextBrowser>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "WidgetRenderer.h"
#include "GraphicsItemRenderer.h"

#if QTAV_HAVE(GL)
#   include "GLWidgetRenderer.h"        // Qt5 and Qt6
#   include "GLWidgetRenderer2.h"       // Qt5
#endif

#ifndef QT_NO_OPENGL
#   include "OpenGLWidgetRenderer.h"
#endif

#include "QtAV_factory.h"
#include "QtAV_mkid.h"
#include "digikam_debug.h"

namespace QtAV
{

VideoRendererId VideoRendererId_Widget          = mkid::id32base36_6<'W', 'i', 'd', 'g', 'e', 't'>::value;
VideoRendererId VideoRendererId_OpenGLWidget    = mkid::id32base36_6<'Q', 'O', 'G', 'L', 'W', 't'>::value;
VideoRendererId VideoRendererId_GLWidget2       = mkid::id32base36_6<'Q', 'G', 'L', 'W', 't', '2'>::value;
VideoRendererId VideoRendererId_GLWidget        = mkid::id32base36_6<'Q', 'G', 'L', 'W', 't', '1'>::value;
VideoRendererId VideoRendererId_GraphicsItem    = mkid::id32base36_6<'Q', 'G', 'r', 'a', 'p', 'h'>::value;
VideoRendererId VideoRendererId_GDI             = mkid::id32base36_3<'G', 'D', 'I'>::value;
VideoRendererId VideoRendererId_Direct2D        = mkid::id32base36_3<'D', '2', 'D'>::value;
VideoRendererId VideoRendererId_XV              = mkid::id32base36_6<'X', 'V', 'i', 'd', 'e', 'o'>::value;
VideoRendererId VideoRendererId_X11             = mkid::id32base36_3<'X', '1', '1'>::value;

// QPainterRenderer is abstract. So can not register(operator new will needed)

#if AUTO_REGISTER

FACTORY_REGISTER(VideoRenderer, Widget,         "QWidegt")
FACTORY_REGISTER(VideoRenderer, GraphicsItem,   "QGraphicsItem")

#   if QTAV_HAVE(GL)

FACTORY_REGISTER(VideoRenderer, GLWidget,       "QGLWidegt")
FACTORY_REGISTER(VideoRenderer, GLWidget2,      "QGLWidegt2")

#   endif

#   ifndef QT_NO_OPENGL

FACTORY_REGISTER(VideoRenderer, OpenGLWidget,   "OpenGLWidget")

#   endif
#endif

extern bool RegisterVideoRendererGDI_Man();
extern bool RegisterVideoRendererDirect2D_Man();
extern bool RegisterVideoRendererXV_Man();
extern bool RegisterVideoRendererX11_Man();

namespace Widgets
{

void registerRenderers()
{

    qCDebug(DIGIKAM_QTAV_LOG) << "Register QtAV Renderers:";

    // check whether it is called

    static bool initialized = false;

    if (initialized)
        return;

    initialized = true;

    // factory.h does not check whether an id is registered

    if (VideoRenderer::name(VideoRendererId_Widget))
    {
        return;
    }

#ifndef QT_NO_OPENGL

    qCDebug(DIGIKAM_QTAV_LOG) << "   Qt have no OpenGL support.";

    qCDebug(DIGIKAM_QTAV_LOG)
        << "   register QtAV::OpenGLWidget Renderer:"
        << VideoRenderer::Register<OpenGLWidgetRenderer>(VideoRendererId_OpenGLWidget, "OpenGLWidget");

#endif

#if QTAV_HAVE(GL)

    qCDebug(DIGIKAM_QTAV_LOG) << "   QtAV have OpenGL support.";

    qCDebug(DIGIKAM_QTAV_LOG)
        << "   register QtAV::QGLWidget Renderer:"
        << VideoRenderer::Register<GLWidgetRenderer>(VideoRendererId_GLWidget,         "QGLWidget");

    qCDebug(DIGIKAM_QTAV_LOG)
        << "   register QtAV::QGLWidget2 Renderer:"
        << VideoRenderer::Register<GLWidgetRenderer2>(VideoRendererId_GLWidget2,       "QGLWidget2");

#endif

    qCDebug(DIGIKAM_QTAV_LOG)
        << "   register QtAV::Widget Renderer:"
        << VideoRenderer::Register<WidgetRenderer>(VideoRendererId_Widget,             "Widget");

#if QTAV_HAVE(GDIPLUS)

    qCDebug(DIGIKAM_QTAV_LOG) << "   QtAV have GDI support.";

    qCDebug(DIGIKAM_QTAV_LOG) << "   register QtAV::GDI Renderer:"
                              << RegisterVideoRendererGDI_Man();

#endif

#if QTAV_HAVE(DIRECT2D)

    qCDebug(DIGIKAM_QTAV_LOG) << "   QtAV have Direct2D support.";

    qCDebug(DIGIKAM_QTAV_LOG) << "   register QtAV::Direct2D Renderer:"
                              << RegisterVideoRendererDirect2D_Man();

#endif

#if QTAV_HAVE(XV)

    qCDebug(DIGIKAM_QTAV_LOG) << "   QtAV have XVideo support.";

    qCDebug(DIGIKAM_QTAV_LOG) << "   register QtAV::XV Renderer:"
                              << RegisterVideoRendererXV_Man();

#endif

#if QTAV_HAVE(X11)

    qCDebug(DIGIKAM_QTAV_LOG) << "   QtAV have X11 support.";

    qCDebug(DIGIKAM_QTAV_LOG) << "   register QtAV::X11 Renderer:"
                              << RegisterVideoRendererX11_Man();

#endif

    qCDebug(DIGIKAM_QTAV_LOG)
        << "   register QtAV::GraphicsItem Renderer:"
        << VideoRenderer::Register<GraphicsItemRenderer>(VideoRendererId_GraphicsItem, "GraphicsItem");
}

} // namespace Widgets

namespace
{
    static const struct Q_DECL_HIDDEN register_renderers
    {
        inline register_renderers()
        {
            QtAV::Widgets::registerRenderers();
        }
    } sRegisterVO;
}

void about()
{
    // we should use new because a qobject will delete it's children

    QTextBrowser* const viewQtAV   = new QTextBrowser;
    QTextBrowser* const viewFFmpeg = new QTextBrowser;
    viewQtAV->setOpenExternalLinks(true);
    viewFFmpeg->setOpenExternalLinks(true);
    viewQtAV->setHtml(aboutQtAV_HTML());
    viewFFmpeg->setHtml(aboutFFmpeg_HTML());
    QTabWidget* const tab          = new QTabWidget;
    tab->addTab(viewQtAV,   QLatin1String("QtAV"));
    tab->addTab(viewFFmpeg, QLatin1String("FFmpeg"));
    QPushButton* const qbtn        = new QPushButton(i18n("About Qt"));
    QPushButton* const btn         = new QPushButton(i18n("Ok"));
    QHBoxLayout* const btnLayout   = new QHBoxLayout;
    btnLayout->addWidget(btn);
    btnLayout->addStretch();
    btnLayout->addWidget(qbtn);
    btn->setFocus();
    QDialog dialog;
    dialog.setWindowTitle(i18nc("@title:window", "About QtAV"));
    QVBoxLayout* const layout      = new QVBoxLayout;
    dialog.setLayout(layout);
    layout->addWidget(tab);
    layout->addLayout(btnLayout);

    QObject::connect(qbtn, SIGNAL(clicked()),
                     qApp, SLOT(aboutQt()));

    QObject::connect(btn, SIGNAL(clicked()),
                     &dialog, SLOT(accept()));

    dialog.exec();
}

void aboutFFmpeg()
{
    QMessageBox::about(nullptr, i18nc("@title:window", "About FFmpeg"), aboutFFmpeg_HTML());
}

void aboutQtAV()
{
    QMessageBox::about(nullptr, i18nc("@title:window", "About QtAV"), aboutQtAV_HTML());
}

} // namespace QtAV
