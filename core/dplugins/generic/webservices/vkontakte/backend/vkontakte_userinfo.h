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

#ifndef DIGIKAM_VKONTAKTE_USERINFO_H
#define DIGIKAM_VKONTAKTE_USERINFO_H

// Qt includes

#include <QMetaType>
#include <QJsonObject>
#include <QSharedDataPointer>

namespace Vkontakte
{

// https://vk.com/dev/fields
class UserInfo
{
public:

    enum
    {
        INVALID_TIMEZONE = 42
    };

public:

    UserInfo();
    UserInfo(const UserInfo& other);
    explicit UserInfo(const QJsonObject& jsonData);
    ~UserInfo();

    UserInfo &operator=(const UserInfo& other);

    /**
     * @brief Returns integer user ID at VK.
     *
     * UserInfoJob (the "users.get" method) always returns this, regardless of 
     * the requested additional fields.
     *
     * Returns -1 for uninitialized object or on server error.
     */
    int userId()        const;

    /**
     * @brief Returns user's first name in the default language.
     *
     * UserInfoJob (the "users.get" method) always returns this, regardless of 
     * the requested additional fields.
     *
     * Returns empty string for uninitialized object or on server error.
     */
    QString firstName() const;

    /**
     * @brief Returns user's last name in the default language.
     *
     * UserInfoJob (the "users.get" method) always returns this, regardless of 
     * the requested additional fields.
     *
     * Returns empty string for uninitialized object or on server error.
     */
    QString lastName() const;

    QString nickName() const;
    QString domain()   const;

    int sex()          const;

    bool online()      const;

    /**
     * @brief Returns the stringlist of all possible field groups
     * that may be requested in VK API method "users.get".
     *
     * @return The list of strings to pass as "fields" argument to a method.
     */
    static QStringList allQueryFields();

private:

    class Private;
    QSharedDataPointer<Private> d;
};

} // namespace Vkontakte

Q_DECLARE_METATYPE(Vkontakte::UserInfo)

#endif // DIGIKAM_VKONTAKTE_USERINFO_H
