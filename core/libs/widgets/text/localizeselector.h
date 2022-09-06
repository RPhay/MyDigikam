/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-06-15
 * Description : localize selector widget
 *
 * Copyright (C) 2009-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_LOCALIZE_SELECTOR_H
#define DIGIKAM_LOCALIZE_SELECTOR_H

// Qt includes

#include <QWidget>
#include <QString>

// Local includes

#include "digikam_export.h"
#include "dlayoutbox.h"

class QListWidgetItem;

namespace Digikam
{

class DIGIKAM_EXPORT LocalizeSelector : public DHBox
{
    Q_OBJECT

public:

    explicit LocalizeSelector(QWidget* const parent);
    ~LocalizeSelector()                               override;

    void populateTranslationEntries();

Q_SIGNALS:

    void signalTranslate(const QString& lang);

private Q_SLOTS:

    void slotLocalizeChanged();
    void slotOpenLocalizeSetup();
    void slotTranslate(QListWidgetItem*);

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_LOCALIZE_SELECTOR_H
