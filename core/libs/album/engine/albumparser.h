/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-01-24
 * Description : album parser progress indicator
 *
 * Copyright (C) 2012-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2019-2020 by Minh Nghia Duong <minhnghiaduong997 at gmail dot com>
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

#ifndef DIGIKAM_ALBUM_PARSER_H
#define DIGIKAM_ALBUM_PARSER_H

// Local includes

#include "iteminfo.h"
#include "progressmanager.h"

namespace Digikam
{
class Album;

class DIGIKAM_GUI_EXPORT AlbumParser : public ProgressItem
{
    Q_OBJECT

public:

    /**
     * Contructor to work on image list
     */
    explicit AlbumParser(const ItemInfoList& infoList);

    /**
     * Contructor to work on recursive mode from album
     */
    explicit AlbumParser(Album* const album);

    ~AlbumParser() override;

    void run();

Q_SIGNALS:

    void signalComplete(const QList<QUrl>&);

private Q_SLOTS:

    void slotRun();
    void slotCancel();
    void slotParseItemInfoList(const ItemInfoList& list);

private:

    // Disable
    AlbumParser(QObject*);

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_ALBUM_PARSER_H
