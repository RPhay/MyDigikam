/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2003-03-09
 * Description : Captions, Tags, and Rating properties editor - private container
 *
 * SPDX-FileCopyrightText: 2003-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * SPDX-FileCopyrightText: 2003-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2006-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * SPDX-FileCopyrightText: 2009-2011 by Andi Clemens <andi dot clemens at gmail dot com>
 * SPDX-FileCopyrightText: 2009-2011 by Johannes Wienke <languitar at semipol dot de>
 * SPDX-FileCopyrightText: 2015      by Veaceslav Munteanu <veaceslav dot munteanu90 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "itemdescedittab_p.h"

namespace Digikam
{

ItemDescEditTab::Private::Private(ItemDescEditTab* const tab)
    : modified                    (false),
      ignoreItemAttributesWatch   (false),
      ignoreTagChanges            (false),
      togglingSearchSettings      (false),
      recentTagsBtn               (nullptr),
      assignedTagsBtn             (nullptr),
      revertBtn                   (nullptr),
      openTagMngr                 (nullptr),
      moreMenu                    (nullptr),
      applyBtn                    (nullptr),
      moreButton                  (nullptr),
      applyToAllVersionsButton    (nullptr),
      lastSelectedWidget          (nullptr),
      titleEdit                   (nullptr),
      captionsEdit                (nullptr),
      dateTimeEdit                (nullptr),
      tabWidget                   (nullptr),
      tagsSearchBar               (nullptr),
      newTagEdit                  (nullptr),
      tagCheckView                (nullptr),
      templateSelector            (nullptr),
      templateViewer              (nullptr),
      ratingWidget                (nullptr),
      colorLabelSelector          (nullptr),
      pickLabelSelector           (nullptr),
      tagModel                    (nullptr),
      metadataChangeTimer         (nullptr),
      q                           (tab)
{
}

void ItemDescEditTab::Private::reset()
{
    modified = false;
    hub.resetChanged();
    applyBtn->setEnabled(false);
    revertBtn->setEnabled(false);
    applyToAllVersionsButton->setEnabled(false);
}

void ItemDescEditTab::Private::setInfos(const ItemInfoList& infos)
{
    if (infos.isEmpty())
    {
        hub.reset();
        captionsEdit->blockSignals(true);
        captionsEdit->reset();
        q->resetCaptionEditPlaceholderText();
        captionsEdit->blockSignals(false);
        titleEdit->blockSignals(true);
        titleEdit->reset();
        q->resetTitleEditPlaceholderText();
        titleEdit->blockSignals(false);
        currInfos.clear();
        resetMetadataChangeInfo();
        q->setEnabled(false);

        return;
    }

    q->setEnabled(true);
    currInfos = infos;
    modified  = false;
    resetMetadataChangeInfo();
    hub.reset();
    applyBtn->setEnabled(false);
    revertBtn->setEnabled(false);

    if (currInfos.count() > 1000)
    {
        QApplication::setOverrideCursor(Qt::WaitCursor);
    }

    // First, we read all tags of the items into the cache with one SQL query.
    // This is faster than each item individually.

    currInfos.loadTagIds();

    Q_FOREACH (const ItemInfo& info, currInfos)
    {
        hub.load(info);
    }

    q->updateComments();
    q->updatePickLabel();
    q->updateColorLabel();
    q->updateRating();
    q->updateDate();
    q->updateTemplate();
    q->updateTagsView();
    q->updateRecentTags();
    setFocusToLastSelectedWidget();

    if (currInfos.count() > 1000)
    {
        QApplication::restoreOverrideCursor();
    }
}

void ItemDescEditTab::Private::resetMetadataChangeInfo()
{
    metadataChangeTimer->stop();
    metadataChangeIds.clear();
}

void ItemDescEditTab::Private::setFocusToLastSelectedWidget()
{
    if (lastSelectedWidget)
    {
        lastSelectedWidget->setFocus();
    }

    lastSelectedWidget = nullptr;
}

bool ItemDescEditTab::Private::singleSelection() const
{
    return (currInfos.count() == 1);
}

void ItemDescEditTab::Private::setMetadataWidgetStatus(int status, QWidget* const widget)
{
    if (status == DisjointMetadataDataFields::MetadataDisjoint)
    {
        // For text widgets: Set text color to color of disabled text.

        QPalette palette = widget->palette();
        palette.setColor(QPalette::Text, palette.color(QPalette::Disabled, QPalette::Text));
        widget->setPalette(palette);

        QString msg = i18nc("@info", "Selected items have different text here.");

        if      (qobject_cast<AltLangStrEdit*>(widget))
        {
            titleEdit->setPlaceholderText(msg);
        }
        else if (qobject_cast<CaptionEdit*>(widget))
        {
            captionsEdit->setPlaceholderText(msg);
        }
    }
    else
    {
        widget->setPalette(QPalette());

        if      (qobject_cast<AltLangStrEdit*>(widget))
        {
            q->resetTitleEditPlaceholderText();
        }
        else if (qobject_cast<CaptionEdit*>(widget))
        {
            q->resetCaptionEditPlaceholderText();
        }
    }
}

void ItemDescEditTab::Private::metadataChange(qlonglong imageId)
{
    if (ignoreItemAttributesWatch || modified)
    {
        // Don't lose modifications

        return;
    }

    metadataChangeIds << imageId;
    metadataChangeTimer->start();
}

void ItemDescEditTab::Private::initProgressIndicator()
{
    if (!ProgressManager::instance()->findItembyId(QLatin1String("ItemDescEditTabProgress")))
    {
        FileActionProgress* const item = new FileActionProgress(QLatin1String("ItemDescEditTabProgress"));

        QObject::connect(q, SIGNAL(signalProgressMessageChanged(QString)),
                         item, SLOT(slotProgressStatus(QString)));

        QObject::connect(q, SIGNAL(signalProgressValueChanged(float)),
                         item, SLOT(slotProgressValue(float)));

        QObject::connect(q, SIGNAL(signalProgressFinished()),
                         item, SLOT(slotCompleted()));
    }
}

} // namespace Digikam
