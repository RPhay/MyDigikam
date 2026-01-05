/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2004-11-22
 * Description : digiKam Survey tool - Extra tools
 *
 * SPDX-FileCopyrightText: 2007-2026 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "surveywindow_p.h"

namespace Digikam
{

void SurveyWindow::slotEditItem()
{
    if (!d->thumbView->currentInfo().isNull())
    {
        slotEditItem(d->thumbView->currentInfo());
    }
}

void SurveyWindow::slotEditItem(const ItemInfo& info)
{
    ImageWindow* const im = ImageWindow::imageWindow();
    ItemInfoList list     = d->thumbView->allItemInfos();

    im->loadItemInfos(list, info, i18n("Light Table"));

    if (im->isHidden())
    {
        im->show();
    }
    else
    {
        im->raise();
    }

    im->setFocus();
}

// FIXME
void SurveyWindow::slotLeftSlideShowManualFromCurrent()
{
    QList<DPluginAction*> actions = DPluginLoader::instance()->
                                        pluginActions(QLatin1String("org.kde.digikam.plugin.generic.SlideShow"), this);

    if (actions.isEmpty())
    {
        return;
    }

    // set current image to SlideShow Plugin

    actions[0]->setData(d->previewView->leftItemInfo().fileUrl());

    actions[0]->trigger();

    d->fromLeftPreview = true;
}

void SurveyWindow::slotRightSlideShowManualFromCurrent()
{
    QList<DPluginAction*> actions = DPluginLoader::instance()->
                                       pluginActions(QLatin1String("org.kde.digikam.plugin.generic.SlideShow"), this);

    if (actions.isEmpty())
    {
        return;
    }

    // set current image to SlideShow Plugin

    actions[0]->setData(d->previewView->rightItemInfo().fileUrl());
    actions[0]->trigger();

    d->fromLeftPreview = false;
}

void SurveyWindow::slotSlideShowLastItemUrl()
{
    QList<DPluginAction*> actions = DPluginLoader::instance()->
                                       pluginActions(QLatin1String("org.kde.digikam.plugin.generic.SlideShow"), this);

    if (actions.isEmpty())
    {
        return;
    }

    // get last image to SlideShow Plugin

    QUrl url = actions[0]->data().toUrl();

    if (d->fromLeftPreview && !d->navigateByPairAction->isChecked())
    {
        d->thumbView->blockSignals(true);
        d->thumbView->setCurrentUrl(url);
        d->thumbView->blockSignals(false);
        slotSetItemLeft();
    }
    else
    {
        d->thumbView->setCurrentUrl(url);
    }
}

} // namespace Digikam
