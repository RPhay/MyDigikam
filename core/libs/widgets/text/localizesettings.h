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

#ifndef DIGIKAM_LOCALIZE_SETTINGS_H
#define DIGIKAM_LOCALIZE_SETTINGS_H

// Qt includes

#include <QObject>

// Local includes

#include "digikam_export.h"
#include "localizecontainer.h"

namespace Digikam
{

class DIGIKAM_EXPORT LocalizeSettings : public QObject
{
    Q_OBJECT

public:

    enum ConfigPart
    {
        LocalizeConfig,
        SpellCheckConfig,
        AllConfig
    };

public:

    /**
     * Global container for spell-check and localize settings. All accessor methods are thread-safe.
     */
    static LocalizeSettings* instance();

    /**
     * Returns the current Metadata settings.
     */
    LocalizeContainer settings() const;

    /**
     * Sets the current Metadata settings and writes them to config.
     */
    void setSettings(const LocalizeContainer& settings, ConfigPart config);

    void openLocalizeSetup();

Q_SIGNALS:

    void signalSettingsChanged();
    void signalOpenLocalizeSetup();

private:

    // Disabled
    LocalizeSettings();
    explicit LocalizeSettings(QObject*);
    ~LocalizeSettings() override;

    void readFromConfig();

private:

    class Private;
    Private* const d;

    friend class LocalizeSettingsCreator;
};

} // namespace Digikam

#endif // DIGIKAM_LOCALIZE_SETTINGS_H
