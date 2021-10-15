/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2004-11-22
 * Description : stand alone digiKam image editor
 *
 * Copyright (C) 2004-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2006-2012 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2009-2011 by Andi Clemens <andi dot clemens at gmail dot com>
 * Copyright (C) 2004-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2005-2006 by Tom Albers <tomalbers at kde dot nl>
 * Copyright (C) 2008      by Arnd Baecker <arnd dot baecker at web dot de>
 * Copyright (C) 2013-2015 by Mohamed_Anwer <m_dot_anwer at gmx dot com>
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

#include "showfoto_p.h"

namespace ShowFoto
{

Showfoto::Showfoto(const QList<QUrl>& urlList, QWidget* const)
    : Digikam::EditorWindow(QLatin1String("Showfoto")),
      d                    (new Private)
{
    setXMLFile(QLatin1String("showfotoui5.rc"));

    m_nonDestructive = false;

    // Show splash-screen at start up.

    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group        = config->group(configGroupName());

    if (group.readEntry(QLatin1String("ShowSplash"), true) && !qApp->isSessionRestored())
    {
        d->splash = new Digikam::DSplashScreen();
        d->splash->show();
    }

    // Setup loading cache and thumbnails interface.

    Digikam::LoadingCacheInterface::initialize();
    Digikam::MetaEngineSettings::instance();

    d->thumbLoadThread = new Digikam::ThumbnailLoadThread();
    d->thumbLoadThread->setThumbnailSize(Digikam::ThumbnailSize::Huge);
    d->thumbLoadThread->setSendSurrogatePixmap(true);

    // Check ICC profiles repository availability

    if (d->splash)
    {
        d->splash->setMessage(i18n("Checking ICC repository..."));
    }

    d->validIccPath = Digikam::SetupICC::iccRepositoryIsValid();

    // Populate Themes

    if (d->splash)
    {
        d->splash->setMessage(i18n("Loading themes..."));
    }

    Digikam::ThemeManager::instance();

    // Load plugins

    if (d->splash)
    {
        d->splash->setMessage(i18n("Load Plugins..."));
    }

    DPluginLoader* const dpl = Digikam::DPluginLoader::instance();
    dpl->init();

    // -- Build the GUI -----------------------------------

    setupUserArea();
    setupActions();
    setupStatusBar();
    createGUI(xmlFile());
    registerPluginsActions();
    d->folderView->registerPluginActions(dpl->pluginsActions(DPluginAction::GenericView, this));
    d->stackView->registerPluginActions(dpl->pluginsActions(DPluginAction::GenericView, this));

    cleanupActions();

    // Create tool selection view

    setupSelectToolsAction();

    // Create context menu.

    setupContextMenu();

    // Make signals/slots connections

    setupConnections();

    // Disable all actions

    toggleActions(false);

    // -- Read settings --------------------------------

    readSettings();
    applySettings();
    setAutoSaveSettings(configGroupName(), true);

    d->leftSideBar->loadState();
    d->folderView->loadState();
    d->stackView->loadState();
    d->rightSideBar->loadState();

    //--------------------------------------------------

    d->thumbBarDock->reInitialize();

    // -- Load current items ---------------------------

    slotDroppedUrls(urlList, false, QUrl());
}

Showfoto::~Showfoto()
{
    delete m_canvas;
    m_canvas = nullptr;

    Digikam::ThumbnailLoadThread::cleanUp();
    Digikam::LoadingCacheInterface::cleanUp();
    Digikam::DPluginLoader::instance()->cleanUp();

    delete d->model;
    delete d->filterModel;
    delete d->thumbBar;
    delete d->rightSideBar;
    delete d->thumbLoadThread;
    delete d;
}

bool Showfoto::queryClose()
{
    // wait if a save operation is currently running

    if (!waitForSavingToComplete())
    {
        return false;
    }

    if (!d->thumbBar->currentInfo().isNull() && !promptUserSave(d->thumbBar->currentUrl()))
    {
        return false;
    }

    saveSettings();

    return true;
}

void Showfoto::show()
{
    // Remove Splashscreen.

    if (d->splash)
    {
        d->splash->finish(this);
        delete d->splash;
        d->splash = nullptr;
    }

    // Display application window.

    QMainWindow::show();

    // Report errors from ICC repository path.

    KSharedConfig::Ptr config = KSharedConfig::openConfig();

    if (!d->validIccPath)
    {
        QString message = i18n("<p>The ICC profile path seems to be invalid.</p>"
                               "<p>If you want to set it now, select \"Yes\", otherwise "
                               "select \"No\". In this case, \"Color Management\" feature "
                               "will be disabled until you solve this issue</p>");

        if (QMessageBox::warning(this, qApp->applicationName(), message,
                                 QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
        {
            if (!setup(true))
            {
                KConfigGroup group = config->group(QLatin1String("Color Management"));
                group.writeEntry(QLatin1String("EnableCM"), false);
                config->sync();
            }
        }
        else
        {
            KConfigGroup group = config->group(QLatin1String("Color Management"));
            group.writeEntry(QLatin1String("EnableCM"), false);
            config->sync();
        }
    }
}

void Showfoto::slotClearThumbBar()
{
    d->infoList.clear();
    d->model->clearShowfotoItemInfos();
}

void Showfoto::slotRemoveItemInfos(const QList<ShowfotoItemInfo>& infos)
{
    foreach (const ShowfotoItemInfo& inf, infos)
    {
        d->infoList.removeAll(inf);
    }

    d->model->removeShowfotoItemInfos(infos);
}

void Showfoto::slotShowfotoItemInfoActivated(const ShowfotoItemInfo& info)
{
    if (!d->thumbBar->currentInfo().isNull() && !promptUserSave(d->currentLoadedUrl))
    {
        d->thumbBar->setCurrentUrl(d->currentLoadedUrl);

        return;
    }

    slotOpenUrl(info);
}

Digikam::ThumbBarDock* Showfoto::thumbBar() const
{
    return d->thumbBarDock;
}

Digikam::Sidebar* Showfoto::rightSideBar() const
{
    return (dynamic_cast<Digikam::Sidebar*>(d->rightSideBar));
}

void Showfoto::slotChanged()
{
    QString mpixels;
    QSize dims(m_canvas->imageWidth(), m_canvas->imageHeight());
    mpixels = QLocale().toString(dims.width()*dims.height()/1000000.0, 'f', 1);
    QString str = (!dims.isValid()) ? i18nc("unknown image dimensions", "Unknown")
                                    : i18nc("%1 width, %2 height, %3 mpixels", "%1x%2 (%3Mpx)",
                                            dims.width(),dims.height(),mpixels);
    m_resLabel->setAdjustedText(str);

    if (!d->thumbBar->currentInfo().isNull())
    {
        if (d->thumbBar->currentUrl().isValid())
        {
            QRect sel                = m_canvas->getSelectedArea();
            Digikam::DImg* const img = m_canvas->interface()->getImg();
            d->rightSideBar->itemChanged(d->thumbBar->currentUrl(), sel, img);
        }
    }
}

void Showfoto::slotUpdateItemInfo()
{
    d->itemsNb = d->thumbBar->showfotoItemInfos().size();
    int index  = d->thumbBar->thumbnailIndexForUrl(d->thumbBar->currentUrl());
    QString text;

    if (d->itemsNb > 0)
    {
        text = i18nc("<Image file name> (<Image number> of <Images in album>)",
                     "%1 (%2 of %3)", d->thumbBar->currentInfo().name,
                     index, d->itemsNb);

        setCaption(QDir::toNativeSeparators(d->thumbBar->currentUrl()
                                            .adjusted(QUrl::RemoveFilename).toLocalFile()));
    }
    else
    {
        text = QLatin1String("");
        setCaption(QLatin1String(""));
    }

    m_nameLabel->setText(text);
    toggleNavigation(index);
}

void Showfoto::slotFirst()
{
    if (!d->thumbBar->currentInfo().isNull() && !promptUserSave(d->thumbBar->currentUrl()))
    {
        return;
    }

    d->thumbBar->toFirstIndex();
    d->thumbBar->setCurrentInfo(d->thumbBar->showfotoItemInfos().first());
    slotOpenUrl(d->thumbBar->showfotoItemInfos().first());
}

void Showfoto::slotLast()
{
    if (!d->thumbBar->currentInfo().isNull() && !promptUserSave(d->thumbBar->currentUrl()))
    {
        return;
    }

    d->thumbBar->toLastIndex();
    d->thumbBar->setCurrentInfo(d->thumbBar->showfotoItemInfos().last());
    slotOpenUrl(d->thumbBar->showfotoItemInfos().last());
}

void Showfoto::slotForward()
{
    if (!d->thumbBar->currentInfo().isNull() && !promptUserSave(d->thumbBar->currentUrl()))
    {
        return;
    }

    bool currentIsNull = d->thumbBar->currentInfo().isNull();

    if (!currentIsNull)
    {
         d->thumbBar->toNextIndex();
         slotOpenUrl(d->thumbBar->currentInfo());
    }
}

void Showfoto::slotBackward()
{
    if (!d->thumbBar->currentInfo().isNull() && !promptUserSave(d->thumbBar->currentUrl()))
    {
        return;
    }

    bool currentIsNull = d->thumbBar->currentInfo().isNull();

    if (!currentIsNull)
    {
         d->thumbBar->toPreviousIndex();
         slotOpenUrl(d->thumbBar->currentInfo());
    }
}

DInfoInterface* Showfoto::infoIface(DPluginAction* const)
{
    ShowfotoInfoIface* const iface = new ShowfotoInfoIface(this, d->thumbBar->urls());

    qCDebug(DIGIKAM_GENERAL_LOG) << "Showfoto::infoIface: nb of file" << d->thumbBar->urls().size();

    connect(iface, SIGNAL(signalItemChanged(QUrl)),
            this, SLOT(slotChanged()));

    connect(iface, SIGNAL(signalImportedImage(QUrl)),
            this, SLOT(slotImportedImagefromScanner(QUrl)));

    connect(iface, SIGNAL(signalRemoveImageFromAlbum(QUrl)),
            this, SLOT(slotRemoveImageFromAlbum(QUrl)));

    return iface;
}

void Showfoto::slotOnlineVersionCheck()
{
    ShowfotoSetup::onlineVersionCheck();
}

} // namespace ShowFoto

#include "moc_showfoto.cpp"
