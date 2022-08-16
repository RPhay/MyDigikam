/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2022-08-16
 * Description : Spell-chec Settings Container.
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

// Qt includes

#include <QStringList>

// KDE includes

#include <kconfiggroup.h>

// Local includes

#include "spellchecksettings.h"

namespace Digikam
{

SpellCheckContainer::SpellCheckContainer()
    : isActive            (false)
{
}

SpellCheckContainer::~SpellCheckContainer()
{
}

void SpellCheckContainer::readFromConfig(KConfigGroup& group)
{
    isActive = group.readEntry("Is Active", false);
}

void SpellCheckContainer::writeToConfig(KConfigGroup& group) const
{
    group.writeEntry("Is Active", isActive);
}

QDebug operator<<(QDebug dbg, const SpellCheckContainer& inf)
{
    dbg.nospace() << "[SpellCheckContainer] isActive("
                  << inf.isActive << "), ";
    return dbg.space();
}

} // namespace Digikam
