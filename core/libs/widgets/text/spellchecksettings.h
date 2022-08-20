/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2022-08-16
 * Description : central place for Spell-check and localize settings
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

#ifndef DIGIKAM_SPELL_CHECK_SETTINGS_H
#define DIGIKAM_SPELL_CHECK_SETTINGS_H

// Qt includes

#include <QObject>

// Local includes

#include "digikam_export.h"
#include "spellcheckcontainer.h"

namespace Digikam
{

class DIGIKAM_EXPORT SpellCheckSettings : public QObject
{
    Q_OBJECT

public:

    /**
     * Global container for spell-check settings. All accessor methods are thread-safe.
     */
    static SpellCheckSettings* instance();

    /**
     * Returns the current Metadata settings.
     */
    SpellCheckContainer settings() const;

    /**
     * Sets the current Metadata settings and writes them to config.
     */
    void setSettings(const SpellCheckContainer& settings);

Q_SIGNALS:

    void signalSettingsChanged();

private:

    // Disabled
    SpellCheckSettings();
    explicit SpellCheckSettings(QObject*);
    ~SpellCheckSettings() override;

    void readFromConfig();

private:

    class Private;
    Private* const d;

    friend class SpellCheckSettingsCreator;
};

} // namespace Digikam

#endif // DIGIKAM_SPELL_CHECK_SETTINGS_H
