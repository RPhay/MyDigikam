/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2018-02-22
 * Description : A text translator using web-services - LibreTranslate methods.
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

void DOnlineTranslator::slotRequestLibreLangDetection()
{
    const QString sourceText  = sender()->property(s_textProperty).toString();

    // Generate POST data

    const QByteArray postData = "&q="       + QUrl::toPercentEncoding(sourceText)
                              + "&api_key=" + m_libreApiKey;

    // Setup request

    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader, QLatin1String("application/x-www-form-urlencoded"));
    request.setUrl(QUrl(QString::fromUtf8("%1/detect").arg(m_libreUrl)));

    // Make reply

    m_currentReply            = m_networkManager->post(request, postData);
}

void DOnlineTranslator::slotParseLibreLangDetection()
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

    if (m_sourceLang == Auto)
    {
        const QString langCode = responseObject.value(QStringLiteral("language")).toString();
        m_sourceLang           = language(LibreTranslate, langCode);

        if (m_sourceLang == NoLanguage)
        {
            resetData(ParsingError, i18n("Error: Unable to parse autodetected language"));
        }
    }
}

void DOnlineTranslator::slotRequestLibreTranslate()
{
    const QString sourceText  = sender()->property(s_textProperty).toString();

    // Generate POST data

    const QByteArray postData = "&q="       + QUrl::toPercentEncoding(sourceText)
                              + "&source="  + languageApiCode(LibreTranslate, m_sourceLang).toUtf8()
                              + "&target="  + languageApiCode(LibreTranslate, m_translationLang).toUtf8()
                              + "&api_key=" + m_libreApiKey;

    // Setup request

    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader, QLatin1String("application/x-www-form-urlencoded"));
    request.setUrl(QUrl(QString::fromUtf8("%1/translate").arg(m_libreUrl)));

    // Make reply

    m_currentReply            = m_networkManager->post(request, postData);
}

void DOnlineTranslator::slotParseLibreTranslate()
{
    m_currentReply->deleteLater();

    // Check for errors

    if (m_currentReply->error() != QNetworkReply::NoError)
    {
        resetData(NetworkError, m_currentReply->errorString());
        return;
    }

    const QJsonDocument jsonResponse = QJsonDocument::fromJson(m_currentReply->readAll());
    const QJsonObject responseObject = jsonResponse.object();
    m_translation                    = responseObject.value(QStringLiteral("translatedText")).toString();
}

void DOnlineTranslator::buildLibreDetectStateMachine()
{
    // States

    auto* detectState = new QState(m_stateMachine);
    auto* finalState  = new QFinalState(m_stateMachine);
    m_stateMachine->setInitialState(detectState);

    detectState->addTransition(detectState, &QState::finished, finalState);

    // Setup lang detection state

    const QString text = m_source.left(getSplitIndex(m_source, s_libreTranslateLimit));

    buildNetworkRequestState(detectState,
                             &DOnlineTranslator::slotRequestLibreLangDetection,
                             &DOnlineTranslator::slotParseLibreLangDetection,
                             text);
}

void DOnlineTranslator::buildLibreStateMachine()
{
    // States

    auto* languageDetectionState = new QState(m_stateMachine);
    auto* translationState       = new QState(m_stateMachine);
    auto* finalState             = new QFinalState(m_stateMachine);
    m_stateMachine->setInitialState(languageDetectionState);

    // Transitions

    languageDetectionState->addTransition(languageDetectionState, &QState::finished, translationState);
    translationState->addTransition(translationState, &QState::finished, finalState);

    // Setup LibreTranslate lang code detection

    buildNetworkRequestState(languageDetectionState,
                             &DOnlineTranslator::slotRequestLibreLangDetection,
                             &DOnlineTranslator::slotParseLibreLangDetection,
                             m_source);

    // Setup translation state

    buildSplitNetworkRequest(translationState,
                             &DOnlineTranslator::slotRequestLibreTranslate,
                             &DOnlineTranslator::slotParseLibreTranslate,
                             m_source,
                             s_libreTranslateLimit);
}

} // namespace Digikam
