/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2003-01-31
 * Description : a presentation tool.
 *
 * Copyright (C) 2006-2009 by Valerio Fuoglio <valerio dot fuoglio at gmail dot com>
 * Copyright (C) 2009      by Andi Clemens <andi dot clemens at googlemail dot com>
 * Copyright (C) 2003-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2012-2020 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2020      by Minh Nghia Duong <minhnghiaduong997 at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "slideshowmngr.h"

// C++ includes

#include <cstdlib>

// Qt includes
#include <QApplication>


namespace DigikamGenericSlideShowPlugin
{

SlideShowMngr::SlideShowMngr(QObject* const parent, DInfoInterface* const iface)
    : QObject(parent),
      m_plugin(nullptr),
      m_dialog(nullptr)
{
      //m_sharedData = new PresentationContainer();
      //m_sharedData->iface = iface;
}

SlideShowMngr::~SlideShowMngr()
{
    delete m_dialog;
    //delete m_sharedData;
}

void SlideShowMngr::setPlugin(DPlugin* const plugin)
{
    m_plugin = plugin;
}

void SlideShowMngr::addFiles(const QList<QUrl>& urls)
{
    //m_sharedData->urlList = urls;
}


void SlideShowMngr::showConfigDialog()
{
    m_dialog = new SetupSlideShowDialog(QApplication::activeWindow());

    m_dialog->setPlugin(m_plugin);

    m_dialog->show();
}


void SlideShowMngr::slotSlideShow()
{
/*
    KConfig config;
    KConfigGroup grp = config.group("Presentation Settings");
    bool opengl      = grp.readEntry("OpenGL",  false);
    bool shuffle     = grp.readEntry("Shuffle", false);
    bool wantKB      = grp.readEntry("Effect Name (OpenGL)") == QLatin1String("Ken Burns");

    if (m_sharedData->urlList.isEmpty())
    {
        QMessageBox::information(QApplication::activeWindow(), QString(), i18n("There are no images to show."));
        return;
    }

    if (shuffle)
    {
        qsrand(QTime::currentTime().msec());

        QList<QUrl>::iterator it = m_sharedData->urlList.begin();
        QList<QUrl>::iterator it1;

        for (uint i = 0 ; i < (uint) m_sharedData->urlList.size() ; ++i)
        {
            int inc = (int) (float(m_sharedData->urlList.count()) * qrand() / (RAND_MAX + 1.0));

            it1  = m_sharedData->urlList.begin();
            it1 += inc;

            std::swap(*(it++), *(it1));
        }
    }

    if (!opengl)
    {
        PresentationWidget* const slide = new PresentationWidget(m_sharedData);
        slide->show();
    }
    else
    {
#ifdef HAVE_OPENGL
        bool supportsOpenGL = true;

        if (wantKB)
        {
            PresentationKB* const slide = new PresentationKB(m_sharedData);
            slide->show();

            if (!slide->checkOpenGL())
            {
                supportsOpenGL = false;
                slide->close();
            }

        }
        else
        {
            PresentationGL* const slide = new PresentationGL(m_sharedData);
            slide->show();

            if (!slide->checkOpenGL())
            {
                supportsOpenGL = false;
                slide->close();
            }
        }

        if (!supportsOpenGL)
        {
            QMessageBox::critical(QApplication::activeWindow(), QString(),
                                  i18n("OpenGL support is not available on your system."));
        }
#else
        Q_UNUSED(wantKB);
#endif
    }
*/
}

} // namespace DigikamGenericSlideShowPlugin

