/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2004-11-22
 * Description : digiKam Survey tool - Configure
 *
 * SPDX-FileCopyrightText: 2007-2026 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "surveywindow_p.h"

namespace Digikam
{

void SurveyWindow::slotSetup()
{
    Setup::execDialog(this);
}

void SurveyWindow::slotColorManagementOptionsChanged()
{
    ICCSettingsContainer settings = IccSettings::instance()->settings();

    d->viewCMViewAction->blockSignals(true);
    d->viewCMViewAction->setEnabled(settings.enableCM);
    d->viewCMViewAction->setChecked(settings.useManagedPreviews);
    d->viewCMViewAction->blockSignals(false);
}

void SurveyWindow::slotThemeChanged()
{
}

void SurveyWindow::slotApplicationSettingsChanged()
{
    d->sideBar->setStyle(ApplicationSettings::instance()->getSidebarTitleStyle());

    /// @todo Which part of the settings has to be reloaded?
    //d->sideBar->applySettings();

    d->previewView->setPreviewSettings(ApplicationSettings::instance()->getPreviewSettings());
}

void SurveyWindow::readSettings()
{
    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group        = config->group(configGroupName());

    d->hSplitter->restoreState(group, QLatin1String("Horizontal Splitter State"));
    d->barViewDock->setShouldBeVisible(group.readEntry(QLatin1String("Show Thumbbar"), true));

    QByteArray thumbbarState;
    thumbbarState = group.readEntry(QLatin1String("ThumbbarState"), thumbbarState);
    d->dockArea->restoreState(QByteArray::fromBase64(thumbbarState));

    d->sideBar->setConfigGroup(KConfigGroup(&group, QLatin1String("Sidebar")));
    d->sideBar->loadState();

    readFullScreenSettings(group);
}

void SurveyWindow::writeSettings()
{
    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group        = config->group(configGroupName());
    d->hSplitter->saveState(group, QLatin1String("Horizontal Splitter State"));
    group.writeEntry(QLatin1String("Show Thumbbar"),    d->barViewDock->shouldBeVisible());
    group.writeEntry(QLatin1String("ThumbbarState"),    d->dockArea->saveState().toBase64());
    group.writeEntry(QLatin1String("Clear On Close"),   d->clearOnCloseAction->isChecked());

    d->sideBar->setConfigGroup(KConfigGroup(&group, QLatin1String("Sidebar")));
    d->sideBar->saveState();

    config->sync();
}

void SurveyWindow::applySettings()
{
    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group        = config->group(configGroupName());
    d->clearOnCloseAction->setChecked(group.readEntry(QLatin1String("Clear On Close"), false));
    slotApplicationSettingsChanged();

    // Restore full screen Mode

    readFullScreenSettings(group);

    // NOTE: Image orientation settings in thumbbar is managed by image model.

    refreshView();
}

} // namespace Digikam
