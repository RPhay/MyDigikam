/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2018-06-29
 * Description : a tool to export images to Twitter social network
 *
 * SPDX-FileCopyrightText: 2018 by Tarek Talaat <tarektalaat93 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_TWITTER_ITEM_H
#define DIGIKAM_TWITTER_ITEM_H

 // Qt includes

#include <QString>

namespace DigikamGenericTwitterPlugin
{

class TwUser
{
public:

    TwUser()
      : id        (0),
        uploadPerm(false)
    {
    }

    void clear()
    {
        id         = 0;
        name.clear();
        profileURL = QLatin1String("https://www.facebook.com");
        uploadPerm = true;
    }

    long long id;

    QString   name;
    QString   profileURL;
    bool      uploadPerm;
};

// ---------------------------------------------------------------

/*
enum TWPrivacy
{
    FB_ME = 0,
    FB_FRIENDS = 1,
    FB_FRIENDS_OF_FRIENDS,
    FB_NETWORKS,
    FB_EVERYONE,
    FB_CUSTOM
};
*/

// ---------------------------------------------------------------

class TwAlbum
{
public:

    TwAlbum()
/*
      : privacy = FB_FRIENDS;
*/
    {
    }

    QString   id;

    QString   title;
    QString   description;
    QString   location;
/*
    FbPrivacy privacy;
*/
    QString   url;
};

// ---------------------------------------------------------------

class TwPhoto
{
public:

    TwPhoto()
    {
    }

    QString id;

    QString caption;
    QString thumbURL;
    QString originalURL;
};

} // namespace DigikamGenericTwitterPlugin

#endif // DIGIKAM_TWITTER_ITEM_H
