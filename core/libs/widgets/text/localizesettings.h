/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2023-08-16
 * Description : central place for Spell-check and localize settings
 *
 * SPDX-FileCopyrightText: 2021-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
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
