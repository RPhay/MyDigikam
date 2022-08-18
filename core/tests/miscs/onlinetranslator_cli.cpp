/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-01-05
 * Description : an unit test to check text online-translators.
 *
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

// Qt includes

#include <QCoreApplication>
#include <QTest>
#include <QSignalSpy>
#include <QObject>

// Local includes

#include "digikam_debug.h"
#include "donlinetranslator.h"

using namespace Digikam;

bool do_online_translation(const QString& text,
                           DOnlineTranslator::Engine engine,
                           DOnlineTranslator::Language trLang,
                           DOnlineTranslator::Language srcLang)
{
    qCDebug(DIGIKAM_TESTS_LOG) << "-----------------------------------------------------:";
    qCDebug(DIGIKAM_TESTS_LOG) << "Text to translate   :" << text;
    qCDebug(DIGIKAM_TESTS_LOG) << "With WebService     :" << engine;
    qCDebug(DIGIKAM_TESTS_LOG) << "To target language  :" << trLang;
    qCDebug(DIGIKAM_TESTS_LOG) << "With source language:" << srcLang;

    DOnlineTranslator* const trengine = new DOnlineTranslator;

    QSignalSpy spy(trengine, SIGNAL(signalFinished()));

    trengine->translate(text,       // String to translate
                        engine,     // Web service
                        trLang,     // Target language
                        srcLang,    // Source langage
                        DOnlineTranslator::Auto);

    bool b = spy.wait(3000);

    if (b)
    {
        qCDebug(DIGIKAM_TESTS_LOG) << "Translated text     :" << trengine->translation();
    }
    else
    {
        qCDebug(DIGIKAM_TESTS_LOG) << "Time-out with online translator";
    }

    delete trengine;

    return b;
}

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);

    do_online_translation(QString::fromUtf8("Ma grande maison rouge au bord de la plage"),
                          DOnlineTranslator::Google,
                          DOnlineTranslator::English,
                          DOnlineTranslator::Auto);

    do_online_translation(QString::fromUtf8("I was seen a big light on the sky during this night"),
                          DOnlineTranslator::Google,
                          DOnlineTranslator::French,
                          DOnlineTranslator::Auto);

    do_online_translation(QString::fromUtf8("Bienvenidos a mi casa, les presento a mi familia"),
                          DOnlineTranslator::Google,
                          DOnlineTranslator::German,
                          DOnlineTranslator::Spanish);

    do_online_translation(QString::fromUtf8("Non parlo italiano molto bene"),
                          DOnlineTranslator::Google,
                          DOnlineTranslator::TraditionalChinese,
                          DOnlineTranslator::Italian);

    return 0;
}
