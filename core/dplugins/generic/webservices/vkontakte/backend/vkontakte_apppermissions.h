/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2011-02-19
 * Description : a tool to export images to VKontakte web service
 *
 * Copyright (C) 2011-2015 by Alexander Potashev <aspotashev at gmail dot com>
 * Copyright (C) 2011-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef DIGIKAM_VKONTAKTE_APPPERMISSIONS_H
#define DIGIKAM_VKONTAKTE_APPPERMISSIONS_H

namespace Vkontakte
{

namespace AppPermissions
{

enum Value
{
    NoPermissions   = 0,

    // User allowed to send notifications to him/her
    Notify          = 1,
    // Access to friends
    Friends         = 2,
    // Access to photos
    Photos          = 4,
    // Access to audios
    Audio           = 8,
    // Access to videos
    Video           = 16,
    // Access to offers (obsolete methods)
    Offers          = 32,
    // Access to questions (obsolete methods).
    Questions       = 64,
    // Access to wiki pages
    Pages           = 128,
    // Addition of link to the application in the left menu
    AppLink         = 256,
    // Access to user status
    Status          = 1024,
    // Access to user notes
    Notes           = 2048,
    // Access to advanced methods for messaging
    Messages        = 4096,
    // Access to standard and advanced methods for the wall
    Wall            = 8192,
    // Access to advanced methods for Ads API
    Ads             = 32768,
    // Access to API at any time from a third party server
    Offline         = 65536,
    // Access to documents
    Docs            = 131072,
    // Access to user groups
    Groups          = 262144,
    // Access to notifications about answers to the user
    Notifications   = 524288,
    // Access to statistics of user's own groups and applications
    Stats           = 1048576,
    // User e-mail access. Available only for sites.
    Email           = 4194304
}; // enum Value

inline AppPermissions::Value operator|(AppPermissions::Value a, AppPermissions::Value b)
{
    return static_cast<AppPermissions::Value>(static_cast<int>(a) | static_cast<int>(b));
}

inline AppPermissions::Value& operator|=(AppPermissions::Value& a, AppPermissions::Value b)
{
    return (a = a | b);
}

} // namespace AppPermissions

} // namespace Vkontakte

#endif // DIGIKAM_VKONTAKTE_APPPERMISSIONS_H
