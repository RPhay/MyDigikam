/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2023-21-06
 * Description : Batch remover of metadata.
 *
 * SPDX-FileCopyrightText: 2013-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2023      by Maik Qualmann <metzpinguin at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Qt includes

#include <QObject>

// Local includes

#include "album.h"
#include "iteminfo.h"
#include "maintenancetool.h"

namespace Digikam
{

class MetadataRemover : public MaintenanceTool
{
    Q_OBJECT

public:

    enum RemoveAction
    {
        None = 0,
        Faces,
        Tags
    };

public:

    /**
     * Constructor which remove all images metadata from an Albums list.
     * If list is empty, whole Albums collection is processed.
     */
    explicit MetadataRemover(const AlbumList& list = AlbumList(),
                             RemoveAction action = None,
                             ProgressItem* const parent = nullptr);

    /**
     * Constructor which remove all images metadata from an Images list
     */
    explicit MetadataRemover(const ItemInfoList& list,
                             RemoveAction action = None,
                             ProgressItem* const parent = nullptr);


    ~MetadataRemover()              override;

    void setUseMultiCoreCPU(bool b) override;

private Q_SLOTS:

    void slotStart()                override;
    void slotParseAlbums();
    void slotAlbumParsed(const ItemInfoList&);
    void slotAdvance(const ItemInfo&);
    void slotOneAlbumIsComplete();
    void slotCancel()               override;

private:

    void init(RemoveAction action);
    void parseList();
    void parsePicture();
    void processOneAlbum();

private:

    class Private;
    Private* const d = nullptr;
};

} // namespace Digikam
