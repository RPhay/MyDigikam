/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2022-08-16
 * Description : Spell-check and localize Settings Container.
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

#include "metaenginesettingscontainer.h"

// KDE includes

#include <kconfiggroup.h>

// Local includes

#include "spellchecksettings.h"
#include "digikam_debug.h"

namespace Digikam
{

SpellCheckContainer::SpellCheckContainer()
    : enableSpellCheck(false),
      translatorEngine(DOnlineTranslator::Google)
{
}

SpellCheckContainer::~SpellCheckContainer()
{
}

void SpellCheckContainer::readFromConfig(KConfigGroup& group)
{
    enableSpellCheck = group.readEntry("EnableSpellCheck", false);

    ignoredWords     = group.readEntry("IgnoredWords", QStringList() << QLatin1String("digiKam")
                                                                     << QLatin1String("Showfoto")
                                                                     << QLatin1String("Apple")
                                                                     << QLatin1String("Nikon")
                                                                     << QLatin1String("Android")
                                                                     << QLatin1String("Canon")
                                                                     << QLatin1String("Sony")
                                                                     << QLatin1String("Samsung")
                                                                     << QLatin1String("Panasonic")
                                                                     << QLatin1String("Olympus")
                                                                     << QLatin1String("Exif")
                                                                     << QLatin1String("IPTC")
                                                                     << QLatin1String("Xmp"));
    ignoredWords.removeDuplicates();

    translatorEngine = (DOnlineTranslator::Engine)group.readEntry("TranslatorEngine", (int)DOnlineTranslator::Engine::Google);
    translatorLang   = group.readEntry("TranslatorLang", QStringList() << QLatin1String("en-US")
                                                                       << QLatin1String("fr-FR")
                                                                       << QLatin1String("es-ES")
                                                                       << QLatin1String("it-IT")
                                                                       << QLatin1String("de-DE")
                                                                       << QLatin1String("pt-PT")
    );
}

void SpellCheckContainer::writeToConfig(KConfigGroup& group) const
{
    group.writeEntry("EnableSpellCheck", enableSpellCheck);
    group.writeEntry("IgnoredWords",     ignoredWords);
    group.writeEntry("TranslatorEngine", (int)translatorEngine);
    group.writeEntry("TranslatorLang",   translatorLang);
    group.sync();
}

QDebug operator<<(QDebug dbg, const SpellCheckContainer& inf)
{
    dbg.nospace() << "[SpellCheckContainer] enableSpellCheck("
                  << inf.enableSpellCheck << "), "
                  << "ignoredWords("
                  << inf.ignoredWords << "), "
                  << "translatorEngine("
                  << inf.translatorEngine << "), "
                  << "translatorLang("
                  << inf.translatorLang << "), ";
    return dbg.space();
}

} // namespace Digikam
