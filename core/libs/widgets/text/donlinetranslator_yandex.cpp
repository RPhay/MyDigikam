/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2018-02-22
 * Description : A text translator using web-services - Tnadex methods.
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

void DOnlineTranslator::slotRequestYandexKey()
{
    const QUrl url(QStringLiteral("https://translate.yandex.com"));
    m_currentReply = m_networkManager->get(QNetworkRequest(url));
}

void DOnlineTranslator::slotParseYandexKey()
{
    m_currentReply->deleteLater();

    if (m_currentReply->error() != QNetworkReply::NoError)
    {
        resetData(NetworkError, m_currentReply->errorString());
        return;
    }

    // Check availability of service

    const QByteArray webSiteData = m_currentReply->readAll();

    if (webSiteData.isEmpty()                        ||
        webSiteData.contains("<title>Oops!</title>") ||
        webSiteData.contains("<title>302 Found</title>"))
    {
        resetData(ServiceError, i18n("Error: Engine systems have detected suspicious traffic "
                                     "from your computer network. Please try your request again later."));
        return;
    }

    const QByteArray sidBeginString = "SID: '";
    const int sidBeginStringPos     = webSiteData.indexOf(sidBeginString);

    if (sidBeginStringPos == -1)
    {
        resetData(ParsingError, i18n("Error: Unable to find Yandex SID in web version."));
        return;
    }

    const int sidBeginPosition = sidBeginStringPos + sidBeginString.size();
    const int sidEndPosition   = webSiteData.indexOf('\'', sidBeginPosition);

    if (sidEndPosition == -1)
    {
        resetData(ParsingError, i18n("Error: Unable to extract Yandex SID from web version."));
        return;
    }

    // Yandex show reversed parts of session ID, need to decode

    const QString sid    = QString::fromUtf8(webSiteData.mid(sidBeginPosition,
                                                             sidEndPosition - sidBeginPosition));

    QStringList sidParts = sid.split(QLatin1Char('.'));

    for (int i = 0 ; i < sidParts.size() ; ++i)
    {
        std::reverse(sidParts[i].begin(), sidParts[i].end());
    }

    s_yandexKey = sidParts.join(QLatin1Char('.'));
}

void DOnlineTranslator::slotRequestYandexTranslate()
{
    const QString sourceText = sender()->property(s_textProperty).toString();

    QString lang;

    if (m_sourceLang == Auto)
    {
        lang = languageApiCode(Yandex, m_translationLang);
    }
    else
    {
        lang = languageApiCode(Yandex, m_sourceLang) + QLatin1Char('-') + languageApiCode(Yandex, m_translationLang);
    }

    // Generate API url

    QUrl url(QStringLiteral("https://translate.yandex.net/api/v1/tr.json/translate"));

    url.setQuery(QStringLiteral("id=%1-2-0&srv=tr-text&text=%2&lang=%3")
                     .arg(s_yandexKey,
                          QString::fromUtf8(QUrl::toPercentEncoding(sourceText)),
                          lang));

    // Setup request

    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader, QLatin1String("application/x-www-form-urlencoded"));
    request.setUrl(url);

    // Make reply

    m_currentReply = m_networkManager->post(request, QByteArray());
}

void DOnlineTranslator::slotParseYandexTranslate()
{
    m_currentReply->deleteLater();

    // Check for errors

    if (m_currentReply->error() != QNetworkReply::NoError)
    {
        // Network errors

        if (m_currentReply->error() < QNetworkReply::ContentAccessDenied)
        {
            resetData(NetworkError, m_currentReply->errorString());
            return;
        }

        // Parse data to get request error type

        s_yandexKey.clear();
        const QJsonDocument jsonResponse = QJsonDocument::fromJson(m_currentReply->readAll());
        resetData(ServiceError, jsonResponse.object().value(QStringLiteral("message")).toString());

        return;
    }

    // Read Json

    const QJsonDocument jsonResponse = QJsonDocument::fromJson(m_currentReply->readAll());
    const QJsonObject jsonData       = jsonResponse.object();

    // Parse language

    if (m_sourceLang == Auto)
    {
        QString sourceCode = jsonData.value(QStringLiteral("lang")).toString();
        sourceCode         = sourceCode.left(sourceCode.indexOf(QLatin1Char('-')));
        m_sourceLang       = language(Yandex, sourceCode);

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

    // Parse translation data

    m_translation += jsonData.value(QStringLiteral("text")).toArray().at(0).toString();
}

void DOnlineTranslator::slotRequestYandexSourceTranslit()
{
    requestYandexTranslit(m_sourceLang);
}

void DOnlineTranslator::slotParseYandexSourceTranslit()
{
    parseYandexTranslit(m_sourceTranslit);
}

void DOnlineTranslator::slotRequestYandexTranslationTranslit()
{
    requestYandexTranslit(m_translationLang);
}

void DOnlineTranslator::slotParseYandexTranslationTranslit()
{
    parseYandexTranslit(m_translationTranslit);
}

void DOnlineTranslator::slotRequestYandexDictionary()
{
    // Check if language is supported (need to check here because language may be autodetected)

    if (!isSupportDictionary(Yandex, m_sourceLang, m_translationLang) &&
        !m_source.contains(QLatin1Char(' ')))
    {
        auto* state = qobject_cast<QState*>(sender());
        state->addTransition(new QFinalState(state->parentState()));
        return;
    }

    // Generate API url

    const QString text = sender()->property(s_textProperty).toString();
    QUrl url(QStringLiteral("https://dictionary.yandex.net/dicservice.json/lookupMultiple"));

    url.setQuery(QStringLiteral("text=%1&ui=%2&dict=%3-%4")
                     .arg(QString::fromUtf8(QUrl::toPercentEncoding(text)),
                          languageApiCode(Yandex, m_uiLang),
                          languageApiCode(Yandex, m_sourceLang),
                          languageApiCode(Yandex, m_translationLang)));

    m_currentReply = m_networkManager->get(QNetworkRequest(url));
}

void DOnlineTranslator::slotParseYandexDictionary()
{
    m_currentReply->deleteLater();

    if (m_currentReply->error() != QNetworkReply::NoError)
    {
        resetData(NetworkError, m_currentReply->errorString());
        return;
    }

    // Parse reply

    const QJsonDocument jsonResponse = QJsonDocument::fromJson(m_currentReply->readAll());
    const QJsonValue jsonData        = jsonResponse.object().value(languageApiCode(Yandex, m_sourceLang) +
                                       QLatin1Char('-')                                                  +
                                       languageApiCode(Yandex, m_translationLang)).toObject().value(QStringLiteral("regular"));

    if (m_sourceTranscriptionEnabled)
    {
        m_sourceTranscription = jsonData.toArray().at(0).toObject().value(QStringLiteral("ts")).toString();
    }

    for (const QJsonValueRef typeOfSpeechData : jsonData.toArray())
    {
        QJsonObject typeOfSpeechObject = typeOfSpeechData.toObject();
        const QString typeOfSpeech     = typeOfSpeechObject.value(QStringLiteral("pos")).toObject().value(QStringLiteral("text")).toString();

        for (const QJsonValueRef wordData : typeOfSpeechObject.value(QStringLiteral("tr")).toArray())
        {
            // Parse translation options

            const QJsonObject wordObject       = wordData.toObject();
            const QString word                 = wordObject.value(QStringLiteral("text")).toString();
            const QString gender               = wordObject.value(QStringLiteral("gen")).toObject().value(QStringLiteral("text")).toString();
            const QJsonArray translationsArray = wordObject.value(QStringLiteral("mean")).toArray();
            QStringList translations;
            translations.reserve(translationsArray.size());

            for (const QJsonValue &wordTranslation : translationsArray)
            {
                translations.append(wordTranslation.toObject().value(QStringLiteral("text")).toString());
            }

            m_translationOptions[typeOfSpeech].append({word, gender, translations});
        }
    }
}

void DOnlineTranslator::buildYandexStateMachine()
{
    // States

    auto* keyState                  = new QState(m_stateMachine); // Generate SID from web version first to access API
    auto* translationState          = new QState(m_stateMachine);
    auto* sourceTranslitState       = new QState(m_stateMachine);
    auto* translationTranslitState  = new QState(m_stateMachine);
    auto* dictionaryState           = new QState(m_stateMachine);
    auto* finalState                = new QFinalState(m_stateMachine);
    m_stateMachine->setInitialState(keyState);

    // Transitions

    keyState->addTransition(keyState, &QState::finished, translationState);
    translationState->addTransition(translationState, &QState::finished, sourceTranslitState);
    sourceTranslitState->addTransition(sourceTranslitState, &QState::finished, translationTranslitState);
    translationTranslitState->addTransition(translationTranslitState, &QState::finished, dictionaryState);
    dictionaryState->addTransition(dictionaryState, &QState::finished, finalState);

    // Setup key state

    if (s_yandexKey.isEmpty())
    {
        buildNetworkRequestState(keyState,
                                 &DOnlineTranslator::slotRequestYandexKey,
                                 &DOnlineTranslator::slotParseYandexKey);
    }
    else
    {
        keyState->setInitialState(new QFinalState(keyState));
    }

    // Setup translation state

    buildSplitNetworkRequest(translationState,
                             &DOnlineTranslator::slotRequestYandexTranslate,
                             &DOnlineTranslator::slotParseYandexTranslate,
                             m_source,
                             s_yandexTranslateLimit);

    // Setup source translit state

    if (m_sourceTranslitEnabled)
    {
        buildSplitNetworkRequest(sourceTranslitState,
                                 &DOnlineTranslator::slotRequestYandexSourceTranslit,
                                 &DOnlineTranslator::slotParseYandexSourceTranslit,
                                 m_source,
                                 s_yandexTranslitLimit);
    }
    else

    // Setup source translit state

    if (m_sourceTranslitEnabled)
    {
        buildSplitNetworkRequest(sourceTranslitState,
                                 &DOnlineTranslator::slotRequestYandexSourceTranslit,
                                 &DOnlineTranslator::slotParseYandexSourceTranslit,
                                 m_source,
                                 s_yandexTranslitLimit);
    }
    else
    {
        sourceTranslitState->setInitialState(new QFinalState(sourceTranslitState));
    }

    // Setup translation translit state

    if (m_translationTranslitEnabled)
    {
        buildSplitNetworkRequest(translationTranslitState,
                                 &DOnlineTranslator::slotRequestYandexTranslationTranslit,
                                 &DOnlineTranslator::slotParseYandexTranslationTranslit,
                                 m_translation,
                                 s_yandexTranslitLimit);
    }
    else
    {
        translationTranslitState->setInitialState(new QFinalState(translationTranslitState));
    }

    // Setup dictionary state

    if (m_translationOptionsEnabled && !isContainsSpace(m_source))
    {
        buildNetworkRequestState(dictionaryState,
                                 &DOnlineTranslator::slotRequestYandexDictionary,
                                 &DOnlineTranslator::slotParseYandexDictionary,
                                 m_source);
    }
    else
    {
        dictionaryState->setInitialState(new QFinalState(dictionaryState));
    }
}

void DOnlineTranslator::buildYandexDetectStateMachine()
{
    // States

    auto* keyState    = new QState(m_stateMachine); // Generate SID from web version first to access API
    auto* detectState = new QState(m_stateMachine);
    auto* finalState  = new QFinalState(m_stateMachine);
    m_stateMachine->setInitialState(keyState);

    // Transitions

    keyState->addTransition(keyState, &QState::finished, detectState);
    detectState->addTransition(detectState, &QState::finished, finalState);

    // Setup key state

    if (s_yandexKey.isEmpty())
    {
        buildNetworkRequestState(keyState,
                                 &DOnlineTranslator::slotRequestYandexKey,
                                 &DOnlineTranslator::slotParseYandexKey);
    }
    else
    {
        keyState->setInitialState(new QFinalState(keyState));
    }

    // Setup detect state

    const QString text = m_source.left(getSplitIndex(m_source, s_yandexTranslateLimit));

    buildNetworkRequestState(detectState,
                             &DOnlineTranslator::slotRequestYandexTranslate,
                             &DOnlineTranslator::slotParseYandexTranslate,
                             text);
}

void DOnlineTranslator::requestYandexTranslit(Language language)
{
    // Check if language is supported (need to check here because language may be autodetected)

    if (!isSupportTranslit(Yandex, language))
    {
        auto* state = qobject_cast<QState *>(sender());
        state->addTransition(new QFinalState(state->parentState()));

        return;
    }

    const QString text = sender()->property(s_textProperty).toString();

    // Generate API url

    QUrl url(QStringLiteral("https://translate.yandex.net/translit/translit"));
    url.setQuery(QString::fromUtf8("text=%1&lang=%2")
                 .arg(QString::fromUtf8(QUrl::toPercentEncoding(text)))
                 .arg(languageApiCode(Yandex, language)));

    m_currentReply = m_networkManager->get(QNetworkRequest(url));
}

void DOnlineTranslator::parseYandexTranslit(QString& text)
{
    m_currentReply->deleteLater();

    if (m_currentReply->error() != QNetworkReply::NoError)
    {
        resetData(NetworkError, m_currentReply->errorString());

        return;
    }

    const QByteArray reply = m_currentReply->readAll();

#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)

    text += QString::fromUtf8(reply.mid(1).chopped(1));

#else

    text += QString::fromUtf8(reply.mid(1));
    text.chop(1);

#endif

}

} // namespace Digikam
