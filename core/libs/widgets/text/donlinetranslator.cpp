/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2018-02-22
 * Description : A text translator using web-services.
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

DOnlineTranslator::DOnlineTranslator(QObject* const parent)
    : QObject         (parent),
      m_stateMachine  (new QStateMachine(this)),
      m_networkManager(new QNetworkAccessManager(this)),
      m_libreUrl      (QLatin1String("https://translate.argosopentech.com")),
      m_lingvaUrl     (QLatin1String("https://lingva.ml"))
{
    connect(m_stateMachine, &QStateMachine::finished,
            this, &DOnlineTranslator::signalFinished);

    connect(m_stateMachine, &QStateMachine::stopped,
            this, &DOnlineTranslator::signalFinished);
}

void DOnlineTranslator::translate(const QString& text,
                                  Engine engine,
                                  Language translationLang,
                                  Language sourceLang,
                                  Language uiLang)
{
    abort();
    resetData();

    m_onlyDetectLanguage = false;
    m_source             = text;
    m_sourceLang         = sourceLang;
    m_translationLang    = (translationLang == Auto) ? language(QLocale()) : translationLang;
    m_uiLang             = (uiLang == Auto)          ? language(QLocale()) : uiLang;

    // Check if the selected languages are supported by the engine
 
   if (!isSupportTranslation(engine, m_sourceLang))
    {
        resetData(ParametersError,
                  i18n("Selected source language %1 is not supported for %2",
                       languageName(m_sourceLang),
                       QString::fromUtf8(QMetaEnum::fromType<Engine>().valueToKey(engine))));

        Q_EMIT signalFinished();

        return;
    }

    if (!isSupportTranslation(engine, m_translationLang))
    {
        resetData(ParametersError,
                  i18n("Selected translation language %1 is not supported for %2",
                       languageName(m_translationLang),
                       QString::fromUtf8(QMetaEnum::fromType<Engine>().valueToKey(engine))));

        Q_EMIT signalFinished();

        return;
    }

    if (!isSupportTranslation(engine, m_uiLang))
    {
        resetData(ParametersError,
                  i18n("Selected ui language %1 is not supported for %2",
                       languageName(m_uiLang),
                       QString::fromUtf8(QMetaEnum::fromType<Engine>().valueToKey(engine))));

        Q_EMIT signalFinished();

        return;
    }

    switch (engine)
    {
        case Google:
        {
            buildGoogleStateMachine();
            break;
        }

        case Yandex:
        {
            buildYandexStateMachine();
            break;
        }

        case Bing:
        {
            buildBingStateMachine();
            break;
        }

        case LibreTranslate:
        {
            if (m_libreUrl.isEmpty())
            {
                resetData(ParametersError,
                        i18n("%1 URL can't be empty.",
                             QString::fromUtf8(QMetaEnum::fromType<Engine>().valueToKey(engine))));

                Q_EMIT signalFinished();

                return;
            }

            buildLibreStateMachine();
            break;
        }

        case Lingva:
        {
            if (m_lingvaUrl.isEmpty())
            {
                resetData(ParametersError,
                        i18n("%1 URL can't be empty.",
                             QString::fromUtf8(QMetaEnum::fromType<Engine>().valueToKey(engine))));

                Q_EMIT signalFinished();

                return;
            }

            buildLingvaStateMachine();
            break;
        }
    }

    m_stateMachine->start();
}

QString DOnlineTranslator::engineName(Engine engine)
{
    switch (engine)
    {
        case Yandex:
        {
            return QLatin1String("Yandex");
        }

        case Bing:
        {
            return QLatin1String("Bing");
        }

        case LibreTranslate:
        {
            return QLatin1String("Libre Translate");
        }

        case Lingva:
        {
            return QLatin1String("Lingva");
        }

        default:
        {
            return QLatin1String("Google");
        }
    }
}

void DOnlineTranslator::detectLanguage(const QString& text, Engine engine)
{
    abort();
    resetData();

    m_onlyDetectLanguage = true;
    m_source             = text;
    m_sourceLang         = Auto;
    m_translationLang    = English;
    m_uiLang             = language(QLocale());

    switch (engine)
    {
        case Google:
        {
            buildGoogleDetectStateMachine();
            break;
        }

        case Yandex:
        {
            buildYandexDetectStateMachine();
            break;
        }

        case Bing:
        {
            buildBingDetectStateMachine();
            break;
        }

        case LibreTranslate:
        {
            if (m_libreUrl.isEmpty())
            {
                resetData(ParametersError,
                        i18n("%1 URL can't be empty.",
                             QString::fromUtf8(QMetaEnum::fromType<Engine>().valueToKey(engine))));

                Q_EMIT signalFinished();

                return;
            }

            buildLibreDetectStateMachine();
            break;
        }

        case Lingva:
        {
            if (m_lingvaUrl.isEmpty())
            {
                resetData(ParametersError,
                        i18n("%1 URL can't be empty.",
                             QString::fromUtf8(QMetaEnum::fromType<Engine>().valueToKey(engine))));

                Q_EMIT signalFinished();

                return;
            }

            buildLingvaDetectStateMachine();
            break;
        }
    }

    m_stateMachine->start();
}

void DOnlineTranslator::abort()
{
    if (m_currentReply != nullptr)
    {
        m_currentReply->abort();
    }
}

bool DOnlineTranslator::isRunning() const
{
    return m_stateMachine->isRunning();
}

void DOnlineTranslator::slotSkipGarbageText()
{
    m_translation.append(sender()->property(s_textProperty).toString());
}

void DOnlineTranslator::buildSplitNetworkRequest(QState* const parent,
                                                 void (DOnlineTranslator::*requestMethod)(),
                                                 void (DOnlineTranslator::*parseMethod)(),
                                                 const QString& text,
                                                 int textLimit)
{
    QString unsendedText       = text;
    auto* nextTranslationState = new QState(parent);
    parent->setInitialState(nextTranslationState);

    while (!unsendedText.isEmpty())
    {
        auto* currentTranslationState = nextTranslationState;
        nextTranslationState          = new QState(parent);

        // Do not translate the part if it looks like garbage

        const int splitIndex          = getSplitIndex(unsendedText, textLimit);

        if (splitIndex == -1)
        {
            currentTranslationState->setProperty(s_textProperty, unsendedText.left(textLimit));
            currentTranslationState->addTransition(nextTranslationState);

            connect(currentTranslationState, &QState::entered,
                    this, &DOnlineTranslator::slotSkipGarbageText);

            // Remove the parsed part from the next parsing

            unsendedText = unsendedText.mid(textLimit);
        }
        else
        {
            buildNetworkRequestState(currentTranslationState, requestMethod, parseMethod, unsendedText.left(splitIndex));
            currentTranslationState->addTransition(currentTranslationState, &QState::finished, nextTranslationState);

            // Remove the parsed part from the next parsing

            unsendedText = unsendedText.mid(splitIndex);
        }
    }

    nextTranslationState->addTransition(new QFinalState(parent));
}

void DOnlineTranslator::buildNetworkRequestState(QState* const parent,
                                                 void (DOnlineTranslator::*requestMethod)(),
                                                 void (DOnlineTranslator::*parseMethod)(),
                                                 const QString& text)
{
    // Network substates

    auto* requestingState = new QState(parent);
    auto* parsingState    = new QState(parent);

    parent->setInitialState(requestingState);

    // Substates transitions

    requestingState->addTransition(m_networkManager, &QNetworkAccessManager::finished, parsingState);
    parsingState->addTransition(new QFinalState(parent));

    // Setup requesting state

    requestingState->setProperty(s_textProperty, text);

    connect(requestingState, &QState::entered,
            this, requestMethod);

    // Setup parsing state

    connect(parsingState, &QState::entered,
            this, parseMethod);
}

void DOnlineTranslator::resetData(TranslationError error, const QString& errorString)
{
    m_error       = error;
    m_errorString = errorString;
    m_translation.clear();
    m_translationTranslit.clear();
    m_sourceTranslit.clear();
    m_sourceTranscription.clear();
    m_translationOptions.clear();

    m_stateMachine->stop();

    for (QAbstractState* state : m_stateMachine->findChildren<QAbstractState*>())
    {
        if (!m_stateMachine->configuration().contains(state))
        {
            state->deleteLater();
        }
    }
}

QString DOnlineTranslator::languageApiCode(Engine engine, Language lang)
{
    if (!isSupportTranslation(engine, lang))
    {
        return QString();
    }

    switch (engine)
    {
        case Google:
        {
            return s_googleLanguageCodes.value(lang, s_genericLanguageCodes.value(lang));
        }

        case Yandex:
        {
            return s_yandexLanguageCodes.value(lang, s_genericLanguageCodes.value(lang));
        }

        case Bing:
        {
            return s_bingLanguageCodes.value(lang, s_genericLanguageCodes.value(lang));
        }

        case LibreTranslate:
        {
            return s_genericLanguageCodes.value(lang);
        }

        case Lingva:
        {
            return s_lingvaLanguageCodes.value(lang, s_genericLanguageCodes.value(lang));
        }
    }

    Q_UNREACHABLE();
}

DOnlineTranslator::Language DOnlineTranslator::language(Engine engine, const QString& langCode)
{
    // Engine exceptions

    switch (engine)
    {
        case Google:
        {
            return s_googleLanguageCodes.key(langCode, s_genericLanguageCodes.key(langCode, NoLanguage));
        }

        case Yandex:
        {
            return s_yandexLanguageCodes.key(langCode, s_genericLanguageCodes.key(langCode, NoLanguage));
        }

        case Bing:
        {
            return s_bingLanguageCodes.key(langCode, s_genericLanguageCodes.key(langCode, NoLanguage));
        }

        case LibreTranslate:
        {
            return s_genericLanguageCodes.key(langCode, NoLanguage);
        }

        case Lingva:
        {
            return s_lingvaLanguageCodes.key(langCode, s_genericLanguageCodes.key(langCode, NoLanguage));
        }
    }

    Q_UNREACHABLE();
}

int DOnlineTranslator::getSplitIndex(const QString& untranslatedText, int limit)
{
    if (untranslatedText.size() < limit)
    {
        return limit;
    }

    int splitIndex = untranslatedText.lastIndexOf(QLatin1String(". "), limit - 1);

    if (splitIndex != -1)
    {
        return splitIndex + 1;
    }

    splitIndex = untranslatedText.lastIndexOf(QLatin1Char(' '), limit - 1);

    if (splitIndex != -1)
    {
        return splitIndex + 1;
    }

    splitIndex = untranslatedText.lastIndexOf(QLatin1Char('\n'), limit - 1);

    if (splitIndex != -1)
    {
        return splitIndex + 1;
    }

    // Non-breaking space

    splitIndex = untranslatedText.lastIndexOf(0x00a0, limit - 1);

    if (splitIndex != -1)
    {
        return splitIndex + 1;
    }

    // If the text has not passed any check and is most likely garbage

    return limit;
}

bool DOnlineTranslator::isContainsSpace(const QString& text)
{
    return std::any_of(text.cbegin(), text.cend(), [](QChar symbol)
        {
            return symbol.isSpace();
        }
    );
}

void DOnlineTranslator::addSpaceBetweenParts(QString& text)
{
    if (text.isEmpty())
    {
        return;
    }

#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)

    if (!text.back().isSpace())
    {

#else

    if (!text.at(text.size() - 1).isSpace())
    {

#endif

        text.append(QLatin1Char(' '));
    }
}

} // namespace Digikam
