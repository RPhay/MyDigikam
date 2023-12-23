/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2023-12-21
 * Description : Marble Settings.
 *
 * SPDX-FileCopyrightText: 2022-2023 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

Q_SIGNALS:

    void signalSettingsChanged();

    void signalMarbleSettingsChanged(const MarbleSettingsContainer& current,
                                     const MarbleSettingsContainer& previous);

private:

    // Disabled
    MarbleSettings();
    explicit MarbleSettings(QObject*);
    ~MarbleSettings() override;

    void readFromConfig();

private:

    class Private;
    Private* const d;

    friend class MarbleSettingsCreator;
};

} // namespace Digikam

#endif // DIGIKAM_MARBLE_SETTINGS_H
