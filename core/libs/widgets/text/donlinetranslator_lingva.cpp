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

#include "donlinetranslator_p.h"

namespace Digikam
{

void DOnlineTranslator::slotRequestLingvaTranslate()
{
    const QString sourceText = sender()->property(Private::s_textProperty).toString();

    // Generate API url

    QUrl url(QString::fromUtf8("%1/api/v1/%2/%3/%4")
                    .arg(d->lingvaUrl)
                    .arg(languageApiCode(Lingva, d->sourceLang))
                    .arg(languageApiCode(Lingva, d->translationLang))
                    .arg(QString::fromUtf8(QUrl::toPercentEncoding(sourceText))));

    d->currentReply = d->networkManager->get(QNetworkRequest(url));
}

void DOnlineTranslator::slotParseLingvaTranslate()
{
    d->currentReply->deleteLater();

    // Check for errors

    if (d->currentReply->error() != QNetworkReply::NoError)
    {
        resetData(NetworkError, d->currentReply->errorString());
        return;
    }

    // Parse translation data

    const QJsonDocument jsonResponse = QJsonDocument::fromJson(d->currentReply->readAll());
    const QJsonObject responseObject = jsonResponse.object();
    d->translation                    = responseObject.value(QStringLiteral("translation")).toString();
}

void DOnlineTranslator::buildLingvaStateMachine()
{
    // States

    auto* translationState = new QState(d->stateMachine);
    auto* finalState       = new QFinalState(d->stateMachine);
    d->stateMachine->setInitialState(translationState);

    // Transitions

    translationState->addTransition(translationState, &QState::finished, finalState);

    // Setup translation state

    buildSplitNetworkRequest(translationState,
                             &DOnlineTranslator::slotRequestLingvaTranslate,
                             &DOnlineTranslator::slotParseLingvaTranslate,
                             d->source,
                             Private::s_googleTranslateLimit);
}

void DOnlineTranslator::buildLingvaDetectStateMachine()
{
    // States

    auto* detectState = new QState(d->stateMachine);
    auto* finalState  = new QFinalState(d->stateMachine);
    d->stateMachine->setInitialState(detectState);

    detectState->addTransition(detectState, &QState::finished, finalState);

    // Setup lang detection state

    const QString text = d->source.left(getSplitIndex(d->source, Private::s_googleTranslateLimit));

    buildNetworkRequestState(detectState,
                             &DOnlineTranslator::slotRequestLingvaTranslate,
                             &DOnlineTranslator::slotParseLingvaTranslate,
                             text);
}

} // namespace Digikam
