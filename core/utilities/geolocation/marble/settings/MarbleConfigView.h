// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Bastian Holst <bastianholst@gmx.de>
//

#ifndef MARBLE_CONFIG_VIEW_H
#define MARBLE_CONFIG_VIEW_H

// Qt includes

#include <QTabWidget>

// Local includes

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
                              QWidget* const parent = nullptr);
    ~MarbleConfigView() override;

    void readSettings();
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

    Q_DISABLE_COPY(MarbleConfigView)

    class Private;
    Private* const d;
};

} // namespace Marble

#endif // MARBLE_CONFIG_VIEW_H
