/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2023-12-21
 * Description : Marble Settings.
 *
 * SPDX-FileCopyrightText: 2022-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_MARBLE_SETTINGS_H
#define DIGIKAM_MARBLE_SETTINGS_H

// Qt includes

#include <QObject>

// Local includes

#include "digikam_export.h"
#include "marblesettingscontainer.h"
#include "setupgeolocation.h"

namespace Marble
{
    class MarbleWidget;
}

using namespace Marble;

namespace Digikam
{

class DIGIKAM_EXPORT MarbleSettings : public QObject
{
    Q_OBJECT

public:

    /**
     * Global container for Metadata settings. All accessor methods are thread-safe.
     */
    static MarbleSettings* instance();

    /**
     * Returns the current Metadata settings.
     */
    MarbleSettingsContainer settings() const;

    /**
     * Sets the current Metadata settings and writes them to config.
     */
    void setSettings(const MarbleSettingsContainer& settings);

    /**
     * Store one MarbleWidget instance in the collection.
     */
    void registerWidget(MarbleWidget* const widget);

    /**
     * Return the first registered MarbleWidget instance stored in the collection.
     * If no valid instance is found, nullptr is returned.
     */
    MarbleWidget* mainMarbleWidget() const;

    void openSetupGeolocation(SetupGeolocation::GeolocationTab tab);

Q_SIGNALS:

    void signalSettingsChanged();

    void signalMarbleSettingsChanged(const MarbleSettingsContainer& current,
                                     const MarbleSettingsContainer& previous);

    void signalSetupGeolocation(int tab);

private:

    // Disabled
    MarbleSettings();
    explicit MarbleSettings(QObject*);
    ~MarbleSettings() override;

    void readFromConfig();

    void applySettingsToWidgets(const MarbleSettingsContainer& settings);

private:

    class Private;
    Private* const d;

    friend class MarbleSettingsCreator;
};

} // namespace Digikam

#endif // DIGIKAM_MARBLE_SETTINGS_H
