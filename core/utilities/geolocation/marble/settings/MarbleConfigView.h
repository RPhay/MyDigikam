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

class DIGIKAM_EXPORT MarbleConfigView : public QTabWidget
{
    Q_OBJECT

public:

    explicit MarbleConfigView(MarbleWidget* const marbleWidget,
                              QWidget* const parent = nullptr );
    ~MarbleConfigView() override;

    void applySettings();

Q_SIGNALS:

    /**
     * The user clicked on the button to clear volatile tile cache.
     */
    void clearVolatileCacheClicked();

    /**
     * The user clicked on the button to clear persistent tile cache.
     */
    void clearPersistentCacheClicked();

private:

    void readSettings();

private:

    Q_DISABLE_COPY( MarbleConfigView )

    class Private;
    Private* const d;
};

} // namespace Marble

#endif // DIGIKAM_MARBLE_CONFIG_VIEW_H
