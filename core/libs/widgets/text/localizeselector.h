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

#ifndef DIGIKAM_LOCALIZE_SELECTOR_H
#define DIGIKAM_LOCALIZE_SELECTOR_H

// Qt includes

#include <QWidget>
#include <QString>

// Local includes

#include "digikam_export.h"
#include "dlayoutbox.h"

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

    void signalTranslate();

private Q_SLOTS:

    void slotLocalizeChanged();
    void slotOpenLocalizeSetup();

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_LOCALIZE_SELECTOR_H
