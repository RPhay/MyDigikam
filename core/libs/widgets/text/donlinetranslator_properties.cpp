/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2018-02-22
 * Description : A text translator using web-services - Properties methods.
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

bool DOnlineTranslator::isSourceTranslitEnabled() const
{
    return m_sourceTranslitEnabled;
}

void DOnlineTranslator::setSourceTranslitEnabled(bool enable)
{
    m_sourceTranslitEnabled = enable;
}

bool DOnlineTranslator::isTranslationTranslitEnabled() const
{
    return m_translationTranslitEnabled;
}

void DOnlineTranslator::setTranslationTranslitEnabled(bool enable)
{
    m_translationTranslitEnabled = enable;
}

bool DOnlineTranslator::isSourceTranscriptionEnabled() const
{
    return m_sourceTranscriptionEnabled;
}

void DOnlineTranslator::setSourceTranscriptionEnabled(bool enable)
{
    m_sourceTranscriptionEnabled = enable;
}

bool DOnlineTranslator::isTranslationOptionsEnabled() const
{
    return m_translationOptionsEnabled;
}

void DOnlineTranslator::setTranslationOptionsEnabled(bool enable)
{
    m_translationOptionsEnabled = enable;
}

void DOnlineTranslator::setEngineUrl(Engine engine, const QString& url)
{
    switch (engine)
    {
        case LibreTranslate:
            m_libreUrl  = url;
            break;

        case Lingva:
            m_lingvaUrl = url;
            break;

        default:
            break;
    }
}

void DOnlineTranslator::setEngineApiKey(Engine engine, const QByteArray& apiKey)
{
    switch (engine)
    {
        case LibreTranslate:
            m_libreApiKey = apiKey;
            break;

        default:
            break;
    }
}

QJsonDocument DOnlineTranslator::toJson() const
{
    QJsonObject translationOptions;

    for (auto it = m_translationOptions.cbegin() ; it != m_translationOptions.cend() ; ++it)
    {
        QJsonArray arr;

        for (const DOnlineTranslatorOption& option : it.value())
        {
            arr.append(option.toJson());
        }

        translationOptions.insert(it.key(), arr);
    }

    QJsonObject object
    {
        { QLatin1String("source"),               m_source                   },
        { QLatin1String("sourceTranscription"),  m_sourceTranscription      },
        { QLatin1String("sourceTranslit"),       m_sourceTranslit           },
        { QLatin1String("translation"),          m_translation              },
        { QLatin1String("translationOptions"),   qMove(translationOptions)  },
        { QLatin1String("translationTranslit"),  m_translationTranslit      },
    };

    return QJsonDocument(object);
}

QString DOnlineTranslator::source() const
{
    return m_source;
}

QString DOnlineTranslator::sourceTranslit() const
{
    return m_sourceTranslit;
}

QString DOnlineTranslator::sourceTranscription() const
{
    return m_sourceTranscription;
}

QString DOnlineTranslator::sourceLanguageName() const
{
    return languageName(m_sourceLang);
}

DOnlineTranslator::Language DOnlineTranslator::sourceLanguage() const
{
    return m_sourceLang;
}

QString DOnlineTranslator::translation() const
{
    return m_translation;
}

QString DOnlineTranslator::translationTranslit() const
{
    return m_translationTranslit;
}

QString DOnlineTranslator::translationLanguageName() const
{
    return languageName(m_translationLang);
}

DOnlineTranslator::Language DOnlineTranslator::translationLanguage() const
{
    return m_translationLang;
}

QMap<QString, QVector<DOnlineTranslatorOption>> DOnlineTranslator::translationOptions() const
{
    return m_translationOptions;
}

DOnlineTranslator::TranslationError DOnlineTranslator::error() const
{
    return m_error;
}

QString DOnlineTranslator::errorString() const
{
    return m_errorString;
}

QString DOnlineTranslator::languageCode(Language lang)
{
    return s_genericLanguageCodes.value(lang);
}

DOnlineTranslator::Language DOnlineTranslator::language(const QString& langCode)
{
    return s_genericLanguageCodes.key(langCode, NoLanguage);
}

} // namespace Digikam
