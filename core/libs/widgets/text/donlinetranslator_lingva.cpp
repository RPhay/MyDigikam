/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2018-02-22
 * Description : A text translator using web-services - Lingva methods.
 *
 * Copyright (C) 2018-2022 by Hennadii Chernyshchyk <genaloner at gmail dot com>
 * Copyright (C) 2021-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "donlinetranslator.h"

// Qt includes

#include <QCoreApplication>
#include <QFinalState>
#include <QJsonDocument>
#include <QNetworkReply>
#include <QStateMachine>

// KDE includes

#include <klocalizedstring.h>

namespace Digikam
{

void DOnlineTranslator::slotRequestLingvaTranslate()
{
    const QString sourceText = sender()->property(s_textProperty).toString();

    // Generate API url

    QUrl url(QString::fromUtf8("%1/api/v1/%2/%3/%4")
                    .arg(m_lingvaUrl)
                    .arg(languageApiCode(Lingva, m_sourceLang))
                    .arg(languageApiCode(Lingva, m_translationLang))
                    .arg(QString::fromUtf8(QUrl::toPercentEncoding(sourceText))));

    m_currentReply = m_networkManager->get(QNetworkRequest(url));
}

void DOnlineTranslator::slotParseLingvaTranslate()
{
    m_currentReply->deleteLater();

    // Check for errors

    if (m_currentReply->error() != QNetworkReply::NoError)
    {
        resetData(NetworkError, m_currentReply->errorString());
        return;
    }

    // Parse translation data

    const QJsonDocument jsonResponse = QJsonDocument::fromJson(m_currentReply->readAll());
    const QJsonObject responseObject = jsonResponse.object();
    m_translation                    = responseObject.value(QStringLiteral("translation")).toString();
}

void DOnlineTranslator::buildLingvaStateMachine()
{
    // States

    auto* translationState = new QState(m_stateMachine);
    auto* finalState       = new QFinalState(m_stateMachine);
    m_stateMachine->setInitialState(translationState);

    // Transitions

    translationState->addTransition(translationState, &QState::finished, finalState);

    // Setup translation state

    buildSplitNetworkRequest(translationState,
                             &DOnlineTranslator::slotRequestLingvaTranslate,
                             &DOnlineTranslator::slotParseLingvaTranslate,
                             m_source,
                             s_googleTranslateLimit);
}

void DOnlineTranslator::buildLingvaDetectStateMachine()
{
    // States

    auto* detectState = new QState(m_stateMachine);
    auto* finalState  = new QFinalState(m_stateMachine);
    m_stateMachine->setInitialState(detectState);

    detectState->addTransition(detectState, &QState::finished, finalState);

    // Setup lang detection state

    const QString text = m_source.left(getSplitIndex(m_source, s_googleTranslateLimit));

    buildNetworkRequestState(detectState,
                             &DOnlineTranslator::slotRequestLingvaTranslate,
                             &DOnlineTranslator::slotParseLingvaTranslate,
                             text);
}

} // namespace Digikam
