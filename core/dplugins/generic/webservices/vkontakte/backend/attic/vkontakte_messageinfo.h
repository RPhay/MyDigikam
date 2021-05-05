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

#ifndef DIGIKAM_VKONTAKTE_MESSAGEINFO_H
#define DIGIKAM_VKONTAKTE_MESSAGEINFO_H

// Qt includes

#include <QDateTime>

namespace Vkontakte
{

/**
 * Class that describes a private message on vk.com
 */
class MessageInfo : public QObject
{
    Q_OBJECT

    // date, uid, mid, title, body, read_state, out
    Q_PROPERTY(QString date WRITE setDateString READ dateString)
    Q_PROPERTY(int uid WRITE setUid READ uid)
    Q_PROPERTY(int mid WRITE setMid READ mid)
    Q_PROPERTY(QString title WRITE setTitle READ title)
    Q_PROPERTY(QString body WRITE setBody READ body)
    Q_PROPERTY(int read_state WRITE setReadState READ readState)
    Q_PROPERTY(int out WRITE setOut READ out) // do non-discussion messages have this?

    // For chats: chat_id, chat_active
    Q_PROPERTY(QString chat_id WRITE setChatId READ chatId)
    Q_PROPERTY(QString chat_active WRITE setChatActive READ chatActive)

public:

    MessageInfo();
    ~MessageInfo();

    /**
     * Set the creation time of the note
     * @param createdTime Time as UNIX timestamp
     */
    void setDateString(const QString& createdDate);

    /**
     * Returns the creation date/time as a UNIX timestamp
     */
    QString dateString() const;

    /**
     * Returns the creation date/time in QDateTime
     */
    QDateTime date() const;

    void setUid(int uid);
    int uid() const;

    void setMid(int mid);
    int mid() const;

    void setTitle(const QString& title);
    QString title() const;

    /**
     * @brief Returns the title without "Re(n): " prefix
     **/
    QString coreTitle() const;

    void setBody(const QString& body);
    QString body() const;

    void setReadState(int readState);
    int readState() const;

    void setOut(int out);
    int out() const;

    void setChatId(const QString& chatId);
    QString chatId() const;

    void setChatActive(const QString& chatActive);
    QString chatActive() const;

    QString remoteId() const;

private:

    class Private;
    Private* const d;
};

/**
 * @brief This class is needed for using qSort on QLists of MessageInfoPtrs
 **/
class MessageInfoPtr : public QSharedPointer<MessageInfo>
{
public:

    explicit MessageInfoPtr(MessageInfo* const ptr);

    /**
     * @brief Compares by message ID (MessageInfo::m_mid)
     **/
    bool operator<(const MessageInfoPtr& o) const;
};

} // namespace Vkontakte

#endif // DIGIKAM_VKONTAKTE_MESSAGEINFO_H
