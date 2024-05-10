/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2013-11-18
 * Description : a tool to export images to Dropbox web service
 *
 * SPDX-FileCopyrightText: 2013      by Pankaj Kumar <me at panks dot me>
 * SPDX-FileCopyrightText: 2013-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_DB_NEW_ALBUM_DLG_H
#define DIGIKAM_DB_NEW_ALBUM_DLG_H

// Local includes

#include "wsnewalbumdialog.h"

using namespace Digikam;

namespace DigikamGenericDropBoxPlugin
{

class DBFolder;

class DBNewAlbumDlg : public WSNewAlbumDialog
{
    Q_OBJECT

public:

    explicit DBNewAlbumDlg(QWidget* const parent, const QString& toolName);
    ~DBNewAlbumDlg() override = default;

    void getFolderTitle(DBFolder& folder);
};

} // namespace DigikamGenericDropBoxPlugin

#endif // DIGIKAM_DB_NEW_ALBUM_DLG_H
