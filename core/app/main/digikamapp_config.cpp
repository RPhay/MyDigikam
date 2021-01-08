/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2002-16-10
 * Description : main digiKam interface implementation - Configure
 *
 * Copyright (C) 2002-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "digikamapp_p.h"

namespace Digikam
{

bool DigikamApp::setup()
{
    return Setup::execDialog(this, Setup::LastPageUsed);
}

bool DigikamApp::setupICC()
{
    return Setup::execSinglePage(this, Setup::ICCPage);
}

void DigikamApp::slotSetup()
{
    setup();
}

void DigikamApp::slotSetupChanged()
{
    // raw loading options might have changed
    LoadingCacheInterface::cleanCache();

    // TODO: clear history when location changed
/*
    if (ApplicationSettings::instance()->getAlbumLibraryPath() != AlbumManager::instance()->getLibraryPath())
    {
        d->view->clearHistory();
    }
*/
    const DbEngineParameters prm = ApplicationSettings::instance()->getDbEngineParameters();

    if (!AlbumManager::instance()->databaseEqual(prm))
    {
        AlbumManager::instance()->changeDatabase(ApplicationSettings::instance()->getDbEngineParameters());
    }

    if (ApplicationSettings::instance()->getShowFolderTreeViewItemsCount())
    {
        AlbumManager::instance()->prepareItemCounts();
    }

    // Load full-screen options

    KConfigGroup group = KSharedConfig::openConfig()->group(configGroupName());
    readFullScreenSettings(group);

    d->view->applySettings();

    AlbumThumbnailLoader::instance()->setThumbnailSize(ApplicationSettings::instance()->getTreeViewIconSize(),
                                                       ApplicationSettings::instance()->getTreeViewFaceSize());

    if (LightTableWindow::lightTableWindowCreated())
    {
        LightTableWindow::lightTableWindow()->applySettings();
    }

    if (QueueMgrWindow::queueManagerWindowCreated())
    {
        QueueMgrWindow::queueManagerWindow()->applySettings();
    }

    d->config->sync();
}

void DigikamApp::slotEditKeys()
{
    editKeyboardShortcuts();
}

void DigikamApp::slotThemeChanged()
{
    ApplicationSettings* const settings = ApplicationSettings::instance();
    QString theme                       = ThemeManager::instance()->currentThemeName();

    if (qApp->activeWindow() && settings->getCurrentTheme() != theme)
    {
        qApp->processEvents();

        QColor color   = qApp->palette().color(qApp->activeWindow()->backgroundRole());
        int colorValue = color.red();
        colorValue    += color.blue();
        colorValue    += color.green();
        colorValue     = colorValue / 3;

        QString iconTheme;
        QString msgText;

        if (colorValue > 127)
        {
            msgText   = i18n("You have chosen a bright color scheme. We switch "
                             "to a dark icon theme. The icon theme is "
                             "available after a restart of digiKam.");

            iconTheme = QLatin1String("breeze");
        }
        else
        {
            msgText   = i18n("You have chosen a dark color scheme. We switch "
                             "to a bright icon theme. The icon theme is "
                             "available after a restart of digiKam.");

            iconTheme = QLatin1String("breeze-dark");
        }

        if (settings->getIconTheme() != iconTheme)
        {
            QMessageBox::information(qApp->activeWindow(),
                                     qApp->applicationName(), msgText);

            settings->setIconTheme(iconTheme);
        }
    }

    settings->setCurrentTheme(theme);
}

} // namespace Digikam
