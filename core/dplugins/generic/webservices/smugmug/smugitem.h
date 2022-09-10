/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-12-01
 * Description : a tool to export images to Smugmug web service
 *
 * SPDX-FileCopyrightText: 2008-2009 by Luka Renko <lure at kubuntu dot org>
 * SPDX-FileCopyrightText: 2008-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2018      by Thanh Trung Dinh <dinhthanhtrung1996 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_SMUG_ITEM_H
#define DIGIKAM_SMUG_ITEM_H

// Qt includes

#include <QString>

namespace DigikamGenericSmugPlugin
{

class SmugUser
{

public:

    explicit SmugUser()
      : fileSizeLimit(0)
    {
    }

    void clear()
    {
        email.clear();
        nickName.clear();
        displayName.clear();
        accountType.clear();
        userUri.clear();
        nodeUri.clear();
        folderUri.clear();
        fileSizeLimit = 0;
    }

public:

    QString email;
    QString nickName;
    QString displayName;
    QString accountType;
    QString userUri;
    QString nodeUri;
    QString folderUri;
    int     fileSizeLimit;
};

// ---------------------------------------------------------------------------------

class SmugAlbum
{

public:

    explicit SmugAlbum()
      : id           (-1),
        categoryID   (-1),
        subCategoryID(-1),
        isPublic     (true),
        canShare     (true),
        imageCount   (0),
        tmplID       (-1)
    {
    }

public:

    qint64  id;
    QString nodeID;

    QString name;
    QString key;
    QString title;
    QString description;
    QString keywords;

    qint64  categoryID;
    QString category;

    qint64  subCategoryID;
    QString subCategory;

    bool    isPublic;
    QString password;
    bool    canShare;
    QString passwordHint;

    int     imageCount;

    /// below fields only used by createAlbum (and not by listAlbums)
    qint64  tmplID;
    QString tmpl;

    static bool lessThan(SmugAlbum& a, SmugAlbum& b)
    {
        return a.title.toLower() < b.title.toLower();
    }
};

// ---------------------------------------------------------------------------------

class SmugPhoto
{

public:

    explicit SmugPhoto()
      : id(-1)
    {
    }

public:

    qint64  id;

    QString key;
    QString caption;
    QString keywords;

    QString thumbURL;
    QString originalURL;
};

// ---------------------------------------------------------------------------------

class SmugAlbumTmpl
{

public:

    explicit SmugAlbumTmpl()
      : id(-1),
        isPublic(true)
    {
    }

public:

    qint64  id;
    QString name;
    QString uri;

    bool    isPublic;
    QString password;
    QString passwordHint;
};

// ---------------------------------------------------------------------------------

class SmugCategory
{

public:

    explicit SmugCategory()
      : id(-1)
    {
    }

public:

    qint64  id;
    QString name;
};

} // namespace DigikamGenericSmugPlugin

#endif // DIGIKAM_SMUG_ITEM_H
