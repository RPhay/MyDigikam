/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2004-11-22
 * Description : stand alone digiKam image editor GUI
 *
 * Copyright (C) 2004-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2006-2012 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2009-2011 by Andi Clemens <andi dot clemens at gmail dot com>
 * Copyright (C) 2004-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2005-2006 by Tom Albers <tomalbers at kde dot nl>
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

#ifndef SHOW_FOTO_H
#define SHOW_FOTO_H

// Qt includes

#include <QUrl>

// Local includes

#include "editorwindow.h"
#include "showfotoiteminfo.h"

namespace ShowFoto
{

class ShowFoto : public Digikam::EditorWindow
{
    Q_OBJECT

public:

    explicit ShowFoto(const QList<QUrl>& urlList, QWidget* const parent = nullptr);
    ~ShowFoto()                                             override;

public:

    DInfoInterface* infoIface(DPluginAction* const ac)      override;

    virtual void show();

private:

    bool queryClose()                                       override;

    bool save()                                             override;
    bool saveAs()                                           override;
    void moveFile()                                         override;
    void finishSaving(bool success)                         override;
    QUrl saveDestinationUrl()                               override;
    bool saveNewVersion()                                   override;
    bool saveCurrentVersion()                               override;
    bool saveNewVersionAs()                                 override;
    bool saveNewVersionInFormat(const QString&)             override;

    void saveIsComplete()                                   override;
    void saveAsIsComplete()                                 override;
    void saveVersionIsComplete()                            override;

    void openFolder(const QUrl& url);
    void openUrls(const QList<QUrl>& urls);

    Digikam::ThumbBarDock* thumbBar()                 const override;
    Digikam::Sidebar*      rightSideBar()             const override;

private Q_SLOTS:

    void slotForward()                                      override;
    void slotBackward()                                     override;
    void slotLast()                                         override;
    void slotFirst()                                        override;
    void slotFileWithDefaultApplication()                   override;
    void slotOpenWith(QAction* action = nullptr)            override;
    void slotShowfotoItemInfoActivated(const ShowfotoItemInfo& info);

    void slotOpenFile();
    void slotOpenFolder();
    void slotOpenUrl(const ShowfotoItemInfo& info);
    void slotDroppedUrls(const QList<QUrl>& droppedUrls,
                         bool dropped);
    void slotDeleteCurrentItem()                            override;

    void slotChanged()                                      override;
    void slotUpdateItemInfo()                               override;

    void slotPrepareToLoad()                                override;
    void slotLoadingStarted(const QString& filename)        override;
    void slotLoadingFinished(const QString& filename,
                             bool success)                  override;
    void slotSavingStarted(const QString& filename)         override;

    void slotRevert()                                       override;

    void slotAddedDropedItems(QDropEvent*)                  override;

    void slotOnlineVersionCheck()                           override;

Q_SIGNALS:

    void signalLoadCurrentItem(const QList<QUrl>& urlList);
    void signalOpenFolder(const QUrl&);
    void signalOpenFile(const QList<QUrl>& urls);
    void signalInfoList(const ShowfotoItemInfoList&);

// -- Internal setup methods implemented in showfoto_config.cpp ----------------------------------------

public Q_SLOTS:

    void slotSetup()                                        override;
    void slotSetupICC()                                     override;

private:

    bool setup(bool iccSetupPage = false);
    void applySettings();
    void readSettings()                                     override;
    void saveSettings()                                     override;

private Q_SLOTS:

    void slotSetupMetadataFilters(int);

// -- Internal setup methods implemented in showfoto_setup.cpp ----------------------------------------

private:

    void setupActions()                                     override;
    void setupConnections()                                 override;
    void setupUserArea()                                    override;

    void toggleActions(bool val)                            override;
    void toggleNavigation(int index);
    void addServicesMenu()                                  override;

private Q_SLOTS:

    void slotContextMenu()                                  override;

// -- Import tools methods implemented in showfoto_import.cpp -------------------------------------

private Q_SLOTS:

    void slotImportedImagefromScanner(const QUrl& url);

// -- Internal private container --------------------------------------------------------------------

private:

    class Private;
    Private* const d;
};

} // namespace ShowFoto

#endif // SHOW_FOTO_H
