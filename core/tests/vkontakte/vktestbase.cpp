/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2011-02-19
 * Description : Unit tests for VKontakte web service
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

#include "vktestbase.h"

// Qt includes

#include <QEventLoop>
#include <QFile>
#include <QTextStream>
#include <QTest>

// Local includes

#include "vkontakte_vkapi.h"

#define VK_APP_ID "2446321"

VkTestBase::VkTestBase(QObject* const /*parent*/)
    : m_vkapi(new Vkontakte::VkApi(nullptr))
{
}

VkTestBase::~VkTestBase()
{
    if (m_vkapi)
    {
        delete m_vkapi;
    }
}

QString VkTestBase::getSavedToken() const
{
    QFile file(QLatin1String(AUTOTESTS_API_TOKEN_PATH));

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return QString();
    }

    QTextStream in(&file);
    QString line = in.readLine();

    return line.trimmed();
}

void VkTestBase::authenticate(Vkontakte::AppPermissions::Value permissions)
{
    m_vkapi->setAppId(QLatin1String(VK_APP_ID)); // TODO: library should better not crash if setAppId is not called
    m_vkapi->setRequiredPermissions(permissions);

    QString token = getSavedToken();

    if (!token.isEmpty())
    {
        m_vkapi->setInitialAccessToken(token);
    }

    m_vkapi->startAuthentication(false);

    // Wait for any outcome of the authentication process, including failure
    QEventLoop loop;

    connect(m_vkapi, SIGNAL(authenticated()),
            &loop, SLOT(quit()));

    connect(m_vkapi, SIGNAL(canceled()),
            &loop, SLOT(quit()));

    loop.exec();

    QVERIFY(m_vkapi->isAuthenticated());
}

QString VkTestBase::accessToken() const
{
    return m_vkapi->accessToken();
}
