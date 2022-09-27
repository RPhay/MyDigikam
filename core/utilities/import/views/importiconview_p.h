/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-25-07
 * Description : Private Qt item view for images
 *
 * SPDX-FileCopyrightText:      2012 by Islam Wazery <wazery at ubuntu dot com>
 * SPDX-FileCopyrightText: 2012-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_IMPORT_ICON_VIEW_PRIVATE_H
#define DIGIKAM_IMPORT_ICON_VIEW_PRIVATE_H

// Qt includes

#include <QObject>

// Local includes

#include "importiconview.h"
#include "importdelegate.h"
#include "importsettings.h"
#include "importoverlays.h"

namespace Digikam
{

class ImportNormalDelegate;

class Q_DECL_HIDDEN ImportIconView::Private : public QObject
{
    Q_OBJECT
    Q_DECLARE_PUBLIC(ImportIconView)

public:

    explicit Private(ImportIconView* const qq);
    ~Private() override;

    void updateOverlays();

public:

    ItemViewUtilities*     utilities;
    ImportNormalDelegate*  normalDelegate;

    bool                   overlaysActive;

    ImportRotateOverlay*   rotateLeftOverlay;
    ImportRotateOverlay*   rotateRightOverlay;

private:

    ImportIconView*        q_ptr;
};

} // namespace Digikam

#endif // DIGIKAM_IMPORT_ICON_VIEW_PRIVATE_H
