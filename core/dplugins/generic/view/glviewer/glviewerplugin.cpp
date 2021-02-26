/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2018-07-30
 * Description : a plugin to preview image with OpenGL.
 *
 * Copyright (C) 2018-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "glviewerplugin.h"

// Qt includes

#include <QPointer>
#include <QMessageBox>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "glviewerwidget.h"
#include "digikam_debug.h"

namespace DigikamGenericGLViewerPlugin
{

GLViewerPlugin::GLViewerPlugin(QObject* const parent)
    : DPluginGeneric(parent)
{
}

GLViewerPlugin::~GLViewerPlugin()
{
}

QString GLViewerPlugin::name() const
{
    return i18n("OpenGL Viewer");
}

QString GLViewerPlugin::iid() const
{
    return QLatin1String(DPLUGIN_IID);
}

QIcon GLViewerPlugin::icon() const
{
    return QIcon::fromTheme(QLatin1String("show-gpu-effects"));
}

QString GLViewerPlugin::description() const
{
    return i18n("A tool to preview image with OpenGL");
}

QString GLViewerPlugin::details() const
{
    return i18n("<p>This tool preview a series of items using OpenGL effects.</p>"
                "<p><b>Image Access</b></p>"
                "<ul><li>Next image: scroll-wheel down | down arrow | right arrow | PgDown | Space | n</li>"
                "<li>Previous image: scroll-wheel up | up arrow | left arrow | PgUp | p</li>"
                "<li>Quit: Esc</li></ul>"
                "<p><b>Display</b></p>"
                "<ul><li>Toggle fulls-creen to normal: f</li>"
                "<li>Toggle scroll-wheel action: c (either zoom or change image)</li>"
                "<li>Rotation: r</li>"
                "<li>Reset view: double click</li>"
                "<li>Original size: o</li></ul"
                "<p><b>Zooming</b></p>"
                "<ul><li>Move mouse in up-down-direction while pressing the right mouse button</li>"
                "<li>Alternatively, press c and use the scrollwheel</li>"
                "<li>Plus-minus</li>"
                "<li>ctrl + scrollwheel</li></ul>"
                "<p><b>Panning</b></p>"
                "<ul><li>Move mouse while pressing the left button</li></ul>");
}

QList<DPluginAuthor> GLViewerPlugin::authors() const
{
    return QList<DPluginAuthor>()
            << DPluginAuthor(QString::fromUtf8("Markus Leuthold"),
                             QString::fromUtf8("kusi at forum dot titlis dot org"),
                             QString::fromUtf8("(C) 2007-2008"))
            << DPluginAuthor(QString::fromUtf8("Gilles Caulier"),
                             QString::fromUtf8("caulier dot gilles at gmail dot com"),
                             QString::fromUtf8("(C) 2008-2021"))
            ;
}

void GLViewerPlugin::setup(QObject* const parent)
{
    DPluginAction* const ac = new DPluginAction(parent);
    ac->setIcon(icon());
    ac->setText(i18nc("@action", "OpenGL Image Viewer"));
    ac->setObjectName(QLatin1String("glviewer"));
    ac->setActionCategory(DPluginAction::GenericView);

    connect(ac, SIGNAL(triggered(bool)),
            this, SLOT(slotGLViewer()));

    addAction(ac);
}

void GLViewerPlugin::slotGLViewer()
{
    DInfoInterface* const iface   = infoIface(sender());
    QPointer<GLViewerWidget> view = new GLViewerWidget(this, iface);

    if (view->listOfFilesIsEmpty())
    {
        return;
    }

    switch (view->getOGLstate())
    {
        case oglOK:
        {
            view->show();
            break;
        }

        case oglNoRectangularTexture:
        {
            qCCritical(DIGIKAM_DPLUGIN_GENERIC_LOG) << "GL_ARB_texture_rectangle not supported";
            QMessageBox::critical(nullptr, i18n("OpenGL error"), i18n("GL_ARB_texture_rectangle not supported"));
            break;
        }

        case oglNoContext:
        {
            qCCritical(DIGIKAM_DPLUGIN_GENERIC_LOG) << "no OpenGL context found";
            QMessageBox::critical(nullptr, i18n("OpenGL error"), i18n("No OpenGL context found"));
            break;
        }
    }
}

} // namespace DigikamGenericGLViewerPlugin
