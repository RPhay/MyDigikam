/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2018-02-22
 * Description : A text translator using web-services - Bing methods.
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

void DOnlineTranslator::slotRequestBingCredentials()
{
    const QUrl url(QStringLiteral("https://www.bing.com/translator"));
    m_currentReply = m_networkManager->get(QNetworkRequest(url));
}

void DOnlineTranslator::slotParseBingCredentials()
{
    m_currentReply->deleteLater();

    if (m_currentReply->error() != QNetworkReply::NoError)
    {
        resetData(NetworkError, m_currentReply->errorString());
        return;
    }

    const QByteArray webSiteData            = m_currentReply->readAll();
    const QByteArray credentialsBeginString = "var params_RichTranslateHelper = [";
    const int credentialsBeginPos           = webSiteData.indexOf(credentialsBeginString);

    if (credentialsBeginPos == -1)
    {
        resetData(ParsingError, i18n("Error: Unable to find Bing credentials in web version."));
        return;
    }

    const int keyBeginPos = credentialsBeginPos + credentialsBeginString.size();
    const int keyEndPos   = webSiteData.indexOf(',', keyBeginPos);

    if (keyEndPos == -1)
    {
        resetData(ParsingError, i18n("Error: Unable to extract Bing key from web version."));
        return;
    }

    s_bingKey               = webSiteData.mid(keyBeginPos, keyEndPos - keyBeginPos);
    const int tokenBeginPos = keyEndPos + 2; // Skip two symbols instead of one because the value is enclosed in quotes
    const int tokenEndPos   = webSiteData.indexOf('"', tokenBeginPos);

    if (tokenEndPos == -1)
    {
        resetData(ParsingError, i18n("Error: Unable to extract Bing token from web version."));
        return;
    }

    s_bingToken          = webSiteData.mid(tokenBeginPos, tokenEndPos - tokenBeginPos);
    const int igBeginPos = webSiteData.indexOf("IG");
    const int igEndPos   = webSiteData.indexOf('"', igBeginPos + 2);

    if (igEndPos == -1)
    {
        resetData(ParsingError, i18n("Error: Unable to extract additional Bing information from web version."));
        return;
    }

    s_bingIg              = QString::fromUtf8(webSiteData.mid(igBeginPos, igEndPos - igBeginPos));
    const int iidBeginPos = webSiteData.indexOf("data-iid");
    const int iidEndPos   = webSiteData.indexOf('"', iidBeginPos + 2);

    if (iidEndPos == -1)
    {
        resetData(ParsingError, i18n("Error: Unable to extract additional Bing information from web version."));
        return;
    }

    s_bingIid = QString::fromUtf8(webSiteData.mid(iidBeginPos, iidEndPos - iidBeginPos));
}

void DOnlineTranslator::slotRequestBingTranslate()
{
    const QString sourceText = sender()->property(s_textProperty).toString();

    // Generate POST data

    const QByteArray postData = "&text="     + QUrl::toPercentEncoding(sourceText)
                              + "&fromLang=" + languageApiCode(Bing, m_sourceLang).toUtf8()
                              + "&to="       + languageApiCode(Bing, m_translationLang).toUtf8()
                              + "&token="    + s_bingToken
                              + "&key="      + s_bingKey;

    QUrl url(QStringLiteral("https://www.bing.com/ttranslatev3"));
    url.setQuery(QStringLiteral("IG=%1&IID=%2").arg(s_bingIg, s_bingIid));

    // Setup request

    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader, QLatin1String("application/x-www-form-urlencoded"));
    request.setHeader(QNetworkRequest::UserAgentHeader, QString::fromUtf8("%1/%2").arg(QCoreApplication::applicationName()).arg(QCoreApplication::applicationVersion()));
    request.setUrl(url);

    // Make reply

    m_currentReply = m_networkManager->post(request, postData);
}

void DOnlineTranslator::slotParseBingTranslate()
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
    const QJsonObject responseObject = jsonResponse.array().first().toObject();

    if (m_sourceLang == Auto)
    {
        const QString langCode = responseObject.value(QStringLiteral("detectedLanguage")).toObject().value(QStringLiteral("language")).toString();
        m_sourceLang           = language(Bing, langCode);

        if (m_sourceLang == NoLanguage)
        {
            resetData(ParsingError, i18n("Error: Unable to parse autodetected language"));
            return;
        }

        if (m_onlyDetectLanguage)
            return;
    }

    const QJsonObject translationsObject = responseObject.value(QStringLiteral("translations")).toArray().first().toObject();
    m_translation                       += translationsObject.value(QStringLiteral("text")).toString();
    m_translationTranslit               += translationsObject.value(QStringLiteral("transliteration")).toObject().value(QStringLiteral("text")).toString();
}

void DOnlineTranslator::slotRequestBingDictionary()
{
    // Check if language is supported (need to check here because language may be autodetected)

    if (!isSupportDictionary(Bing, m_sourceLang, m_translationLang) && !m_source.contains(QLatin1Char(' ')))
    {
        auto* state = qobject_cast<QState *>(sender());
        state->addTransition(new QFinalState(state->parentState()));
        return;
    }

    // Generate POST data

    const QByteArray postData = "&text=" + QUrl::toPercentEncoding(sender()->property(s_textProperty).toString())
                              + "&from=" + languageApiCode(Bing, m_sourceLang).toUtf8()
                              + "&to="   + languageApiCode(Bing, m_translationLang).toUtf8();

    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader, QLatin1String("application/x-www-form-urlencoded"));
    request.setUrl(QUrl(QStringLiteral("https://www.bing.com/tlookupv3")));

    m_currentReply = m_networkManager->post(request, postData);
}

void DOnlineTranslator::slotParseBingDictionary()
{
    m_currentReply->deleteLater();

    // Check for errors

    if (m_currentReply->error() != QNetworkReply::NoError)
    {
        resetData(NetworkError, m_currentReply->errorString());
        return;
    }

    const QJsonDocument jsonResponse = QJsonDocument::fromJson(m_currentReply->readAll());
    const QJsonObject responseObject = jsonResponse.array().first().toObject();

    for (const QJsonValueRef dictionaryData : responseObject.value(QStringLiteral("translations")).toArray())
    {
        const QJsonObject dictionaryObject = dictionaryData.toObject();
        const QString typeOfSpeech         = dictionaryObject.value(QStringLiteral("posTag")).toString().toLower();
        const QString word                 = dictionaryObject.value(QStringLiteral("displayTarget")).toString().toLower();
        const QJsonArray translationsArray = dictionaryObject.value(QStringLiteral("backTranslations")).toArray();
        QStringList translations;
        translations.reserve(translationsArray.size());

        for (const QJsonValue &wordTranslation : translationsArray)
        {
            translations.append(wordTranslation.toObject().value(QStringLiteral("displayText")).toString());
        }

        m_translationOptions[typeOfSpeech].append({word, {}, translations});
    }
}

void DOnlineTranslator::buildBingStateMachine()
{
    // States

    auto* credentialsState = new QState(m_stateMachine); // Generate credentials from web version first to access API
    auto* translationState = new QState(m_stateMachine);
    auto* dictionaryState  = new QState(m_stateMachine);
    auto* finalState       = new QFinalState(m_stateMachine);
    m_stateMachine->setInitialState(credentialsState);

    // Transitions

    credentialsState->addTransition(credentialsState, &QState::finished, translationState);
    translationState->addTransition(translationState, &QState::finished, dictionaryState);
    dictionaryState->addTransition(dictionaryState, &QState::finished, finalState);

    // Setup credentials state

    if (s_bingKey.isEmpty() || s_bingToken.isEmpty())
    {
        buildNetworkRequestState(credentialsState,
                                 &DOnlineTranslator::slotRequestBingCredentials,
                                 &DOnlineTranslator::slotParseBingCredentials);
    }
    else
    {
        credentialsState->setInitialState(new QFinalState(credentialsState));
    }

    // Setup translation state

    buildSplitNetworkRequest(translationState,
                             &DOnlineTranslator::slotRequestBingTranslate,
                             &DOnlineTranslator::slotParseBingTranslate,
                             m_source,
                             s_bingTranslateLimit);

    // Setup dictionary state

    if (m_translationOptionsEnabled && !isContainsSpace(m_source))
    {
        buildNetworkRequestState(dictionaryState,
                                 &DOnlineTranslator::slotRequestBingDictionary,
                                 &DOnlineTranslator::slotParseBingDictionary,
                                 m_source);
    }
    else
    {
        dictionaryState->setInitialState(new QFinalState(dictionaryState));
    }
}

void DOnlineTranslator::buildBingDetectStateMachine()
{
    // States

    auto* detectState = new QState(m_stateMachine);
    auto* finalState  = new QFinalState(m_stateMachine);
    m_stateMachine->setInitialState(detectState);

    detectState->addTransition(detectState, &QState::finished, finalState);

    // Setup translation state

    const QString text = m_source.left(getSplitIndex(m_source, s_bingTranslateLimit));

    buildNetworkRequestState(detectState,
                             &DOnlineTranslator::slotRequestBingTranslate,
                             &DOnlineTranslator::slotParseBingTranslate,
                             text);
}

} // namespace Digikam
