/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2003-03-09
 * Description : Captions, Tags, and Rating properties editor
 *
 * SPDX-FileCopyrightText: 2003-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * SPDX-FileCopyrightText: 2003-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2006-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_ITEM_DESC_EDIT_TAB_H
#define DIGIKAM_ITEM_DESC_EDIT_TAB_H

// Qt includes

#include <QScrollArea>
#include <QPixmap>
#include <QEvent>

// Local includes

#include "dlayoutbox.h"
#include "digikam_export.h"
#include "iteminfolist.h"
#include "albummanager.h"
#include "albummodel.h"
#include "metadatahub.h"
#include "searchtextbar.h"
#include "addtagslineedit.h"
#include "disjointmetadata.h"

namespace Digikam
{
class ItemInfo;
class TaggingAction;

class ItemDescEditTab : public DVBox
{
    Q_OBJECT

public:

    explicit ItemDescEditTab(QWidget* const parent);
    ~ItemDescEditTab()                      override;

    void assignPickLabel(int pickId);
    void assignColorLabel(int colorId);
    void assignRating(int rating);
    void setItem(const ItemInfo& info = ItemInfo());
    void setItems(const ItemInfoList& infos);
    void setFocusToTitlesEdit();
    void setFocusToCommentsEdit();

    void readSettings(KConfigGroup& group);
    void writeSettings(KConfigGroup& group);

Q_SIGNALS:

    void signalProgressMessageChanged(const QString& actionDescription);
    void signalProgressValueChanged(float percent);
    void signalProgressFinished();

    void signalPrevItem();
    void signalNextItem();

    void signalAskToApplyChanges(const QList<ItemInfo>& infos, DisjointMetadata* hub);

protected:

    bool eventFilter(QObject* o, QEvent* e) override;

private:

    void reset();

    void setInfos(const ItemInfoList& infos);
    void setFocusToLastSelectedWidget();

    void updateComments();
    void updatePickLabel();
    void updateColorLabel();
    void updateRating();
    void updateDate();
    void updateTemplate();

    bool singleSelection() const;
    void setMetadataWidgetStatus(int status, QWidget* const widget);
    void metadataChange(qlonglong imageId);
    void resetMetadataChangeInfo();
    void initProgressIndicator();

    void resetTitleEditPlaceholderText();
    void resetCaptionEditPlaceholderText();

private Q_SLOTS:

    void slotApplyAllChanges();
    void slotApplyChangesToAllVersions();
    void slotRevertAllChanges();
    void slotChangingItems();
    void slotCommentChanged();
    void slotTitleChanged();
    void slotDateTimeChanged(const QDateTime& dateTime);
    void slotPickLabelChanged(int pickId);
    void slotColorLabelChanged(int colorId);
    void slotRatingChanged(int rating);
    void slotTemplateSelected();
    void slotModified();
    void slotReloadForMetadataChange();

    void slotImagesChanged(int albumId);
    void slotImageRatingChanged(qlonglong imageId);
    void slotImageDateChanged(qlonglong imageId);
    void slotImageCaptionChanged(qlonglong imageId);

    void slotMoreMenu();
    void slotReadFromFileMetadataToDatabase();
    void slotWriteToFileMetadataFromDatabase();

    void slotAskToApplyChanges(const QList<ItemInfo>& infos, DisjointMetadata* hub);

    ///@{
    /// Tags management methods (itemdescedittab_tags.cpp)

public:

    void populateTags();
    void setFocusToTagsView();
    void setFocusToNewTagEdit();
    void activateAssignedTagsButton();

    AddTagsLineEdit* getNewTagEdit() const;

Q_SIGNALS:

    void signalTagFilterMatch(bool);

private:

    void setTagState(TAlbum* const tag, DisjointMetadataDataFields::Status status);
    void updateTagsView();
    void updateRecentTags();

private Q_SLOTS:

    void slotTagsSearchChanged(const SearchTextSettings& settings);
    void slotTagStateChanged(Album* album, Qt::CheckState checkState);
    void slotTaggingActionActivated(const TaggingAction&);
    void slotImageTagsChanged(qlonglong imageId);
    void slotOpenTagsManager();

    void slotRecentTagsMenuActivated(int);
    void slotAssignedTagsToggled(bool);
    void slotUnifyPartiallyTags();

    ///@}

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_ITEM_DESC_EDIT_TAB_H
