/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-05-19
 * Description : Find Duplicates View.
 *
 * SPDX-FileCopyrightText: 2016-2017 by Mario Frank <mario dot frank at uni minus potsdam dot de>
 * SPDX-FileCopyrightText: 2008-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2008-2012 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * SPDX-FileCopyrightText: 2009      by Andi Clemens <andi dot clemens at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_FIND_DUPLICATES_VIEW_H
#define DIGIKAM_FIND_DUPLICATES_VIEW_H

// Qt includes

#include <QWidget>
#include <QList>

// Local includes

#include "album.h"

class QTreeWidgetItem;

namespace Digikam
{

class FindDuplicatesView : public QWidget
{
    Q_OBJECT

public:

    explicit FindDuplicatesView(QWidget* const parent = nullptr);
    ~FindDuplicatesView() override;

    QList<SAlbum*> currentFindDuplicatesAlbums() const;
    void setActive(bool val);
    void populateTreeView();

public Q_SLOTS:

    void slotSetSelectedAlbums(const QList<PAlbum*>& albums);
    void slotSetSelectedAlbums(const QList<TAlbum*>& albums);
    void slotRemoveDuplicates();

private Q_SLOTS:

    void initAlbumUpdateConnections();
    void slotAlbumAdded(Album* a);
    void slotAlbumDeleted(Album* a);
    void slotSearchUpdated(SAlbum* a);
    void slotSelectItemsTimer();
    /*!
     * \brief slotClear
     * Remove extradata from every album stored in the listview
     * This extra data is used to indicate that the album was already
     * processed
     */
    void slotClear();
    void slotFindDuplicates();
    void slotUpdateDuplicates(const QList<SAlbum*>& sAlbumsToRebuild,
                              const QList<qlonglong>& deletedImages);
    void slotDuplicatesAlbumActived();
    void slotComplete();
    void slotUpdateFingerPrints();
    void slotCheckForValidSettings();
    void slotApplicationSettingsChanged();
    void slotReferenceSelectionMethodChanged(int index);

private:

    void enableControlWidgets(bool);

    void updateSimilarityRangeInterval();

    void resetAlbumsAndTags();

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_FIND_DUPLICATES_VIEW_H
