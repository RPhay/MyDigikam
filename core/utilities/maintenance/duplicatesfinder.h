/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-01-20
 * Description : Duplicates items finder.
 *
 * Copyright (C) 2012-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2015      by Mohamed_Anwer <m_dot_anwer at gmx dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_DUPLICATES_FINDER_H
#define DIGIKAM_DUPLICATES_FINDER_H

// Qt includes

#include <QString>
#include <QObject>

// Local includes

#include "album.h"
#include "maintenancetool.h"

namespace Digikam
{

class DuplicatesFinder : public MaintenanceTool
{
    Q_OBJECT

public:

    /** Version to find all duplicates over a specific list to PAlbums and TAlbums
     */
    DuplicatesFinder(const AlbumList& albums,
                     const AlbumList& tags,
                     int albumTagRelation = 0,
                     int minSimilarity = 90,
                     int maxSimilarity = 100,
                     int searchResultRestriction = 0,
                     ProgressItem* const parent = nullptr);

    ~DuplicatesFinder() override;

private Q_SLOTS:

    void slotStart() override;
    void slotDone() override;
    void slotCancel() override;
    void slotDuplicatesProgress(int percentage);

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_DUPLICATES_FINDER_H
