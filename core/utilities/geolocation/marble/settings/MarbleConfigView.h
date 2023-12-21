// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Bastian Holst <bastianholst@gmx.de>
//

#ifndef DIGIKAM_MARBLE_CONFIG_VIEW_H
#define DIGIKAM_MARBLE_CONFIG_VIEW_H

#include <QTabWidget>

#include "MarbleGlobal.h"
#include "MarbleLocale.h"

#include "digikam_export.h"

namespace Marble
{

class MarbleWidget;

class MarbleConfigViewPrivate;

class DIGIKAM_EXPORT MarbleConfigView : public QTabWidget
{
    Q_OBJECT

public:

    explicit MarbleConfigView(MarbleWidget* const marbleWidget,
                              QWidget* const parent = nullptr );
    ~MarbleConfigView() override;

    // View Settings

    MarbleLocale::MeasurementSystem measurementSystem() const;
    Marble::AngleUnit angleUnit()                       const;
    void setAngleUnit(Marble::AngleUnit unit);
    Marble::MapQuality stillQuality()                   const;
    Marble::MapQuality animationQuality()               const;
    QFont mapFont()                                     const;
    bool inertialEarthRotation()                        const;
    bool mouseViewRotation()                            const;

    // Cache Settings

    int volatileTileCacheLimit()                        const;
    int persistentTileCacheLimit()                      const;
    QString proxyUrl()                                  const;
    int proxyPort()                                     const;

    QString proxyUser()                                 const;
    QString proxyPass()                                 const;
    bool proxyType()                                    const;
    bool proxyAuth()                                    const;

Q_SIGNALS:

    /**
     * This signal is emitted when the loaded settings were changed.
     * Either by the user or by loading them initially from disk.
     */
    void settingsChanged();

    /**
     * The user clicked on the button to clear volatile tile cache.
     */
    void clearVolatileCacheClicked();

    /**
     * The user clicked on the button to clear persistent tile cache.
     */
    void clearPersistentCacheClicked();

public Q_SLOTS:

    /**
     * Read settings and update interface.
     */
    void readSettings();

    /**
     * Write settings to disk.
     */
    void writeSettings();

private Q_SLOTS:

    /**
     * Synchronize the loaded settings with the file on hard disk.
     */
    void syncSettings();

private:

    Q_DISABLE_COPY( MarbleConfigView )

    MarbleConfigViewPrivate * const d;
};

} // namespace Marble

#endif // DIGIKAM_MARBLE_CONFIG_VIEW_H
