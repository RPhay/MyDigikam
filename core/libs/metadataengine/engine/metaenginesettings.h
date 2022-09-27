/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-08-20
 * Description : central place for MetaEngine settings
 *
 * SPDX-FileCopyrightText: 2010-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_META_ENGINE_SETTINGS_H
#define DIGIKAM_META_ENGINE_SETTINGS_H

// Qt includes

#include <QObject>

// Local includes

#include "digikam_export.h"
#include "metaenginesettingscontainer.h"

namespace Digikam
{

class DIGIKAM_EXPORT MetaEngineSettings : public QObject
{
    Q_OBJECT

public:

    /**
     * Global container for Metadata settings. All accessor methods are thread-safe.
     */
    static MetaEngineSettings* instance();

    /**
     * Returns the current Metadata settings.
     */
    MetaEngineSettingsContainer settings() const;

    /**
     * Sets the current Metadata settings and writes them to config.
     */
    void setSettings(const MetaEngineSettingsContainer& settings);

    /**
     * Shortcut to get exif rotation settings from container.
     */
    bool exifRotate()                      const;

Q_SIGNALS:

    void signalSettingsChanged();

    void signalMetaEngineSettingsChanged(const MetaEngineSettingsContainer& current,
                                         const MetaEngineSettingsContainer& previous);

private:

    // Disabled
    MetaEngineSettings();
    explicit MetaEngineSettings(QObject*);
    ~MetaEngineSettings() override;

    void readFromConfig();

private:

    class Private;
    Private* const d;

    friend class MetaEngineSettingsCreator;
};

} // namespace Digikam

#endif // DIGIKAM_META_ENGINE_SETTINGS_H
