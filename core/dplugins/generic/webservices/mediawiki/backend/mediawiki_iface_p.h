/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2011-03-22
 * Description : a Iface C++ interface
 *
 * SPDX-FileCopyrightText: 2011-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2011      by Remi Benoit <r3m1 dot benoit at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_MEDIAWIKI_IFACE_P_H
#define DIGIKAM_MEDIAWIKI_IFACE_P_H

#include "mediawiki_iface.h"

// Qt includes

#include <QString>
#include <QUrl>
#include <QNetworkAccessManager>

namespace MediaWiki
{

class Q_DECL_HIDDEN Iface::Private
{

public:

    Private(const QUrl& url, const QString& userAgent, QNetworkAccessManager* const manager)
        : url      (url),
          userAgent(userAgent),
          manager  (manager)
    {
    }

    ~Private()
    {
        delete manager;
    }

public:

    static const QString         POSTFIX_USER_AGENT;

    const QUrl                   url;
    const QString                userAgent;
    QNetworkAccessManager* const manager         = nullptr;
};

const QString Iface::Private::POSTFIX_USER_AGENT = QString::fromUtf8("MediaWiki-silk");

} // namespace MediaWiki

#endif // DIGIKAM_MEDIAWIKI_IFACE_P_H
