/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2018-07-30
 * Description : a plugin to render slideshow.
 *
 * Copyright (C) 2018-2020 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2020      by Minh Nghia Duong <minhnghiaduong997 at gmail dot com>
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

#include "slideshowplugin.h"

// Qt includes

#include <QPointer>
#include <QMenu>
#include <QDebug>

// KDE includes

#include <klocalizedstring.h>

// Local includes

//#include "slideshow.h"
#include "slideshowmngr.h"

namespace DigikamGenericSlideShowPlugin
{

SlideShowPlugin::SlideShowPlugin(QObject* const parent)
    : DPluginGeneric(parent),
      m_slideshowMngr(nullptr)
{
}

SlideShowPlugin::~SlideShowPlugin()
{
}

void SlideShowPlugin::cleanUp()
{
    delete m_slideshowMngr;
}

QString SlideShowPlugin::name() const
{
    return i18n("SlideShow");
}

QString SlideShowPlugin::iid() const
{
    return QLatin1String(DPLUGIN_IID);
}

QIcon SlideShowPlugin::icon() const
{
    return QIcon::fromTheme(QLatin1String("view-presentation"));
}

QString SlideShowPlugin::description() const
{
    return i18n("A tool to render slideshow");
}

QString SlideShowPlugin::details() const
{
    return i18n("<p>This tool render a series of items as a simple slide-show.</p>"
                "<p>Plenty of items properties can be displayed as overlay while running.</p>"
                "<p>This tool can play album contents in recursive mode with children albums if any.</p>");
}

QList<DPluginAuthor> SlideShowPlugin::authors() const
{
    return QList<DPluginAuthor>()
            << DPluginAuthor(QString::fromUtf8("Enrico Ros <eros dot kde at email dot it>"),
                             QString::fromUtf8("eros dot kde at email dot it"),
                             QString::fromUtf8("(C) 2004"))
            << DPluginAuthor(QString::fromUtf8("Renchi Raju"),
                             QString::fromUtf8("renchi dot raju at gmail dot com"),
                             QString::fromUtf8("(C) 2004-2005"))
            << DPluginAuthor(QString::fromUtf8("Gilles Caulier"),
                             QString::fromUtf8("caulier dot gilles at gmail dot com"),
                             QString::fromUtf8("(C) 2005-2020"))
            ;
}

void SlideShowPlugin::setup(QObject* const parent)
{
    DPluginAction* const ac = new DPluginAction(parent);
    ac->setIcon(icon());
    ac->setText(i18nc("@action", "Slideshow Plugin"));
    //TODO: rechange all name 'slideshow plugin' to 'slideshow'
    ac->setObjectName(QLatin1String("slideshow_plugin"));
    ac->setActionCategory(DPluginAction::GenericView);
    //c->setShortcut(Qt::ALT + Qt::SHIFT + Qt::Key_F9);


    DInfoInterface* const iface = infoIface(parent);

    if (iface == nullptr)
    {
        qDebug() << "SlideShowPlugin::iface is null";
    }

    //supportAlbums return false ??? and iface is null ???
    if (1 /*iface && iface->supportAlbums()*/)
    {
        QMenu* const slideShowActions = new QMenu(i18n("Slideshow_Plugin"), nullptr);
        slideShowActions->setIcon(icon());
        ac->setMenu(slideShowActions);

        //Action show all
        QAction* const slideShowAllAction = new QAction(i18n("All"), parent);
        slideShowAllAction->setObjectName(QLatin1String("slideshow_all"));
        slideShowAllAction->setShortcut(Qt::Key_A);                 //TODO: rechange shortcut to F9
        slideShowActions->addAction(slideShowAllAction);

        connect(slideShowAllAction, SIGNAL(triggered()),
                this, SLOT(slotMenuSlideShowAll()));

        //Action show selection
        QAction* const slideShowSelectionAction = new QAction(i18n("Selection"), parent);
        slideShowSelectionAction->setObjectName(QLatin1String("slideshow_selected"));
        slideShowSelectionAction->setShortcut(Qt::ALT + Qt::Key_A);     //TODO: rechange shortcut to ALT+F9
        slideShowActions->addAction(slideShowSelectionAction);

        connect(slideShowSelectionAction, SIGNAL(triggered()),
                this, SLOT(slotMenuSlideShowSelection()));

        //Action show recursive
        QAction* const slideShowRecursiveAction = new QAction(i18n("With All Sub-Albums"), parent);
        slideShowRecursiveAction->setObjectName(QLatin1String("slideshow_recursive"));
        slideShowRecursiveAction->setShortcut(Qt::SHIFT + Qt::Key_A);    //TODO: rechange shortcut to SHIFT+F9
        slideShowActions->addAction(slideShowRecursiveAction);

        connect(slideShowRecursiveAction, SIGNAL(triggered()),
                this, SLOT(slotMenuSlideShowRecursive()));

        //Action configuration
        QAction* const configurationAction = new QAction(i18n("Configurations"), parent);
        configurationAction->setObjectName(QLatin1String("slideshow_configation"));
        configurationAction->setShortcut(Qt::CTRL + Qt::Key_A);    //TODO: rechange shortcut to CTRL+F9
        slideShowActions->addAction(configurationAction);

        connect(configurationAction, SIGNAL(triggered()),
                this, SLOT(slotMenuSlideShowConfiguration()));

    }
    else
    {
        //TODO change shortcut to F9
        ac->setShortcut(Qt::Key_A);

        connect(ac, SIGNAL(triggered(bool)),
                this, SLOT(slotSlideShow()));
    }

    addAction(ac);
}

void SlideShowPlugin::slotSlideShow()
{
    DInfoInterface* const iface = infoIface(sender());
    qDebug() << "SlideshowPlugin::slotSlideShow";

    m_slideshowMngr = new SlideShowMngr(this, iface);

    m_slideshowMngr->setPlugin(this);

    m_slideshowMngr->slotSlideShow();
}

//TODO: add slotMenuSlideShowAll
void SlideShowPlugin::slotMenuSlideShowAll()
{

    DInfoInterface* const iface = infoIface(sender());

    m_slideshowMngr = new SlideShowMngr(this, iface);

    m_slideshowMngr->setPlugin(this);

    m_slideshowMngr->slotSlideShow();

    qDebug() << "SlideshowPlugin::slotMenuSlideShowAll";
}

//TODO: add slotMenuSlideShowSelection
void SlideShowPlugin::slotMenuSlideShowSelection()
{
    qDebug() << "SlideshowPlugin::slotMenuSlideShowSelection";
}

//TODO: add slotMenuSlideShowRecursive
void SlideShowPlugin::slotMenuSlideShowRecursive()
{
    qDebug() << "SlideshowPlugin::slotMenuSlideShowRecursive";
}

//TODO: show configuration dialog
void SlideShowPlugin::slotMenuSlideShowConfiguration()
{
    qDebug() << "SlideshowPlugin::slotMenuSlideShowConfiguration";

    DInfoInterface* const iface = infoIface(sender());

    if (iface == nullptr)
    {
        //TODO: resolve the problem iface is null
        qDebug() << "SlideShowPlugin::slotMenuSlideShowConfiguration() : iface is null";
    }

    if (m_slideshowMngr)
    {
        delete m_slideshowMngr;
    }

    m_slideshowMngr = new SlideShowMngr(this, iface);

    m_slideshowMngr->setPlugin(this);

    m_slideshowMngr->showConfigDialog();

}

} // namespace DigikamGenericSlideShowPlugin
