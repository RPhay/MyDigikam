/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2022-08-20
 * Description : Localization of Strings Config widget.
 *
 * Copyright (C) 2021-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_LOCALIZE_CONFIG_H
#define DIGIKAM_LOCALIZE_CONFIG_H

// Qt includes

#include <QWidget>

// Local includes

#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_EXPORT LocalizeConfig : public QWidget
{
    Q_OBJECT

public:

    explicit LocalizeConfig(QWidget* const parent = nullptr);
    ~LocalizeConfig() override;

    void applySettings();
    void readSettings();

private Q_SLOTS:

    void slotTranslatorChanged();

private:

    void populateAltLanguages();
    void populateTranslatorLanguages();

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_LOCALIZE_CONFIG_H
