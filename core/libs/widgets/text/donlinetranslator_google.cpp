/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2018-02-22
 * Description : A text translator using web-services - Google methods.
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

void DOnlineTranslator::requestGoogleTranslate()
{
    const QString sourceText = sender()->property(s_textProperty).toString();

    // Generate API url

    QUrl url(QStringLiteral("https://translate.googleapis.com/translate_a/single"));

    url.setQuery(QStringLiteral("client=gtx&ie=UTF-8&oe=UTF-8&dt=bd&dt=ex&dt=ld&dt=md&dt=rw&dt=rm&dt=ss&dt=t&dt=at&dt=qc&sl=%1&tl=%2&hl=%3&q=%4")
                     .arg(languageApiCode(Google, m_sourceLang),
                          languageApiCode(Google, m_translationLang),
                          languageApiCode(Google, m_uiLang),
                          QString::fromUtf8(QUrl::toPercentEncoding(sourceText))));

    m_currentReply = m_networkManager->get(QNetworkRequest(url));
}

void DOnlineTranslator::parseGoogleTranslate()
{
    m_currentReply->deleteLater();

    // Check for error

    if (m_currentReply->error() != QNetworkReply::NoError)
    {
        if (m_currentReply->error() == QNetworkReply::ServiceUnavailableError)
        {
            resetData(ServiceError, i18n("Error: Engine systems have detected suspicious traffic from your "
                                         "computer network. Please try your request again later."));
        }
        else
        {
            resetData(NetworkError, m_currentReply->errorString());
        }

        return;
    }

    // Check availability of service

    const QByteArray data = m_currentReply->readAll();

    if (data.startsWith('<'))
    {
        resetData(ServiceError, i18n("Error: Engine systems have detected suspicious traffic from your "
                                     "computer network. Please try your request again later."));
        return;
    }

    // Read Json

    const QJsonDocument jsonResponse = QJsonDocument::fromJson(data);
    const QJsonArray jsonData        = jsonResponse.array();

    if (m_sourceLang == Auto)
    {
        // Parse language

        m_sourceLang = language(Google, jsonData.at(2).toString());

        if (m_sourceLang == NoLanguage)
        {
            resetData(ParsingError, i18n("Error: Unable to parse autodetected language"));
            return;
        }

        if (m_onlyDetectLanguage)
        {
            return;
        }
    }

    addSpaceBetweenParts(m_translation);
    addSpaceBetweenParts(m_translationTranslit);
    addSpaceBetweenParts(m_sourceTranslit);

    for (const QJsonValueRef translationData : jsonData.at(0).toArray())
    {
        const QJsonArray translationArray = translationData.toArray();
        m_translation.append(translationArray.at(0).toString());

        if (m_translationTranslitEnabled)
        {
            m_translationTranslit.append(translationArray.at(2).toString());
        }

        if (m_sourceTranslitEnabled)
        {
            m_sourceTranslit.append(translationArray.at(3).toString());
        }
    }

    if (m_source.size() >= s_googleTranslateLimit)
    {
        return;
    }

    // Translation options

    if (m_translationOptionsEnabled)
    {
        for (const QJsonValueRef typeOfSpeechData : jsonData.at(1).toArray())
        {
            const QJsonArray typeOfSpeechDataArray = typeOfSpeechData.toArray();
            const QString typeOfSpeech             = typeOfSpeechDataArray.at(0).toString();

            for (const QJsonValueRef wordData : typeOfSpeechDataArray.at(2).toArray())
            {
                const QJsonArray wordDataArray     = wordData.toArray();
                const QString word                 = wordDataArray.at(0).toString();
                const QString gender               = wordDataArray.at(4).toString();
                const QJsonArray translationsArray = wordDataArray.at(1).toArray();
                QStringList translations;
                translations.reserve(translationsArray.size());

                for (const QJsonValue &wordTranslation : translationsArray)
                {
                    translations.append(wordTranslation.toString());
                }

                m_translationOptions[typeOfSpeech].append({word, gender, translations});
            }
        }
    }
}

void DOnlineTranslator::buildGoogleStateMachine()
{
    // States (Google sends translation, translit and dictionary in one request,
    // that will be splitted into several by the translation limit)

    auto* translationState = new QState(m_stateMachine);
    auto* finalState       = new QFinalState(m_stateMachine);
    m_stateMachine->setInitialState(translationState);

    translationState->addTransition(translationState, &QState::finished, finalState);

    // Setup translation state

    buildSplitNetworkRequest(translationState,
                             &DOnlineTranslator::requestGoogleTranslate,
                             &DOnlineTranslator::parseGoogleTranslate,
                             m_source,
                             s_googleTranslateLimit);
}

void DOnlineTranslator::buildGoogleDetectStateMachine()
{
    // States

    auto* detectState  = new QState(m_stateMachine);
    auto* finalState   = new QFinalState(m_stateMachine);
    m_stateMachine->setInitialState(detectState);

    detectState->addTransition(detectState, &QState::finished, finalState);

    // Setup detect state

    const QString text = m_source.left(getSplitIndex(m_source, s_googleTranslateLimit));

    buildNetworkRequestState(detectState,
                             &DOnlineTranslator::requestGoogleTranslate,
                             &DOnlineTranslator::parseGoogleTranslate,
                             text);
}

} // namespace Digikam
