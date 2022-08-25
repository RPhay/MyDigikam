/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2018-02-22
 * Description : A text translator using web-services - Private members.
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

#ifndef DIGIKAM_DONLINE_TRANSLATOR_P_H
#define DIGIKAM_DONLINE_TRANSLATOR_P_H

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

extern const QMap<DOnlineTranslator::Language, QString> s_genericLanguageCodes;
extern const QMap<DOnlineTranslator::Language, QString> s_googleLanguageCodes;
extern const QMap<DOnlineTranslator::Language, QString> s_yandexLanguageCodes;
extern const QMap<DOnlineTranslator::Language, QString> s_bingLanguageCodes;
extern const QMap<DOnlineTranslator::Language, QString> s_lingvaLanguageCodes;

extern const QMap<QString, QString>                     s_rfc3066LanguageCodesGeneric;
extern const QMap<QString, QString>                     s_rfc3066LanguageCodesGoogle;
extern const QMap<QString, QString>                     s_rfc3066LanguageCodesYandex;
extern const QMap<QString, QString>                     s_rfc3066LanguageCodesBing;
extern const QMap<QString, QString>                     s_rfc3066LanguageCodesLingva;

} // namespace Digikam

#endif // DIGIKAM_DONLINE_TRANSLATOR_P_H
