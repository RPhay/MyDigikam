/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2015-06-21
 * Description : a tool to export items to Google web services
 *
 * Copyright (C) 2015      by Shourya Singh Gupta <shouryasgupta at gmail dot com>
 * Copyright (C) 2015-2020 by Caulier Gilles <caulier dot gilles at gmail dot com>
 * Copyright (C) 2018      by Thanh Trung Dinh <dinhthanhtrung1996 at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "gstalkerbase.h"

// Qt includes

#include <QMap>
#include <QDateTime>
#include <QDesktopServices>
#include <QOAuthHttpServerReplyHandler>

// Local includes

#include "digikam_debug.h"
#include "wstoolutils.h"

using namespace Digikam;

namespace DigikamGenericGoogleServicesPlugin
{

class Q_DECL_HIDDEN GSTalkerBase::Private
{
public:

    explicit Private()
      : parent    (nullptr),
        linked    (false),
        authUrl   (QLatin1String("https://accounts.google.com/o/oauth2/auth")),
        tokenUrl  (QLatin1String("https://accounts.google.com/o/oauth2/token")),
        identity  (QLatin1String("258540448336-hgdegpohibcjasvk1p595fpvjor15pbc.apps.googleusercontent.com")),
        sharedKey (QLatin1String("iiIKTNM4ggBXiTdquAzbs2xw")),
        netMngr   (nullptr)
    {
    }

    QWidget*               parent;

    bool                   linked;

    QString                authUrl;
    QString                tokenUrl;
    QString                identity;
    QString                sharedKey;

    QNetworkAccessManager* netMngr;
};

GSTalkerBase::GSTalkerBase(QWidget* const parent, const QStringList& scope, const QString& serviceName)
    : m_scope      (scope),
      m_serviceName(serviceName),
      m_reply      (nullptr),
      m_service    (nullptr),
      d            (new Private)
{
    d->parent  = parent;
    d->netMngr = new QNetworkAccessManager(this);
    m_service  = new QOAuth2AuthorizationCodeFlow(d->netMngr, this);

    m_service->setContentType(QAbstractOAuth::ContentType::Json);
    m_service->setClientIdentifierSharedKey(d->sharedKey);
    m_service->setScope(m_scope.join(QLatin1String(" ")));
    m_service->setAuthorizationUrl(QUrl(d->authUrl));
    m_service->setAccessTokenUrl(QUrl(d->tokenUrl));
    m_service->setClientIdentifier(d->identity);

    m_service->setModifyParametersFunction([](QAbstractOAuth::Stage stage, QVariantMap* parameters)
        {
            if (stage == QAbstractOAuth::Stage::RequestingAccessToken)
            {
                QByteArray code = parameters->value(QLatin1String("code")).toByteArray();
                (*parameters)[QLatin1String("code")] = QUrl::fromPercentEncoding(code);
            }
        }
    );

    QOAuthHttpServerReplyHandler* const replyHandler = new QOAuthHttpServerReplyHandler(8000, this);
    m_service->setReplyHandler(replyHandler);

    // OAuth configuration saved to between dk sessions

    m_service->setRefreshToken(WSToolUtils::readToken(m_serviceName));

    connect(m_service, &QOAuth2AuthorizationCodeFlow::authorizeWithBrowser,
            this, &GSTalkerBase::slotOpenBrowser);

    connect(m_service, &QOAuth2AuthorizationCodeFlow::tokenChanged,
            this, &GSTalkerBase::slotTokenChanged);

    connect(m_service, &QOAuth2AuthorizationCodeFlow::granted,
            this, &GSTalkerBase::slotLinkingSucceeded);

    connect(m_service, &QOAuth2AuthorizationCodeFlow::error,
            this, &GSTalkerBase::slotLinkingFailed);
}

GSTalkerBase::~GSTalkerBase()
{
    if (m_reply)
    {
        m_reply->abort();
    }

    delete d;
}

void GSTalkerBase::link()
{
    Q_EMIT signalBusy(true);
    m_service->grant();
}

void GSTalkerBase::unlink()
{
    Q_EMIT signalBusy(true);

    d->linked = false;

    m_service->setToken(QString());
    m_service->setRefreshToken(QString());

    WSToolUtils::clearToken(m_serviceName);

    m_bearerAccessToken.clear();
    m_accessToken.clear();
}

void GSTalkerBase::slotLinkingFailed()
{
    qCDebug(DIGIKAM_WEBSERVICES_LOG) << "LINK to" << m_serviceName << "fail";

    d->linked = false;

    Q_EMIT signalBusy(false);
    Q_EMIT signalAuthenticationRefused();
}

void GSTalkerBase::slotLinkingSucceeded()
{
    if (m_service->status() == QAbstractOAuth::Status::Granted)
    {
        qCDebug(DIGIKAM_WEBSERVICES_LOG) << "LINK to" << m_serviceName << "ok";

        d->linked = true;

        Q_EMIT signalAccessTokenObtained();
    }
}

void GSTalkerBase::slotOpenBrowser(const QUrl& url)
{
    qCDebug(DIGIKAM_WEBSERVICES_LOG) << "Open Browser... (" << url << ")";

    QDesktopServices::openUrl(url);
}

bool GSTalkerBase::authenticated() const
{
    return d->linked;
}

void GSTalkerBase::slotTokenChanged(const QString& token)
{
    m_accessToken       = token;
    m_bearerAccessToken = QLatin1String("Bearer ") + m_accessToken;

    WSToolUtils::saveToken(m_serviceName, m_service->refreshToken());
}

void GSTalkerBase::doOAuth()
{
    qCDebug(DIGIKAM_WEBSERVICES_LOG) << "current time" << QDateTime::currentDateTime();
    qCDebug(DIGIKAM_WEBSERVICES_LOG) << "expires at: " << m_service->expirationAt();

    if (!m_service->refreshToken().isEmpty())
    {
        m_service->refreshAccessToken();
    }
    else
    {
        link();
    }
}

} // namespace DigikamGenericGoogleServicesPlugin
