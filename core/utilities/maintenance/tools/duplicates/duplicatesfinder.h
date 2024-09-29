/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-01-20
 * Description : Duplicates items finder.
 *
 * SPDX-FileCopyrightText: 2012-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2015      by Mohamed_Anwer <m_dot_anwer at gmx dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Qt includes

#include <QString>
#include <QObject>
#include <QImage>

// Local includes

#include "digikam_gui_export.h"
#include "album.h"
#include "maintenancetool.h"
#include "haariface.h"
#include "iteminfo.h"

namespace Digikam
{

class DIGIKAM_GUI_EXPORT DuplicatesFinder : public MaintenanceTool
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
                     HaarIface::RefImageSelMethod method = HaarIface::RefImageSelMethod::OlderOrLarger,
                     const AlbumList& referenceImageAlbum = {},
                     ProgressItem* const parent = nullptr);

    ~DuplicatesFinder()     override;

private Q_SLOTS:

    void slotStart()        override;
    void slotDone()         override;
    void slotCancel()       override;
    void slotDuplicatesProgress(int percentage, const ItemInfo& inf, const QImage& img);

private:

    class Private;
    Private* const d = nullptr;
};

} // namespace Digikam
