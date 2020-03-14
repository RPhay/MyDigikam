/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2018-07-30
 * Description : a plugin to render slideshow.
 *
 * Copyright (C) 2018-2020 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2019-2020 by Minh Nghia Duong <minhnghiaduong997 at gmail dot com>
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
#include <QString>
#include <QApplication>

// KDE includes

#include <klocalizedstring.h>

// Local includes
#include "metaenginesettings.h"

#include "setupslideshow_dialog.h"
#include "slideshowloader.h"

using namespace Digikam;

namespace DigikamGenericSlideShowPlugin
{

SlideShowPlugin::SlideShowPlugin(QObject* const parent)
    : DPluginGeneric(parent),
      m_iface(nullptr)
{
}

SlideShowPlugin::~SlideShowPlugin()
{
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
            << DPluginAuthor(QString::fromUtf8("Enrico Ros"),
                             QString::fromUtf8("eros dot kde at email dot it"),
                             QString::fromUtf8("(C) 2004"))
            << DPluginAuthor(QString::fromUtf8("Renchi Raju"),
                             QString::fromUtf8("renchi dot raju at gmail dot com"),
                             QString::fromUtf8("(C) 2004-2005"))
            << DPluginAuthor(QString::fromUtf8("Gilles Caulier"),
                             QString::fromUtf8("caulier dot gilles at gmail dot com"),
                             QString::fromUtf8("(C) 2005-2020"))
            << DPluginAuthor(QString::fromUtf8("Minh Nghia Duong"),
                             QString::fromUtf8("minhnghiaduong997 at gmail dot com"),
                             QString::fromUtf8("(C) 2019-2020"))
            ;
}

void SlideShowPlugin::setup(QObject* const parent)
{
    DPluginAction* const ac = new DPluginAction(parent);
    ac->setIcon(icon());
    ac->setText(i18nc("@action", "Slideshow"));
    ac->setObjectName(QLatin1String("slideshow_plugin"));
    ac->setActionCategory(DPluginAction::GenericView);

    m_iface = infoIface(ac);

    if (m_iface && parent->objectName() == QLatin1String("Digikam"))
    {
        QMenu* const slideShowActions = new QMenu(i18n("Slideshow"), nullptr);
        slideShowActions->setIcon(icon());
        ac->setMenu(slideShowActions);

        // Action show all

        QAction* const slideShowAllAction = new QAction(i18n("All"), parent);
        slideShowAllAction->setObjectName(QLatin1String("slideshow_all"));
        slideShowAllAction->setShortcut(Qt::Key_F9);
        slideShowActions->addAction(slideShowAllAction);

        connect(slideShowAllAction, SIGNAL(triggered()),
                this, SLOT(slotMenuSlideShowAll()));

        // Action show selection

        QAction* const slideShowSelectionAction = new QAction(i18n("Selection"), parent);
        slideShowSelectionAction->setObjectName(QLatin1String("slideshow_selected"));
        slideShowSelectionAction->setShortcut(Qt::ALT + Qt::Key_F9);
        slideShowActions->addAction(slideShowSelectionAction);

        connect(slideShowSelectionAction, SIGNAL(triggered()),
                this, SLOT(slotMenuSlideShowSelection()));

        // Action show recursive

        QAction* const slideShowRecursiveAction = new QAction(i18n("With All Sub-Albums"), parent);
        slideShowRecursiveAction->setObjectName(QLatin1String("slideshow_recursive"));
        slideShowRecursiveAction->setShortcut(Qt::SHIFT + Qt::Key_F9);
        slideShowActions->addAction(slideShowRecursiveAction);

        connect(slideShowRecursiveAction, SIGNAL(triggered()),
                this, SLOT(slotMenuSlideShowRecursive()));

        connect(ac, SIGNAL(triggered(bool)),
                this, SLOT(slotShowManual()));
    }
    else
    {
        ac->setShortcut(Qt::Key_F9);

        connect(ac, SIGNAL(triggered(bool)),
                this, SLOT(slotMenuSlideShow()));
    }

    addAction(ac);
}

void SlideShowPlugin::addConnectionSlideEnd(QObject* obj)
{
}

void SlideShowPlugin::slotMenuSlideShow()
{
    DPluginAction* const ac = dynamic_cast<DPluginAction*>(sender());

    QUrl startFrom;
    if (ac)
    {
        startFrom = ac->data().toUrl();

        ac->setData(QVariant());
    }

    m_iface                           = infoIface(sender());
    SlideShowSettings* const settings = new SlideShowSettings();

    settings->readFromConfig();

    settings->exifRotate = MetaEngineSettings::instance()->settings().exifRotate;
    settings->fileList   = m_iface->currentAlbumItems();

    slideshow(settings, true, startFrom);
}

void SlideShowPlugin::slotMenuSlideShowAll()
{
    SlideShowSettings* const settings = new SlideShowSettings();

    settings->readFromConfig();

    settings->fileList                = m_iface->currentAlbumItems();

    slideshow(settings);
}

void SlideShowPlugin::slotMenuSlideShowSelection()
{
    SlideShowSettings* const settings = new SlideShowSettings();

    settings->readFromConfig();

    settings->fileList                = m_iface->currentSelectedItems();

    slideshow(settings);
}

void SlideShowPlugin::slotMenuSlideShowRecursive()
{
    connect(m_iface, SIGNAL(signalAlbumItemsRecursiveCompleted(const QList<QUrl>&)),
            this, SLOT(slotShowRecursive(const QList<QUrl>&)));

    m_iface->parseAlbumItemsRecursive();
}

void SlideShowPlugin::slotShowRecursive(const QList<QUrl>& imageList)
{
    SlideShowSettings* const settings = new SlideShowSettings();

    settings->readFromConfig();

    settings->fileList                = imageList;

    slideshow(settings);
}

void SlideShowPlugin::slotShowManual()
{
    DPluginAction* const ac = dynamic_cast<DPluginAction*>(sender());

    QUrl startFrom;

    if (ac)
    {
        startFrom = ac->data().toUrl();

        ac->setData(QVariant());

        if (!startFrom.isValid())
        {
            return;
        }
    }
    else
    {
        return;
    }

    SlideShowSettings* const settings = new SlideShowSettings();

    settings->readFromConfig();

    settings->exifRotate              = MetaEngineSettings::instance()->settings().exifRotate;
    settings->fileList                = m_iface->currentAlbumItems();

    slideshow(settings, true, startFrom);
}

void SlideShowPlugin::slotSlideShowFinished(const QUrl& lastImage)
{
/*
 *  TODO: pass Url of last image out to main Apps
    QList<DPluginAction*> actionList = actions(parent());

    qDebug() << "SlideShowPlugin::slotSlideShowFinished actionList" << actionList.size();

    if(!actionList.isEmpty())
    {
        actionList[0]->setData(lastImage);

        actionList[0]->changed();
    }
*/
}

void SlideShowPlugin::slideshow(SlideShowSettings* settings, bool autoPlayEnabled, const QUrl& startFrom)
{
    SlideShowLoader* const slide = new SlideShowLoader(m_iface, settings);

    slide->setShortCutPrefixes(m_iface->passShortcutActionsToWidget(slide));

    settings->autoPlayEnabled    = autoPlayEnabled;

    //TODO: preview settings for digikam
    //settings.previewSettings = ApplicationSettings::instance()->getPreviewSettings();

    if (startFrom.isValid())
    {
        settings->imageUrl = startFrom;
    }

    if      (settings->imageUrl.isValid())
    {
        slide->setCurrentItem(settings->imageUrl);
    }
    else if (settings->startWithCurrent)
    {
        slide->setCurrentItem(m_iface->currentSelectedItems()[0]);
    }

    connect(slide, SIGNAL(signalLastItemUrl(QUrl)),
            this, SLOT(slotSlideShowFinished(QUrl)));

    connect(m_iface, SIGNAL(signalShortcutPressed(QString,int)),
            slide, SLOT(slotHandleShortcut(QString,int)));

    slide->show();
}

} // namespace DigikamGenericSlideShowPlugin
