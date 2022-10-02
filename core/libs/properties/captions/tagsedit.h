/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-07-12
 * Description : tags editor
 *
 * SPDX-FileCopyrightText: 2009-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_TAGS_EDIT_H
#define DIGIKAM_TAGS_EDIT_H

// Qt includes

#include <QWidget>
#include <QScrollArea>

// Local includes

#include "digikam_export.h"

namespace Digikam
{

class Album;
class DisjointMetadata;
class SearchTextSettings;
class TaggingAction;
class AddTagsLineEdit;

class DIGIKAM_GUI_EXPORT TagsEdit : public QScrollArea
{
    Q_OBJECT

public:

    explicit TagsEdit(DisjointMetadata* const hub, QWidget* const parent);
    ~TagsEdit()                                      override;

    void updateRecentTags();
    AddTagsLineEdit* getNewTagEdit() const;

Q_SIGNALS:

    void signalModified();

private Q_SLOTS:

    void slotOpenTagsManager();
    void slotTagStateChanged(Album* album, Qt::CheckState checkState);
    void slotTagsSearchChanged(const SearchTextSettings& settings);
    void slotAssignedTagsToggled(bool t);
    void slotTaggingActionActivated(const TaggingAction& action);
    void slotRecentTagsMenuActivated(int id);

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_TAGS_EDIT_H
